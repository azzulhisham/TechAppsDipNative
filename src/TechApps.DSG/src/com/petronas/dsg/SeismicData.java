package com.petronas.dsg;

import java.nio.file.Paths;
import java.util.Iterator;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.ui.PlatformUI;

public class SeismicData implements ISeismicData {
	private String _path;
	private float[][] segy;
	
	private int _numberOfTrace;
	public int getNumberOfTrace() {
		return _numberOfTrace;
	}
	
	private int _numberOfSample;
	public int getNumberOfSample() {
		return _numberOfSample;
	}
	
	private int _numberOfInline;
	public int getNumberOfInLine() {
		return _numberOfInline;
	}
	
	private int _numberOfCrossLine;
	public int getNumberOfCrossLine() {
		return _numberOfCrossLine;
	}
	
	private float _sampleRate;
	public float getSampleRate() {
		return _sampleRate;
	}	
	
    public native boolean write(String inputName, String outputName, float[] traces);
    public native static float[][] read(String fileName, int[] info);
    public float[][] read(String fileName) {
        int[] info = new int[5];
        float[][] traces = read(fileName, info);
        _numberOfTrace = info[0];
        _numberOfInline = info[1];
        _numberOfCrossLine = info[2];
        _numberOfSample = info[3];
        _sampleRate = info[4] / 1000f;
        return traces;
    }  
	
	static {
        System.loadLibrary("SegyJni");
        
    }
	
	public SeismicData(String path) {
		_path = path;
	}
	
	public SeismicData() {
		
	}	
	
	public void init() {
		IStructuredSelection ss = (IStructuredSelection) PlatformUI.getWorkbench()
				.getActiveWorkbenchWindow().getSelectionService().getSelection();
		if (ss == null)
			return;
		
		for (Iterator<?> iter = ss.iterator(); iter.hasNext();) {
			Object obj = iter.next();
			if (obj instanceof String)
				_path = (String) obj;
		}
		
		if (_path != null) {
			segy = read(_path);
			if (segy == null || segy.length == 0)
				_path = null;
		}
	}
	
	public String getName() {
		return _path == null ? "" : Paths.get(_path).getFileName().toString();
	}
	float[][] output;
	String[] names;
	// set output name
	public void setOutputName(String... name) {
		output = new float[name.length][];
		names = new String[name.length];
		for (int i = 0; i < output.length; i++) {
			names[i] = _path.replace(".s", String.format(name[i], "") + ".s") ;
			output[i] = new float[_numberOfTrace * _numberOfSample];
		}
	}
	
	// save all output
	public void save() {
		for (int i = 0; i < output.length; i++) {
			write(_path, names[i], output[i]);
			System.out.println("save seismic " + names[i]);
		}
	}
	
	public void save(SeismicRange range, int index) {
		float[] items = range.getItem();
		System.arraycopy(items, 0, output[index], range.id * _numberOfSample, items.length);
	}
	
	public SeismicRange[] sliceByInline() {
		SeismicRange[] ranges = new SeismicRange[_numberOfInline];
		for (int i = 0, l = 0; i < ranges.length;i++) {
			float[] traces = new float[_numberOfCrossLine * _numberOfSample];
			for (int j = 0; j < _numberOfCrossLine; j++, l++) {
				System.arraycopy(segy[l], 0, traces, j * _numberOfSample, _numberOfSample);				
			}
			ranges[i] = new SeismicRange(traces, _numberOfCrossLine);
			ranges[i].id = i;
		}
		return ranges;
	}
	
	public SeismicRange[] subCubeByTrace(int size) {
		SeismicRange[] ranges = new SeismicRange[_numberOfTrace];
		int padding = size / 2;
		int cubeSize = size * size;
		for (int i = 0, k = 0; i < _numberOfInline; i++) {
			for (int j = 0; j < _numberOfCrossLine; j++, k++) {
				float[] cube = new float[_numberOfSample * cubeSize];
				for (int w = 0, m = i-padding, l = 0; w < size; w++, m++) {
					for (int h = 0, n = j - padding; h < size; h++, n++, l++) {
						if (m < 0 || m > _numberOfInline - 1 || n < 0 || n > _numberOfCrossLine - 1)
							continue;
						int index = (m * _numberOfCrossLine) + n;
						System.arraycopy(segy[index], 0, cube, l * _numberOfSample, _numberOfSample);
					}
				}
				ranges[k] = new SeismicRange(cube, cubeSize);
				ranges[k].id = k;
			}
		}
		return ranges;
	}
	
	public float[][] getByInline(int index) {		
		float[][] segy = read(_path); 
		float[][] ranges = new float[_numberOfCrossLine][_numberOfSample];
		int start = index * _numberOfCrossLine;
		for (int i = 0; i < ranges.length;i++) {
			ranges[i] = segy[start + i];
		}
		return ranges;
	}
}