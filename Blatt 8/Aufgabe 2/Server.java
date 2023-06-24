import java.io.*;
import java.net.*;

public class Server {
    public static void main(String args[]) throws IOException {
        ServerSocket listen = new ServerSocket(1234);
        while (true) {
            Socket socket = listen.accept();

            BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            PrintStream out = new PrintStream(socket.getOutputStream());
            out.print("Hello world. ");
            while (true) {
                String message = in.readLine();
                if (message == null) {
                    break;
                }
                String answer = message.replace('i', 'o');
                out.println(answer);
                System.out.printf("Message send! ");
            }
            in.close();
            out.close();
            socket.close();
            System.out.println("Socket closed.");
        }
    }
}
