package com.petronas.dsg;

import java.util.function.Supplier;

public class SeismicRange {	
	private float[] _items;
	public int id = 0;
	public int traceSize = 1;
	public Supplier<float[]> fn = () -> { return _items; };
	
	public SeismicRange() {}
	
	public SeismicRange(float[] items, int size) {
		traceSize = size;
		_items = items;
	}
	
	public SeismicRange(float[] items, int size, int id) {
		this(items, size);
		this.id = id;
	}
	
	public float[] getData()
	{
		return fn.get();
	}
	
	public void setItem(float[] items)
	{
		_items = items;
	}
	
	public float[] getItem()
	{
		return _items;
	}
}
