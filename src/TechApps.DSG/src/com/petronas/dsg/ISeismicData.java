package com.petronas.dsg;

public interface ISeismicData {
	int getNumberOfTrace();
	int getNumberOfSample();
	int getNumberOfInLine();
	int getNumberOfCrossLine();
	float getSampleRate();
	void init();
	String getName();	
	void setOutputName(String... name);
	SeismicRange[] sliceByInline();
	SeismicRange[] subCubeByTrace(int size);
	void save(SeismicRange range, int index);
	void save();
}
