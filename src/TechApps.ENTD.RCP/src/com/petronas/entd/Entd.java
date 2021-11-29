package com.petronas.entd;

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

public class Entd {

	public int unit = 3;
	public double rotation = 0;
	private int sampleSize = 0;
	private ISeismicData _seismic;
	private int thread_pool;

	public Entd(ISeismicData seismic) {
		_seismic = seismic;
		int cores = Runtime.getRuntime().availableProcessors(); // 48, 24, 12
		thread_pool = (cores / 2) - 1; // 6, 3, 1
	}

	public float[][] calculate(float[] input) {
		int padder = unit + 1;
		int width = padder * 2 + 1;
		int widthPad = width + 2;
		float[] density = new float[8];
		float[][] outputs = new float[11][sampleSize];
		for (int t = 0; t < sampleSize; t++) {
			float[][][] cube = new float[widthPad][widthPad][widthPad];
			for (int i = 1, l = 0; i < width + 1; i++) {
				for (int j = 1; j < width + 1; j++, l++) { // get trace cube
					int m = t - padder;
					for (int k = 1; k < width + 1; k++, m++) {
						cube[i][j][k] = m < 0 || m > sampleSize - 1 ? 0 : input[(l * sampleSize) + m];
					}
				}
			}

			float[][][] surfaces = getSurface(cube);
			// calculate plane
			int convergence = 0;
			for (int plane = 0; plane < surfaces.length; plane++) {
				float[][] surface = surfaces[plane];
				int output = detect(plane, density, surface);
				convergence += output;
				outputs[plane][t] = output;
			}
			outputs[surfaces.length][t] = convergence;
		}
		outputs[10] = density;
		return outputs;
	}

	SeismicRange[][] calculate(SeismicRange[] ranges, int inline) {		
		int numberOfCrossLine = _seismic.getNumberOfCrossLine();
		int index = inline * numberOfCrossLine;
		SeismicRange[][] o = new SeismicRange[11][numberOfCrossLine];
		for (int j = 0; j < numberOfCrossLine; j++) {
			SeismicRange range = ranges[index + j];
			float[] floatArray = range.getData();
			float[][] result = calculate(floatArray);
			for (int i = 0; i < result.length; i++) {
				o[i][j] = new SeismicRange(result[i], 1);
				o[i][j].id = range.id;
			}
		}
		return o;
	}

	void save(SeismicRange[][] result, float[] density, SubMonitor sub) {
		int numberOfCrossLine = _seismic.getNumberOfCrossLine();
		int inLine = result[0][0].id / numberOfCrossLine;
		System.out.println("save inline: " + inLine);
		sub.setTaskName("save inline: " + inLine);

		for (int k = 0; k < result.length - 1; k++) { // number of output
			float[] output = new float[numberOfCrossLine * sampleSize];
			for (int i = 0; i < result[k].length; i++) {
				System.arraycopy(result[k][i].getItem(), 0, output, i * sampleSize, sampleSize);
			}
			SeismicRange range = new SeismicRange(output, numberOfCrossLine);
			range.id = inLine;
			_seismic.save(range, k); // save the result
		}

		SeismicRange[] fractures = result[result.length - 1];
		for (int k = 0; k < fractures.length; k++) {
			float[] densities = fractures[k].getItem();
			for (int i = 0; i < density.length; i++) {
				density[i] += densities[i];
			}
		}
	}

	private static final boolean debug = false;
	private static final String formatName = "ENTD_%s_%s%s"; //<name>_<id>_ENTD_<unit>_All

