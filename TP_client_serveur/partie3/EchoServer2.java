import java.io.*;
import java.net.*;

/**
 * This module contains the application logic of an echo server
 * which uses a stream socket for interprocess  communication.
 * A command-line argument is required to specify the server port.
 * @author M. L. Liu
 */
public class EchoServer2 {
   static final String endMessage = ".";
   public static void main(String[] args) {
      int serverPort = 7;    // default port
      String message;

      if (args.length == 1 )
         serverPort = Integer.parseInt(args[0]);       
      try {
         // instantiates a stream socket for accepting
         //   connections
   	   ServerSocket myConnectionSocket = 
            new ServerSocket(serverPort); 
/**/     System.out.println("Daytime server ready.");  
         while (true) {  // forever loop
            // wait to accept a connection 
/**/        System.out.println("Waiting for a connection.");
            MyStreamSocket myDataSocket = new MyStreamSocket
                (myConnectionSocket.accept( ));
/**/        System.out.println("connection accepted");
            boolean done = false;
            while (!done) {
               message = myDataSocket.receiveMessage( );
/**/           System.out.println("message received: "+ message);

               if ((message.trim()).equals (endMessage)){
                  //Session over; close the data socket.
/**/              System.out.println("Session over.");
                  myDataSocket.close( );
                  done = true;
                  continue;
               } 

               String[] parse = message.split(" ");

               if(parse.length != 3){
                  myDataSocket.sendMessage("Invalid input: Please provide an operator and two operands.");
               }
               else{
                  String validationResult = Calculatrice.valide_input(parse[0], parse[1], parse[2]);
                  System.out.println("Validation result: " + validationResult);
                  if(validationResult != ""){
                     myDataSocket.sendMessage(validationResult);
                  }else{
                     int result = Calculatrice.calculate(parse[0], parse[1], parse[2]);
                     myDataSocket.sendMessage(message + " = " + result);
                  }
               }

		       } //end while !done
         } //end while forever
       } // end try
	    catch (Exception ex) {
          ex.printStackTrace( );
	    }
   } //end main
} // end class
