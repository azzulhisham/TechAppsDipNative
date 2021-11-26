package test;

public class TestNative {
	
	public native double multiple(double a, double b);
	
	static {
		System.loadLibrary("test_native");
	}

	public static void main(String[] args) {
		
		TestNative tn = new TestNative();
		double result = tn.multiple(1.1, 3.2);
		
		System.out.println("");
		System.out.println(result);
	}
}
