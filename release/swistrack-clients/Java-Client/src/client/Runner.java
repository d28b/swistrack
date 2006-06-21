package client;

public class Runner {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		ClientThread ct = new ClientThread("lanoslap17.epfl.ch",3000);
		System.out.println("Running client...");
		ct.start();
	}

}
