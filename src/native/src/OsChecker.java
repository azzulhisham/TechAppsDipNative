
public class OsChecker {

	private static String OS = System.getProperty("os.name").toLowerCase();
	
	public static boolean isWin() {
		return OS.contains("win");
	}
	
	public static void main(String[] args) {
		if(isWin()) {
			System.out.println("Yes! You are powered by Windows");
		}
	}
}
