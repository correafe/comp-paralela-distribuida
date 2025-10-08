import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

public class Servidor {
    private static Map<String, Veiculo> veiculos = Collections.synchronizedMap(new HashMap<>());

    public static void main(String[] args) throws IOException {
        int PORT = 54321;
        ServerSocket servidorSocket = new ServerSocket(PORT);
        
        System.out.println("servidor iniciado na porta " + PORT);
        System.out.println("aguardando clientes...");

        while (true) {
            Socket socket = servidorSocket.accept();
            System.out.println("cliente conectado: " + socket.getInetAddress().getHostAddress());

            ClientHandler clientHandler = new ClientHandler(socket, veiculos);
            clientHandler.start();
        }
    }
}