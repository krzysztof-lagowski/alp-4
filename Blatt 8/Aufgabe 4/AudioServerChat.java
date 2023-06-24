import javax.sound.sampled.*;
import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.List;

public class AudioServerChat {
    private static List<Socket> clientSockets = new ArrayList<>();

    public static void main(String[] args) {
        try {
            // Öffnen der Audiodatei
            File audioFile = new File("/Users/krzysztof-test/Documents/Code/Uni/ALP4/untitled/src/DIE-LIEBE-KOMMT-NICHT-AUS-BERLIN.wav");
            AudioInputStream audioInputStream = AudioSystem.getAudioInputStream(audioFile);

            // Abrufen des Audioformats
            AudioFormat audioFormat = audioInputStream.getFormat();

            // Erstellen des Line-Info-Objekts
            DataLine.Info info = new DataLine.Info(SourceDataLine.class, audioFormat);

            // Öffnen des Audioausgangs
            SourceDataLine line = (SourceDataLine) AudioSystem.getLine(info);
            line.open(audioFormat);

            // Starten des Audioausgangs
            line.start();

            // Starten des Serversockets
            ServerSocket serverSocket = new ServerSocket(1234);
            System.out.println("Server gestartet. Warte auf Verbindungen...");

            while (true) {
                // Warten auf eingehende Verbindung
                Socket clientSocket = serverSocket.accept();
                System.out.println("Neue Verbindung hergestellt: " + clientSocket.getInetAddress());

                // Client-Socket zur Liste der verbundenen Sockets hinzufügen
                clientSockets.add(clientSocket);

                // Starten eines separaten Threads, um Audiodaten an den Client zu senden und Nachrichten zu empfangen
                Thread clientThread = new Thread(() -> {
                    try {
                        // Öffnen des Audiostreams vom Server
                        InputStream inputStream = clientSocket.getInputStream();

                        // Lesen und Abspielen der Audiodaten
                        byte[] buffer = new byte[4096];
                        int bytesRead = 0;
                        while ((bytesRead = audioInputStream.read(buffer)) != -1) {
                            line.write(buffer, 0, bytesRead);
                        }

                        // Beenden des Audioausgangs
                        line.drain();
                        line.close();
                        audioInputStream.close();
                        clientSocket.close();

                        // Entfernen des Client-Sockets aus der Liste der verbundenen Sockets
                        clientSockets.remove(clientSocket);
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                });

                // Starten des Client-Threads
                clientThread.start();

                // Starten eines separaten Threads, um Nachrichten von Client zu empfangen und an alle Clients zu senden
                Thread messageThread = new Thread(() -> {
                    try {
                        BufferedReader reader = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
                        String message;
                        while ((message = reader.readLine()) != null) {
                            System.out.println("Nachricht empfangen: " + message);
                            // Weiterleiten der Nachricht an alle verbundenen Clients
                            for (Socket socket : clientSockets) {
                                if (socket != clientSocket) {
                                    PrintWriter writer = new PrintWriter(socket.getOutputStream(), true);
                                    writer.println(message);
                                }
                            }
                        }
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                });

                // Starten des Message-Threads
                messageThread.start();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
