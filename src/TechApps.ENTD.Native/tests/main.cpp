#include <vector>
#include <tuple>
#include "Entd.h"
#include "cubing.h"
#include <SegyData.h>
#include <SegyRead.h>
#include <SegyWrite.h>
#include <iostream>
#include <PspaException.h>
#include <omp.h>
#include <fstream>
#include <deque>

using namespace std;
using namespace PagosaGeo;

int main(int argc, char *argv[])
{
  //read segy
  //auto filename = "../../../data/F3_demo.sgy";
  auto filename = "../../../data/input_Anding_AUB1.segy";

  SegyRead inputData(filename);
  ENTD fn;

  //cube
  //get surface
  auto numTrcsIn = inputData.numTrcsInFile();
  auto rng = inputData.range();
  auto nSample = (int)rng.numSmps();
  auto nILine = rng.numInlines();
  auto nXLine = rng.numCrosslines();
  auto dt = (float)rng.dSample();

  cout << "Inline = " << nILine << "\nCrossline = " << nXLine << "\nSamples = " << nSample << endl;

  //user setting
  auto unitLength = 3;
  auto padder = unitLength + 1;
  auto mid = unitLength + 2;

  //new seis setting
  auto newILRange = nILine + padder * 2;
  auto newXLRange = nXLine + padder * 2;
  auto newZRange = nSample + padder * 2;

  // data format matlab
  // F3_Demo = z x i (176x251x226)
  // matlab permute x i z 251x226x176
  // data2 + padding 8 = 259x234x184

  // send subcube to nodes
  auto nproc = 24;
  auto chunk = numTrcsIn / nproc;
  auto leftOver = numTrcsIn % nproc;
  auto width = padder * 2 + 1;
  auto nSamplePad = nSample + padder * 2;
  vector<vector<vector<float>>> mpi(nproc);
  deque<vector<vector<float>>> cube(width);
  vector<SegyTraceHeader> headers;
  headers.reserve(inputData.numTrcsInFile());

  vector<int> ChunkSize(nproc);
  for (int i = 0; i < nproc; i++) {
    auto add = i < leftOver ? 1 : 0;
    ChunkSize[i] = chunk + add;
  }
  
  auto chunkId = 0;
  auto tn = 0;
  auto tm = 0;
  auto area = width * width;
  // set
  for (size_t i = 0, I = 0; i < newILRange; i++) {
    vector<vector<float>> txLine(newXLRange);
    for (size_t j = 0; j < newXLRange; j++) {
      vector<float> tx(nSample);
      if (i > padder - 1 && i < nILine + padder && j > padder - 1 && j < nXLine + padder) {
        SeisTrace trc; // seismic trace data
        trc.setNumSmps(nSample);
        trc.setDSample(dt);
        inputData.readTrace(trc);
        trc.getValues(tx.data(), nSample);     
        headers.push_back(trc.getHeaders());        
      }
      txLine[j] = tx;
    }    
    
    if (i < width) {
      cube[I] = txLine;
      I++;
    } else {
      cube.pop_front();
      cube.push_back(txLine);
    }
        
    if (i > width - 2) {
      // send to mpi according to chunkId
      for (size_t c = padder; c < nXLine + padder; c++, tn++) {        
        for (size_t ci = 0; ci < width; ci++) {            
          for (size_t cj = 0, cl = c - padder; cj < width; cj++, cl++) {
            mpi[chunkId].push_back(cube[ci][cl]); // mpi send tx
          }
        }        
        if (tn == ChunkSize[chunkId] - 1) {
          chunkId++;
          tn = -1;
          // i = newILRange; //debug
          // break;
        }
      }      
    }    
  }


  vector<int> fracture_density(8);
  vector<vector<vector<vector<float>>>> outputAll(nproc);
  auto widthPad = width + 2;


  #pragma omp parallel num_threads(24)
  {
    auto p = omp_get_thread_num();
    cout << "start parallel " << p << endl;
  // for (size_t p = 0; p < nproc; p++) { // each nodes
    vector<vector<vector<float>>> outputs(10, vector<vector<float>>(ChunkSize[p], vector<float>(nSample)));
    for (size_t c = 0; c < ChunkSize[p]; c++) {
      for (size_t t = 0; t < nSample; t++) {
        vector<vector<vector<float>>> cube(widthPad, vector<vector<float>>(widthPad, vector<float>(widthPad)));
        for (size_t i = 1, l = area * c; i < width + 1; i++) {  // column or in line
          for (size_t j = 1; j < width + 1; j++, l++) {         // row or cross line 
            int m = t-padder;
            for (size_t k = 1; k < width + 1; k++, m++) {       // depth or sample
              cube[i][j][k] = m < 0 || m > nSample - 1 ? 0 : mpi[p][l][m];
            }
          }
        }
        //this surface is 9 surface need to extract
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
        //plane computation
        for (int plane = 0; plane < surfaces.size(); plane++) {
          // auto surface = expand(Surface[plane]);
          auto surface = surfaces[plane];
          auto output = fn.Detect(plane, fracture_density, surface);
          convergence += output;
          outputs[plane][c][t] = output;
        }
        outputs[9][c][t] = convergence;
      }
    }
    outputAll[p] = outputs;
    cout << "end parallel " << p << endl;
  }

  auto infofile = "../../../data/entd_density.csv";
  ofstream out(infofile);
	out << "direction" << "," << "density" << "\n";
	for (int i = 0; i < fracture_density.size(); i++) {
		out << i+1 << "," << fracture_density[i] << "\n";
	}			
	out.close();

  for (size_t p = 0; p < 10; p++)
  {
    string name = "../../../data/entd_" + to_string(p) + ".segy";
    SegyWrite writer(name, inputData);
    writer.writeFileHeader();
    for (size_t i = 0, k = 0; i < nproc; i++) {
      for (size_t j = 0; j < outputAll[i][p].size(); j++, k++) {
        auto tx = outputAll[i][p][j];
        SeisTrace trc;
        trc.setValues(tx.data(), nSample);
        trc.setHeaders(headers[k]);
        writer.writeTrace(trc);
      }
    }
  }

  return 0;
}