	public float[] run(IProgressMonitor monitor) {
		// generate output name
		String[] outfilename = new String[10];
		for (int i = 0; i < outfilename.length - 1; i++)
			outfilename[i] = String.format(formatName, unit, "Plane_", (i + 1));
		outfilename[9] = String.format(formatName, unit, "All", "");
		_seismic.setOutputName(outfilename);
		
		templates = segment(unit); // initialize template
		sampleSize = _seismic.getNumberOfSample();
		int numberOfCrossLine = _seismic.getNumberOfCrossLine();
		int numberOfInLine = _seismic.getNumberOfInLine();
		final int padding = unit + 1;
		final int width = padding * 2 + 1;

		final SeismicRange[] ranges = _seismic.subCubeByTrace(width);
		SubMonitor sub = SubMonitor.convert(monitor, ranges.length / numberOfCrossLine);
		float[] density = new float[8];
		
if (debug) {
		for (int k = 0; k < numberOfInLine; k++) {
			try {
				SeismicRange[][] result = calculate(ranges, k);
				save(result, density, sub);
			} catch (Exception e) {
				e.printStackTrace();
			}			 
			sub.split(1);
		}
		
} else {
		ExecutorService executor = Executors.newFixedThreadPool(thread_pool);
		CompletionService<SeismicRange[][]> service = new ExecutorCompletionService<SeismicRange[][]>(executor);
		List<Future<SeismicRange[][]>> tasks = new ArrayList<Future<SeismicRange[][]>>();

		for (int k = 0; k < numberOfInLine; k++) {
			final int index = k;
			tasks.add(service.submit(() -> {				
				return calculate(ranges, index);
			}));
		}

		for (int j = 0; j < tasks.size(); j++) {
			if (monitor.isCanceled())
				break;
			try {
				SeismicRange[][] result = service.take().get();
				save(result, density, sub);
			} catch (InterruptedException | ExecutionException e) {
				// _log.error("future error" + e.getMessage());
				e.printStackTrace();
			} catch (Exception e) {
				// _log.error("future error" + e.getMessage());
				e.printStackTrace();
			}
			sub.split(1);
		}
		executor.shutdown();
}

		return density;
	}

	private Row[][] templates;

	private final double[][] _kernel = new double[][] { { 1.21306131942527, 1.60653065971263, 1.21306131942527 },
			{ 1.60653065971263, 2, 1.60653065971263 }, { 1.21306131942527, 1.60653065971263, 1.21306131942527 }, };

	// TODO uses array instead
	private class Row {
		public List<Integer> Xi;
		public List<Integer> Xj;

		public Row() {
			Xi = new ArrayList<Integer>();
			Xj = new ArrayList<Integer>();
		}
	}

	double[][] convolution2d(float[][] values) {
		int padding = _kernel.length / 2;
		double[][] convs = new double[values.length - padding * 2][values.length - padding * 2];
		double vn = values.length == 0 ? 0 : values[0].length;
		double kn = _kernel.length == 0 ? 0 : _kernel[0].length;

		for (int i = padding; i < values.length - padding; i++) {
			for (int j = padding; j < vn - padding; j++) {
				double conv = 0;
				for (int k = 0; k < _kernel.length; k++) {
					for (int l = 0; l < kn; l++) {
						conv += values[i + l - padding][j + k - padding] * _kernel[k][l];
					}
				}
				convs[i - padding][j - padding] = conv;
			}
		}
		return convs;
	}

	int detect(int plane, float[] density, float[][] values) {
		double[][] convs = convolution2d(values);
		int mid = unit + 1;
		double point = convs[mid][mid];
		int convergences = 0;
		for (int d = 0; d < templates.length; d++) {
			Row[] template = templates[d];
			int v = 0;
			for (int r = 0; r < template.length; r++) {
				double[] x = new double[5];
				for (int o = 0; o < x.length; o++) {
					int cx = template[r].Xi.get(o) - 1;
					int cy = template[r].Xj.get(o) - 1;
					x[o] = convs[cx][cy];
				}

				double c1 = x[0] + x[4] - 2 * x[2];
				double c2 = x[1] + x[3] - 2 * x[2];
				double c3 = x[0] + x[2] - 2 * x[1];
				double c4 = x[4] + x[2] - 2 * x[3];

				v += (c1 > point && c2 > c3 && c2 > c4) ? 1 : 0;
			}

			if (v == unit) {
				convergences++;
				if (plane == 0)
					density[d]++;
			}
		}
		return convergences;
	}

