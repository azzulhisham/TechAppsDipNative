package test;

import com.petronas.dip.CallNative;


public class TestJNI {

	public static void main(String[] args) {
		System.out.println("App Start.........................");
		
		try {
			double test = CallNative.sum(1.1, 2.2);
			System.out.println(test);			
		}
		catch (Exception e) {
			System.out.println("Error...........................");		
		}
		finally {
			System.out.println("End...........................");		
		}

	}

}
