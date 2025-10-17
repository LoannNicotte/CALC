import java.io.*;
import java.net.*;

public class Example1Receiver {

    static MulticastSocket s;
    static InetAddress group;

    public static void main(String[] args) {
        try {
            group = InetAddress.getByName("239.1.2.3");
            s = new MulticastSocket(3456);
            System.out.println("Joined group at 239.1.2.3 port 3456");

            // Nouvelle méthode (Java 14+)
            NetworkInterface ni = NetworkInterface.getByInetAddress(InetAddress.getLocalHost());
            s.joinGroup(new InetSocketAddress(group, 3456), ni);

            while (true) {
               try {
                  byte[] buf = new byte[100];
                  DatagramPacket recv = new DatagramPacket(buf, buf.length);
                  s.receive(recv);
                  System.out.println("Message reçu : " + new String(recv.getData(), 0, recv.getLength()));
               } catch (IOException e) {
                  s.leaveGroup(new InetSocketAddress(group, 3456), ni);
                  s.close();
                  break;
               }
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }
}
