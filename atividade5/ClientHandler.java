import java.io.EOFException;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.Socket;
import java.util.ArrayList;
import java.util.Map;

public class ClientHandler extends Thread {
    private Socket socket;
    private Map<String, Veiculo> veiculos;
    private ObjectInputStream in;
    private ObjectOutputStream out;

    public ClientHandler(Socket socket, Map<String, Veiculo> veiculos) {
        this.socket = socket;
        this.veiculos = veiculos;
    }

    @Override
    public void run() {
        try {
            out = new ObjectOutputStream(socket.getOutputStream());
            in = new ObjectInputStream(socket.getInputStream());

            while (true) {
                String operacao = (String) in.readObject(); 
                
                switch (operacao) {
                    case "INSERIR": {
                        Veiculo v = (Veiculo) in.readObject(); 
                        veiculos.put(v.getPlaca(), v);
                        out.writeObject("veiculo inserido com sucesso!"); 
                        break;
                    }
                    case "REMOVER": {
                        String placa = (String) in.readObject(); 
                        if (veiculos.remove(placa) != null) {
                            out.writeObject("veiculo removido com sucesso!");
                        } else {
                            out.writeObject("ERRO: veiculo com a placa nao encontrado.");
                        }
                        break;
                    }
                    case "BUSCAR": {
                        String placa = (String) in.readObject(); 
                        Veiculo v = veiculos.get(placa);
                        out.writeObject(v); 
                        break;
                    }
                    case "ALTERAR": {
                        Veiculo v = (Veiculo) in.readObject();
                        if (veiculos.containsKey(v.getPlaca())) {
                            veiculos.put(v.getPlaca(), v);
                            out.writeObject("veiculo alterado com sucesso!");
                        } else {
                             out.writeObject("ERRO: veiculo nao encontrado para alteracao.");
                        }
                        break;
                    }
                    case "LISTAR_TODOS": {
                        out.writeObject(new ArrayList<>(veiculos.values()));
                        break;
                    }
                }
                out.flush();
            }
        } catch (EOFException e) {
            System.out.println("cliente desconectado: " + socket.getInetAddress().getHostAddress());
        } catch (IOException | ClassNotFoundException e) {
            System.err.println("erro na comunicacao com cliente: " + e.getMessage());
        } finally {
            try {
                socket.close();
            } catch (IOException e) {
                System.err.println("erro ao fechar socket: " + e.getMessage());
            }
        }
    }
}