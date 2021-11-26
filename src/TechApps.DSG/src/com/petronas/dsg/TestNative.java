package com.petronas.dsg;

public class TestNative {

	static {
		System.loadLibrary("test_native");
	}
	
	public static native double multiple(double a, double b);
	public static native double sum(double a, double b);
	
	
	public TestNative()
	{
		
	}
	
	public void RunSum()
	{
		System.out.println("Start Run Sum");
		double result =  sum(2.54, 3.5);
		System.out.println("Stop Run Sum");
		System.out.println(result);
	}
	
	
	
//	public static void main(String[] args) {
//		System.out.println("App StartTTT :");
//			
//		double resultSum = sum(1.1, 3.2);
//		double resultMul = multiple(1.1, 3.2);
//		
//		System.out.println("The Result:");
//		System.out.println(String.format("Sum :%.5f", resultSum));
//		System.out.println(String.format("Mul :%.5f", resultMul));
//	}
}