	Row[][] segment(int length) {
		int mid = (int) (Math.round(((length + 1) * 2 + 1) / 2.0));
		return new Row[][] { segment1(length, mid), segment2(length, mid), segment3(length, mid), segment4(length, mid),
				segment5(length, mid), segment6(length, mid), segment7(length, mid), segment8(length, mid), };
	}

	Row[] segment1(int length, int mid) {
		Row[] rows = new Row[length];
		for (int row = 0, a = mid; row < rows.length; row++, a--) {
			Row R = new Row();
			for (int count = 0, b = 2; count < 5; count++, b--) {
				R.Xi.add(a);
				R.Xj.add(mid - b);
			}
			rows[row] = R;
		}
		return rows;
	}

	Row[] segment2(int length, int mid) {
		Row[] rows = new Row[length];
		for (int row = 0, x = mid, y = mid; row < length; row++, x--, y += row % 2) {
			Row R = new Row();
			R.Xi.add(x - 1);
			R.Xj.add(y - 2);
			R.Xi.add(x - 1);
			R.Xj.add(y - 1);
			R.Xi.add(x - 0);
			R.Xj.add(y - 0);
			R.Xi.add(x + 1);
			R.Xj.add(y + 1);
			R.Xi.add(x + 1);
			R.Xj.add(y + 2);
			rows[row] = R;
		}
		return rows;
	}

	Row[] segment3(int length, int mid) {
		Row[] rows = new Row[length];
		for (int row = 0, x = mid, y = mid; row < length; row++, x--, y++) {
			Row R = new Row();
			for (int count = 0, a = 2, b = 2; count < 5; count++, a--, b--) {
				R.Xi.add(x - a);
				R.Xj.add(y - b);
			}
			rows[row] = R;
		}
		return rows;
	}

	Row[] segment4(int length, int mid) {
		Row[] rows = new Row[length];

		for (int row = 0, x = mid, y = mid; row < length; row++, x -= (row + 1) % 2, y++) {
			Row R = new Row();
			R.Xi.add(x - 2);
			R.Xj.add(y - 1);
			R.Xi.add(x - 1);
			R.Xj.add(y - 1);
			R.Xi.add(x - 0);
			R.Xj.add(y - 0);
			R.Xi.add(x + 1);
			R.Xj.add(y + 1);
			R.Xi.add(x + 2);
			R.Xj.add(y + 1);
			rows[row] = R;
		}
		return rows;
	}

	Row[] segment5(int length, int mid) {
		Row[] rows = new Row[length];
		for (int row = 0, b = mid; row < length; row++, b++) {
			Row R = new Row();
			for (int count = 0, a = 2; count < 5; count++, a--) {
				R.Xi.add(mid - a);
				R.Xj.add(b);
			}
			rows[row] = R;
		}
		return rows;
	}

	Row[] segment6(int length, int mid) {
		Row[] rows = new Row[length];
		for (int row = 0, x = mid, y = mid; row < length; row++, x += row % 2, y++) {
			Row R = new Row();
			R.Xi.add(x - 2);
			R.Xj.add(y + 1);
			R.Xi.add(x - 1);
			R.Xj.add(y + 1);
			R.Xi.add(x + 0);
			R.Xj.add(y + 0);
			R.Xi.add(x + 1);
			R.Xj.add(y - 1);
			R.Xi.add(x + 2);
			R.Xj.add(y - 1);
			rows[row] = R;
		}
		return rows;
	}

	Row[] segment7(int length, int mid) {
		Row[] rows = new Row[length];

		for (int row = 0, x = mid, y = mid; row < length; row++, x++, y++) {
			Row R = new Row();
			for (int count = 0, a = 2, b = 2; count < 5; count++, b--, a--) {
				R.Xi.add(x - a);
				R.Xj.add(y + b);
			}
			rows[row] = R;
		}
		return rows;
	}

