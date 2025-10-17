import java.net.*;
import java.io.*;
import java.util.ArrayList;

public class Example2SenderReceiver {
   public static void main(String[] args) {
      try {
         register(args[0], args[1]);
         MyDatagramSocket socket = new MyDatagramSocket(Integer.parseInt(args[1]));
         ArrayList<String> rec_info = get_register(args[2]);
         if (rec_info == null) {
            System.out.println("Impossible de récupérer les informations d'enregistrement.");
            return;
         }
         // Boucle d’envoi de messages
         while (true) {
            socket.sendMessage(InetAddress.getByName(rec_info.get(0)), Integer.parseInt(rec_info.get(1)), args[3]);
            System.out.println("Message envoyé : " + args[3]);

            String reponse = socket.receiveMessage();
            System.out.println("Réponse reçue : " + reponse);

            Thread.sleep(3000);
         }

      } catch (Exception e) {
         e.printStackTrace();
      }
   }
   public static void register(String username, String port) {
      try {
         BufferedWriter out = new BufferedWriter(new FileWriter(username + ".txt", true));
         String hostname = InetAddress.getLocalHost().getHostName();
         out.write(hostname + "\n");
         out.write(port + "\n");
         out.close();
      } catch (IOException e) {}
   }

   public static ArrayList<String> get_register(String username) {
      try {
         BufferedReader in = new BufferedReader(new FileReader(username + ".txt"));
         ArrayList<String> ret = new ArrayList<>();
         ret.add(in.readLine());
         ret.add(in.readLine());
         in.close();
         return ret;
      } catch (IOException e) {
         System.out.println("username inconnu");
         return null;
      }
   }
}
