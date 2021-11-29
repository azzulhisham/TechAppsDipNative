package com.petronas.dip;


public class CallNative {

	static {
		System.loadLibrary("test_native");
	}	
	
	public static native double multiple(double a, double b);
	public static native double sum(double a, double b);
	public static native String comboItem();
	public static native String comboCustomItem(String item);
	
	
	public static void main(String[] args) {
		System.out.println("App Start :");
			
		double resultSum = sum(1.1, 3.2);
		double resultMul = multiple(1.1, 3.2);
	
		System.out.println("The Result:");
		System.out.println(String.format("Sum :%.5f", resultSum));
		System.out.println(String.format("Mul :%.5f", resultMul));
		System.out.println(comboItem());
		System.out.println(comboCustomItem("This string is passed from Java..."));
	}
}