	Row[] segment8(int length, int mid) {
		Row[] rows = new Row[length];
		for (int row = 0, x = mid, y = mid; row < length; row++, x++, y += (row + 1) % 2) {
			Row R = new Row();
			R.Xi.add(x - 1);
			R.Xj.add(y + 2);
			R.Xi.add(x - 1);
			R.Xj.add(y + 1);
			R.Xi.add(x + 0);
			R.Xj.add(y + 0);
			R.Xi.add(x + 1);
			R.Xj.add(y - 1);
			R.Xi.add(x + 1);
			R.Xj.add(y - 2);
			rows[row] = R;
		}
		return rows;
	}

	float[][][] getSurface(float[][][] cube) {
		return new float[][][] { getSurface1(cube), getSurface2(cube), getSurface3(cube), getSurface4(cube),
				getSurface5(cube), getSurface6(cube), getSurface7(cube), getSurface8(cube), getSurface9(cube), };
	}

	float[][] getSurface1(float[][][] cube) {
		int mid = ((cube.length - 1) / 2);
		float[][] surface = new float[cube.length][cube.length];
		for (int i = 0; i < cube.length; i++) {
			for (int j = 0; j < cube[i].length; j++) {
				surface[i][j] = cube[i][j][mid];
			}
		}
		return surface;
	}

	float[][] getSurface2(float[][][] cube) {
		int mid = ((cube.length - 1) / 2);
		float[][] surface = new float[cube.length][cube.length];
		for (int i = 0; i < cube.length; i++) {
			for (int j = 0; j < cube[i].length; j++) {
				surface[i][j] = cube[i][mid][j];
			}
		}
		return surface;
	}

	float[][] getSurface3(float[][][] cube) {
		int mid = ((cube.length - 1) / 2);
		float[][] surface = new float[cube.length][cube.length];
		for (int i = 0; i < cube.length; i++) {
			for (int j = 0; j < cube[i].length; j++) {
				surface[i][j] = cube[mid][i][j];
			}
		}
		return surface;
	}

	float[][] getSurface4(float[][][] cube) {
		float[][] surface = new float[cube.length][cube.length];
		for (int i = 0; i < cube.length; i++) {
			for (int j = 0; j < cube[i].length; j++) {
				surface[i][j] = cube[i][i][j];
			}
		}
		return surface;
	}

	float[][] getSurface5(float[][][] cube) {
		float[][] surface = new float[cube.length][cube.length];
		for (int i = 0, q = cube.length - 1; i < cube.length; i++, q--) {
			for (int j = 0; j < cube[i].length; j++) {
				surface[i][j] = cube[i][q][j];
			}
		}
		return surface;
	}

	float[][] getSurface6(float[][][] cube) {
		float[][] surface = new float[cube.length][cube.length];
		for (int i = 0; i < cube.length; i++) {
			for (int j = 0; j < cube[i].length; j++) {
				surface[i][j] = cube[i][j][i];
			}
		}
		return surface;
	}

	float[][] getSurface7(float[][][] cube) {
		float[][] surface = new float[cube.length][cube.length];
		for (int i = 0, r = cube.length - 1; i < cube.length; i++, r--) {
			for (int j = 0; j < cube[i].length; j++) {
				surface[i][j] = cube[i][j][r];
			}
		}
		return surface;
	}

	float[][] getSurface8(float[][][] cube) {
		float[][] surface = new float[cube.length][cube.length];
		for (int i = 0; i < cube.length; i++) {
			for (int j = 0; j < cube[i].length; j++) {
				surface[i][j] = cube[j][i][i];
			}
		}
		return surface;
	}

	float[][] getSurface9(float[][][] cube) {
		float[][] surface = new float[cube.length][cube.length];
		for (int i = 0, q = cube.length - 1; i < cube.length; i++, q--) {
			for (int j = 0; j < cube[i].length; j++) {
				surface[i][j] = cube[j][q][i];
			}
		}
		return surface;
	}
}