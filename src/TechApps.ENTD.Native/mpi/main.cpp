#include <string>
#include <deque>
#include <vector>
#include <array>
#include <tuple>
#include <math.h>
#include <stdio.h>
#include <cstring>
#include <ctime>
#include <time.h>
#include <omp.h>
#include <mpi.h>
#include <algorithm>
#include <unistd.h>
#include <sstream>
/** Pagosa Geo **/
#include <SegyData.h>
#include <SegyRead.h>
#include <SegyWrite.h>
#include <PspaException.h>
/** Intel MKL Library**/
//#include <mkl_vsl.h>
//#include <mkl.h>
#include <args.h>
#include "Entd.h"
#include "cubing.h"
#include <iostream>
#include <fstream>

using namespace std;
using namespace PagosaGeo;

const char *APP = "ENTD_HPC";

void PrintUsage()
{
	cout << "usage:\n";
	cout << APP << " <option> -i <file name>\n";
	cout << "option:\n";
	cout << "-h  print help\n";
	cout << "-v  <level> verbosity level 0: Error, 1: Warning, 2: Info \n";
	cout << "-o  <output_dir>, default . current dir\n";
	cout << "-u  <unit> default: 3\n";
}

int main(int argc, char *argv[])
{
	int status;
	int required = MPI_THREAD_SERIALIZED; //  Initialize MPI.
	int provided;
	int rank = 0;
	int num_procs = 0;
	status = MPI_Init_thread(&argc, &argv, required, &provided);
	status = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	status = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
	vector<char> hostname(100);
	gethostname(hostname.data(), hostname.size()); // resolve hostname

	InputParser input(argc, argv); // parse args
	if (input.cmdOptionExists("-h") || input.cmdOptionExists("--help") || argc == 1)
	{
		if (rank == 0)
			PrintUsage();
		MPI_Barrier(MPI_COMM_WORLD);
		MPI_Finalize();
		return 0;
	}

	int verbosity = atoi(input.getCmdOption("-v").data());
	logger(format("Proc %i, runs at %s", rank, hostname.data()), 2, verbosity);
	// Check the threading support level
	if (provided < required)
	{
		// Insufficient support, degrade to 1 thread and warn the user
		if (rank == 0)
			logger("MPI provides insufficient threading support", 1, verbosity);
		//omp_set_num_threads(1); TODO fix the compiler flags
	}

	
	// master process
	ENTD fn;
	if (input.cmdOptionExists("-u"))
		fn.Unit = atoi(input.getCmdOption("-u").data());

	int MAX_NUM_SMP = 0;
	if (rank == 0)
	{
		const string &filename = input.getCmdOption("-i");
		const string path = !input.cmdOptionExists("-o") ? "." : input.getCmdOption("-o");
		size_t found = filename.find_last_of("/\\");
		size_t found2 = filename.find_last_of(".");
		const string name = filename.substr(found + 1, found2 - found - 1);
		const string formatName = "%s/%s_%s_%i_Plane_%s.segy";
		vector<string> outfilename(10);
		for (size_t i = 0; i < outfilename.size() - 1; i++)
			outfilename[i] = format(formatName, path.data(), name.data(), APP, fn.Unit, to_string(i+1).c_str());
		outfilename[9] = format(formatName, path.data(), name.data(), APP, fn.Unit, "All");
		string infofile = format("%s/%s_%s_%i_density.csv", path.data(), name.data(), APP, fn.Unit);		
			
		timespec tstart, tend;
		uint64_t diff;
		clock_gettime(CLOCK_REALTIME, &tstart); // Works on Linux
		time_t startTime = time(NULL);

		try
		{
			std::vector<SegyWrite*> outputData(outfilename.size());
			SegyRead inputData (filename);
			logger(format("Input %s", filename.data()), 2, verbosity);
			for (int i = 0; i < outfilename.size(); i++)
			{
				outputData[i] = new SegyWrite(outfilename[i], inputData);
				outputData[i]->writeFileHeader();
			}

			auto numTrcsIn = inputData.numTrcsInFile();
			DataRange rng = inputData.range();
			auto dt = (float)rng.dSample();
			auto nSample = (int)rng.numSmps();
			auto nILine = rng.numInlines();
			auto nXLine = rng.numCrosslines(); 
			auto padder = fn.Unit + 1;
			auto newILRange = nILine + padder * 2;
			auto newXLRange = nXLine + padder * 2;
			auto newZRange = nSample + padder * 2;
			vector<SegyTraceHeader> TraceHeader;
			TraceHeader.reserve(numTrcsIn); // segy header

			MAX_NUM_SMP = rng.numSmps();
			logger(format("Started: %s", ctime(&startTime)), 2, verbosity); // ctime() adds new line
			logger(format("Number of traces in file: %i", numTrcsIn), 2, verbosity);

			// send process to nodes
			auto nproc = num_procs - 1;
			vector<int> ChunkSize(nproc);
			int chunk = numTrcsIn / nproc;
			int leftOver = numTrcsIn % nproc;
			int add = 0;
			logger(format("chunk : %i %i", chunk, leftOver), 2, verbosity);
			for (int i = 0; i < nproc; i++)
			{
				int add = i < leftOver ? 1 : 0;
				ChunkSize[i] = chunk + add;
				logger(format("Chunksize of proc %i : % i", i + 1, ChunkSize[i]), 2, verbosity);
				MPI_Send(ChunkSize.data() + i, 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD); // send chunk size
				MPI_Send(&MAX_NUM_SMP, 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD);  // send nz
				// MPI_Send(&dt, 1, MPI_FLOAT, i + 1, 0, MPI_COMM_WORLD);		   // send dt
			}

			logger("Sending data", 2, verbosity);
			int chunkId = 1;
			int width = padder * 2 + 1;
			int tn = 0;
			deque<vector<vector<float>>> cube(width);
			logger(format("inline %i crossline %i", newILRange, newXLRange), 2, verbosity);
			for (size_t i = 0, I = 0; i < newILRange; i++) {
				// logger(format("prepare inline %i", i), 2, verbosity);
				vector<vector<float>> txLine(newXLRange);
				for (size_t j = 0; j < newXLRange; j++) {
					// logger(format("prepare crossline j %i", j), 2, verbosity);
					vector<float> tx(nSample);
					if (i > padder - 1 && i < nILine + padder && j > padder - 1 && j < nXLine + padder) {
						SeisTrace trc; // seismic trace data
						trc.setNumSmps(nSample);
						trc.setDSample(dt);
						inputData.readTrace(trc);
						trc.getValues(tx.data(), nSample);     
						TraceHeader.push_back(trc.getHeaders());  
						// logger(format("get trace data i %i j %i at chunk %i", i, j, chunkId), 2, verbosity);
					}
					txLine[j] = tx;
					// logger(format("done crossline j %i", j), 2, verbosity);
				}    
				
				if (i < width) {
					cube[I] = txLine;
					I++;
				} else {
					cube.pop_front();
      					cube.push_back(txLine);
				}
					
				if (i > width - 2) {
					// logger(format("send mpi for inline %i at chunk %i", i, chunkId), 2, verbosity);
					// send to mpi according to chunkId
					for (size_t c = padder; c < nXLine + padder; c++, tn++) {
						for (size_t ci = 0; ci < width; ci++) {
							for (size_t cj = 0, cl = c - padder; cj < width; cj++, cl++) {
								//mpi send tx    
								MPI_Send(cube[ci][cl].data(), MAX_NUM_SMP, MPI_FLOAT, chunkId, 0, MPI_COMM_WORLD); // send traces
							}
						}        
						if (tn == ChunkSize[chunkId - 1] - 1) {
							//logger(format("construct from inline %i tn %i", i, tn), 2, verbosity);
							logger(format("Successfully send %i Traces to Proc %i", ChunkSize[chunkId - 1] * 81, chunkId), 2, verbosity);
							chunkId++;
							tn = -1;
						}
					}      
				}    
			}

			logger(format("stat %i tn %i", TraceHeader.size(), tn), 2, verbosity);
			logger("Gathering back data", 2, verbosity);
			vector<int> fracture_density(8);

			logger("Gathering fracture density", 2, verbosity);
			for (int i = 0; i < nproc; i++) {
				vector<int> fracture(8);
				MPI_Recv(fracture.data(), fracture.size(), MPI_INT, i+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				for (int j = 0; j < fracture.size(); j++)
					fracture_density[j] += fracture[j];
			}
			
			ofstream out(infofile);
			out << "direction" << "," << "density" << "\n";
			for (int i = 0; i < fracture_density.size(); i++) {
				out << i+1 << "," << fracture_density[i] << "\n";
			}			
			out.close();


			int procID = 0;
			int procIDSum = ChunkSize[0];
			chunkId = 1;
			for (int j = 0; j < numTrcsIn; j++)
			{
				for (int i = 0; i < outfilename.size(); i++)
				{
					SeisTrace trc;
					vector<float> tx(MAX_NUM_SMP);
					MPI_Recv(tx.data(), MAX_NUM_SMP, MPI_FLOAT, chunkId, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
					trc.setValues(tx.data(), MAX_NUM_SMP);
					trc.setHeaders(TraceHeader[j]);
					outputData[i]->writeTrace(trc);
				}
				procID++;

				if (procID >= procIDSum)
				{
					logger(format("Successfully gather %i Traces from Proc %i", ChunkSize[chunkId - 1], chunkId), 2, verbosity);
					chunkId++;
					procIDSum = procIDSum + ChunkSize[chunkId - 1];
				}
			}

		}
		catch (PspaException &ex)
		{
			ex.addSource("MPI master process");
			logger(ex.toString(), 0, verbosity);
			MPI_Barrier(MPI_COMM_WORLD);
			MPI_Finalize();
			return 0;
		}

		time_t endTime = time(NULL);
		logger(format("Ended: %s", ctime(&endTime)), 2, verbosity); // ctime() adds new line
		logger(format("Duration: %.f seconds", difftime(endTime, startTime)), 2, verbosity);
		for (int i = 0; i < outfilename.size(); i++)
			logger(format("Successfully save output to filename : %s", outfilename[i].data()), 2, verbosity);

		clock_gettime(CLOCK_REALTIME, &tend); // Works on Linux
		diff = tend.tv_sec - tstart.tv_sec;	  //BILLION * (tend.tv_sec - tstart.tv_sec) + tend.tv_nsec - tstart.tv_nsec;
		logger(format("MASTER PROCESS TOTAL TIME %i total = %llu seconds", rank, (long long unsigned int)diff), 2, verbosity);

	}
	else
	{
		timespec tstart, tend;
		uint64_t diff;
		clock_gettime(CLOCK_REALTIME, &tstart); // Works on Linux
		//read data		
		int ChunkSizeLocal = 0;
		int padder = fn.Unit + 1;
		int width = padder * 2 + 1;
		MPI_Recv(&ChunkSizeLocal, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&MAX_NUM_SMP, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);		

		int area = width * width;
		int size = ChunkSizeLocal * area;
		vector<vector<float>> traces;
		traces.reserve(size);
		for (int i = 0; i < size; i++)
		{
			vector<float> tx(MAX_NUM_SMP);
			MPI_Recv(tx.data(), MAX_NUM_SMP, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			traces.push_back(tx);
		}
		logger(format("Proc %i, successfully receive %i Traces", rank, traces.size()), 2, verbosity);

		if (rank == 1)
			logger(format("Parameters %i\n%s", rank, fn.toString().data()), 2, verbosity);

		fn.Initialize(); // init 
		vector<int> fracture_density(8);
		vector<vector<vector<float>>> outputs(10, vector<vector<float>>(ChunkSizeLocal, vector<float>(MAX_NUM_SMP)));
		auto widthPad = width + 2;
		for (size_t c = 0; c < ChunkSizeLocal; c++) {
			for (size_t t = 0; t < MAX_NUM_SMP; t++) {
				vector<vector<vector<float>>> cube(widthPad, vector<vector<float>>(widthPad, vector<float>(widthPad)));
				for (size_t i = 1, l = area * c; i < width + 1; i++) {
					for (size_t j = 1; j < width + 1; j++, l++) { //get trace cube
						auto m = t - padder;
						for (size_t k = 1; k < width + 1; k++, m++) {
							cube[i][j][k] = m < 0 || m > MAX_NUM_SMP - 1 ? 0 : traces[l][m];
						}
					}
				}
				vector<vector<vector<float>>> surfaces(9);
				surfaces[0] = getSurface1(cube);
				surfaces[1] = getSurface2(cube);
				surfaces[2] = getSurface3(cube);
				surfaces[3] = getSurface4(cube);
				surfaces[4] = getSurface5(cube);
				surfaces[5] = getSurface6(cube);
				surfaces[6] = getSurface7(cube);
				surfaces[7] = getSurface8(cube);
				surfaces[8] = getSurface9(cube);

				auto convergence = 0;
				for (int plane = 0; plane < surfaces.size(); plane++) {
					auto surface = surfaces[plane];
					auto output = fn.Detect(plane, fracture_density, surface);
					convergence += output;
					outputs[plane][c][t] = output;
				}
				outputs[9][c][t] = convergence;
			}
			//logger(format("Proc %i, successfully get cube for c %i", rank, c), 2, verbosity);
		}
		
		MPI_Ssend(fracture_density.data(), fracture_density.size(), MPI_INT, 0, 0, MPI_COMM_WORLD);
		auto n = outputs.size();
		for (int i = 0; i < ChunkSizeLocal; i++) {
			for (int j = 0; j < n; j++) {
				MPI_Ssend(outputs[j][i].data(), MAX_NUM_SMP, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
			}
		}

		clock_gettime(CLOCK_REALTIME, &tend); // Works on Linux
		diff = tend.tv_sec - tstart.tv_sec;	  //BILLION * (tend.tv_sec - tstart.tv_sec) + tend.tv_nsec - tstart.tv_nsec;
		logger(format("Proc %i, total = %llu seconds", rank, (long long unsigned int)diff), 2, verbosity);
	}

	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();

	return 0;
}
