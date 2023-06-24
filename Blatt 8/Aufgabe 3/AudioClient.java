import javax.sound.sampled.*;
import java.io.InputStream;
import java.net.Socket;

public class AudioClient {
    public static void main(String[] args) {
        try {
            // Verbindung zum Server herstellen
            Socket serverSocket = new Socket("localhost", 1234);

            // Öffnen des Audiostreams vom Server
            InputStream inputStream = serverSocket.getInputStream();
            AudioInputStream audioInputStream = AudioSystem.getAudioInputStream(inputStream);

            // Abrufen des Audioformats
            AudioFormat audioFormat = audioInputStream.getFormat();

            // Erstellen des Line-Info-Objekts
            DataLine.Info info = new DataLine.Info(SourceDataLine.class, audioFormat);

            // Öffnen des Audioausgangs
            SourceDataLine line = (SourceDataLine) AudioSystem.getLine(info);
            line.open(audioFormat);

            // Starten des Audioausgangs
            line.start();

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
            serverSocket.close();

        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
