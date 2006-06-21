/**
 * Little client for Tracking Tool Server
 * Grégory Mermoud - gregory.mermoud@epfl.ch
 * SWIS - I&C - EPFL
 */

package client;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.net.SocketAddress;


public class ClientThread extends Thread {
	public String address;
	public int port;
	private static final int BUFSIZE = 220;   // Size of receive buffer
	
	// communication
	private InputStream in;
	private OutputStream out;
	
	
	public ClientThread(String address, int port) {
		this.address = address;
		this.port = port;
	}
	
	public void run() {
		try {
			// Create a socket to connect to the client
			Socket sock = new Socket(address,port);
			System.out.println("Connected to " + address + ":" + port);
			
			byte[] byteBuffer = new byte[BUFSIZE];  // Receive buffer
			
			in = sock.getInputStream();
			out = sock.getOutputStream();
			
			// getting character 206
			byte ch = new Integer(206).byteValue();
			byte[] buf = new byte[1];
			buf[0] = ch;
			out.write(buf);
			
			
			in.read(byteBuffer,0,byteBuffer.length);
			// reading input buffer
			String trimmedInput = new String(byteBuffer).trim();
			System.out.print(trimmedInput);
			
			System.out.println();
			
			System.out.println("Done! Disconnecting...");
			
			// closing all
			in.close();
			out.close();
			sock.close();  // Close the socket.  We are done with this client!
		} catch (IOException e) {
			System.out.println(e.getMessage());
			e.printStackTrace();
		}
	}
}
