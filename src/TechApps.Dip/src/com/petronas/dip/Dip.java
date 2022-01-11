package com.petronas.dip;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.CompletionService;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorCompletionService;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.SubMonitor;

import com.petronas.dsg.ISeismicData;
import com.petronas.dsg.SeismicRange;

public class Dip {
	public int windowsX = 0;
	public int windowsZ = 0;
	public int dX = 0;
	public int dZ = 0;
	public float psizeCut = 0;
	public int type = 0;

	private float maxAmp = 1;
	private ISeismicData _seismic;
	private int thread_pool;

	static {
		System.loadLibrary("DipJni");
	}

	public static native float[] run(float[] input, int traceSize, int winX, int winZ, int dx, int dz, float psizeCut,
			float maxAmp);

	public float[] run(float[] input, int traceSize) {
		return run(input, traceSize, windowsX, windowsZ, dX, dZ, psizeCut, maxAmp);
	}

	public Dip(ISeismicData seismic) {
		_seismic = seismic;
		int cores = Runtime.getRuntime().availableProcessors(); // 48, 24, 12
		thread_pool = (cores / 2) - 1; // 6, 3, 1
	}

	private static final String formatName = "DIP_%s_%s_%s_%s_%s"; // <name>_<id>_ENTD_<unit>_All

	private void calculate(CompletionService<SeismicRange[]> service, SubMonitor sub, int index) {
		SeismicRange[] ranges = index == 0 ? _seismic.sliceByInline() : _seismic.sliceByCrossline();
		if (maxAmp == 1)
			GetMaxAmp(ranges);
		int numberOfCrossLine = _seismic.getNumberOfCrossLine();
		int length = index == 0 ? numberOfCrossLine : _seismic.getNumberOfInLine();
		List<Future<SeismicRange[]>> futures = new ArrayList<Future<SeismicRange[]>>();
		for (SeismicRange range : ranges) {
			futures.add(service.submit(() -> {
				SeismicRange[] o = new SeismicRange[length];
				float[] input = range.getData();
				float[] result = run(input, length);
				int sampleSize = result.length / length;
				for (int i = 0; i < length; i++) {
					float[] trace = new float[sampleSize];
					System.arraycopy(result, i * sampleSize, trace, 0, sampleSize);
					int id = index == 0 ? i : i * numberOfCrossLine; // align id with in line direction
					o[i] = new SeismicRange(trace, 1, range.id + id);
				}
				return o;
			}));
		}

		try {
			// wait for all task to finish
			for (int j = 0; j < futures.size(); j++) {
				if (sub.isCanceled())
					break;
				SeismicRange[] task = service.take().get();
//				int n = task.id / length;
//				System.out.println("save plane: " + n);
//				sub.setTaskName("save plane: " + n);
				for (int i = 0; i < length; i++) {
					_seismic.save(task[i], index); // save the result
				}
				sub.split(1);
			}
		} catch (InterruptedException | ExecutionException e) {
//			_log.error("future error" + e.getMessage());
			e.printStackTrace();
		} catch (Exception e) {
//			_log.error("future error" + e.getMessage());
			// Thread.currentThread().interrupt();
			e.printStackTrace();
		}
	}

	public void run(IProgressMonitor monitor) {
		// generate output name
		int length = 0;
		int size = type == 0 ? 2 : 1;
		int o = 0;
		String[] outfilename = new String[size];

		if (type == 0 || type == 1) {
			outfilename[o++] = String.format(formatName, "X", windowsX, windowsZ, dX, dZ);
			length += _seismic.getNumberOfCrossLine();
		}
		if (type == 0 || type == 2) {
			outfilename[o++] = String.format(formatName, "Y", windowsX, windowsZ, dX, dZ);
			length += _seismic.getNumberOfInLine();
		}
		_seismic.setOutputName(outfilename);

		SubMonitor sub = SubMonitor.convert(monitor, length);
		ExecutorService executor = Executors.newFixedThreadPool(thread_pool);
		CompletionService<SeismicRange[]> service = new ExecutorCompletionService<SeismicRange[]>(executor);

		if (type == 0 || type == 1)
			calculate(service, sub, 0);
		if (type == 0 || type == 2)
			calculate(service, sub, 1);

		executor.shutdown();
	}

	public void GetMaxAmp(SeismicRange[] ranges) {

		ExecutorService executor = Executors.newFixedThreadPool(thread_pool);
		List<Future<Float>> tasks = new ArrayList<Future<Float>>();
		CompletionService<Float> service = new ExecutorCompletionService<Float>(executor);
		for (SeismicRange range : ranges) {
			tasks.add(service.submit(() -> {
				float max = Float.MIN_VALUE;
				try {
					float[] floatArray = range.getData();
					for (float v : floatArray) {
						if (max < v)
							max = v;
					}
				} catch (Exception e) {
					// _log.error("magnitude " + e.getMessage());
				}
				return max;
			}));
		}

		try {
			for (int j = 0; j < tasks.size(); j++) {
				Future<Float> task = service.take();
				float result = task.get();
				if (maxAmp < result)
					maxAmp = result;
			}
		} catch (InterruptedException e) {
			// _log.error("spectrum thread interrupted: " + e.getMessage());
			Thread.currentThread().interrupt();
		} catch (ExecutionException e) {
			// _log.error("spectrum thread exception: " + e.getMessage());
			Thread.currentThread().interrupt();
		} finally {
			if (executor != null)
				executor.shutdownNow();
		}
	}

}
