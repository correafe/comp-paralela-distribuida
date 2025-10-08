import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.Socket;
import java.util.ArrayList;
import java.util.Scanner;

public class Cliente {
    private static final String SERVER_IP = "127.0.0.1";
    private static final int SERVER_PORT = 54321;

    public static void main(String[] args) {
        try (Socket socket = new Socket(SERVER_IP, SERVER_PORT);
             ObjectOutputStream out = new ObjectOutputStream(socket.getOutputStream());
             ObjectInputStream in = new ObjectInputStream(socket.getInputStream());
             Scanner scanner = new Scanner(System.in)) {

            System.out.println("conectado ao servidor.");

            while (true) {
                exibirMenu();
                int escolha = Integer.parseInt(scanner.nextLine());

                try {
                    switch (escolha) {
                        case 1: inserirVeiculo(scanner, out, in); break;
                        case 2: removerVeiculo(scanner, out, in); break;
                        case 3: buscarVeiculo(scanner, out, in); break;
                        case 4: alterarVeiculo(scanner, out, in); break;
                        case 5: listarTodos(out, in); break;
                        case 0: System.out.println("desconectando..."); return;
                        default: System.out.println("opcao invalida.");
                    }
                } catch (IOException | ClassNotFoundException e) {
                    System.err.println("erro de comunicacao: " + e.getMessage());
                }
            }
        } catch (Exception e) {
            System.err.println("nao foi possivel conectar ao servidor: " + e.getMessage());
        }
    }
    
    private static void exibirMenu() {
        System.out.println("\n--- gestao de veiculos ---");
        System.out.println("1. inserir");
        System.out.println("2. remover");
        System.out.println("3. buscar por placa");
        System.out.println("4. alterar");
        System.out.println("5. listar todos");
        System.out.println("0. sair");
        System.out.print("escolha uma opcao: ");
    }
    
    private static void inserirVeiculo(Scanner scanner, ObjectOutputStream out, ObjectInputStream in) throws IOException, ClassNotFoundException {
        System.out.print("placa: "); String placa = scanner.nextLine();
        System.out.print("marca: "); String marca = scanner.nextLine();
        System.out.print("modelo: "); String modelo = scanner.nextLine();
        System.out.print("ano: "); int ano = Integer.parseInt(scanner.nextLine());
        System.out.print("cor: "); String cor = scanner.nextLine();
        System.out.print("KM: "); double km = Double.parseDouble(scanner.nextLine());
        System.out.print("valor: "); double valor = Double.parseDouble(scanner.nextLine());

        Veiculo v = new Veiculo(placa, marca, modelo, ano, cor, km, valor);
        out.writeObject("INSERIR");
        out.writeObject(v);
        out.flush();
        
        String resposta = (String) in.readObject();
        System.out.println("servidor: " + resposta);
    }
    
    private static void removerVeiculo(Scanner scanner, ObjectOutputStream out, ObjectInputStream in) throws IOException, ClassNotFoundException {
        System.out.print("placa do veiculo para remover: ");
        String placa = scanner.nextLine();
        out.writeObject("REMOVER");
        out.writeObject(placa);
        out.flush();
        
        String resposta = (String) in.readObject();
        System.out.println("servidor: " + resposta);
    }
    
    private static void buscarVeiculo(Scanner scanner, ObjectOutputStream out, ObjectInputStream in) throws IOException, ClassNotFoundException {
        System.out.print("placa do veiculo para buscar: ");
        String placa = scanner.nextLine();
        out.writeObject("BUSCAR");
        out.writeObject(placa);
        out.flush();
        
        Veiculo v = (Veiculo) in.readObject();
        if (v != null) {
            System.out.println("veiculo encontrado: " + v);
        } else {
            System.out.println("veiculo nao encontrado.");
        }
    }
    
    private static void alterarVeiculo(Scanner scanner, ObjectOutputStream out, ObjectInputStream in) throws IOException, ClassNotFoundException {
        System.out.print("placa do veiculo para alterar: ");
        String placa = scanner.nextLine();
        System.out.println("digite os novos dados:");
        System.out.print("marca: "); String marca = scanner.nextLine();
        System.out.print("modelo: "); String modelo = scanner.nextLine();
        System.out.print("ano: "); int ano = Integer.parseInt(scanner.nextLine());
        System.out.print("cor: "); String cor = scanner.nextLine();
        System.out.print("KM: "); double km = Double.parseDouble(scanner.nextLine());
        System.out.print("valor: "); double valor = Double.parseDouble(scanner.nextLine());

        Veiculo v = new Veiculo(placa, marca, modelo, ano, cor, km, valor);
        out.writeObject("ALTERAR");
        out.writeObject(v);
        out.flush();
        
        String resposta = (String) in.readObject();
        System.out.println("servidor: " + resposta);
    }
    
    private static void listarTodos(ObjectOutputStream out, ObjectInputStream in) throws IOException, ClassNotFoundException {
        out.writeObject("LISTAR_TODOS");
        out.flush();
        
        ArrayList<Veiculo> lista = (ArrayList<Veiculo>) in.readObject();
        if (lista.isEmpty()) {
            System.out.println("nenhum veiculo cadastrado.");
        } else {
            System.out.println("--- lista de veiculos ---");
            for (Veiculo v : lista) {
                System.out.println(v);
            }
        }
    }
}