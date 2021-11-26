package test;

public class TestNativeSum {

	public native double sum(double a, double b);
	
	static {
		System.loadLibrary("test_native");
	}
	
	public static void main(String[] args) {
		TestNativeSum tns = new TestNativeSum();
		double result = tns.sum(1.1, 3.2);
		
		System.out.println("");
		System.out.println(result);

	}

}
