import java.net.*;

/**
 * This example illustrates the basic syntax for multicast sending.
 * Compatible with Java 14+ (using NetworkInterface).
 * @author M. L. Liu
 */
public class Example1Sender {

    public static void main(String[] args) {
        if (args.length != 1) {
            System.out.println("Usage: java Example1Sender <message>");
            return;
        }

        try {
            // Adresse du groupe multicast et port
            InetAddress group = InetAddress.getByName("239.1.2.3");
            int port = 3456;

            // Création de la socket multicast
            MulticastSocket socket = new MulticastSocket();

            // Message à envoyer
            String msg = args[0];
            byte[] buffer = msg.getBytes();

            // Préparation du paquet
            DatagramPacket packet = new DatagramPacket(buffer, buffer.length, group, port);

            // Optionnel : limiter la portée du multicast (TTL)
            socket.setTimeToLive(32);

            // Envoi du message
            for (int i = 0; i < 5; i++){
               socket.send(packet);
               Thread.sleep(1000);
            }

            System.out.println("Message envoyé au groupe " + group.getHostAddress() + ":" + port);

            socket.close();

        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
