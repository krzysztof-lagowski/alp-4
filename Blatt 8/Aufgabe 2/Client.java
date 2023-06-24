import java.io.*;
import java.net.*;
public class Client {
    public static void main(String args[]) throws IOException {
        Socket socket = new Socket("localhost", 1234);
        PrintStream out = new PrintStream(socket.getOutputStream());
        BufferedReader in = new BufferedReader(new
                InputStreamReader(socket.getInputStream()));
        BufferedReader keyboard = new BufferedReader(new
                InputStreamReader(System.in));
        while (true) {
            String message = keyboard.readLine();
            if (message == null)
                break;
            out.println(message);
            String answer = in.readLine();
            System.out.println("echo: " + answer);
        }
        in.close();
        out.close();
        socket.close();
    }
}