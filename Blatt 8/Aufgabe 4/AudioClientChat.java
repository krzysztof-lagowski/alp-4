import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;

public class AudioClientChat {
    public static void main(String[] args) {
        try {
            // Verbindung zum Server herstellen auf entspannt s
            Socket socket = new Socket("localhost", 1234);
            System.out.println("Verbunden mit Server");

            // Thread zum Empfangen von Nachrichten starten
            Thread receiveThread = new Thread(() -> {
                try {
                    BufferedReader reader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
                    String message;
                    while ((message = reader.readLine()) != null) {
                        System.out.println("Nachricht erhalten: " + message);
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }
            });
            receiveThread.start();

            // Thread zum Senden von Nachrichten starten
            Thread sendThread = new Thread(() -> {
                try {
                    PrintWriter writer = new PrintWriter(socket.getOutputStream(), true);
                    BufferedReader consoleReader = new BufferedReader(new InputStreamReader(System.in));
                    String message;
                    while ((message = consoleReader.readLine()) != null) {
                        writer.println(message);
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }
            });
            sendThread.start();

        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
