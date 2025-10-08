import java.io.Serializable;

public class Veiculo implements Serializable {
    private static final long serialVersionUID = 1L;

    private String placa;
    private String marca;
    private String modelo;
    private int ano;
    private String cor;
    private double quilometragem;
    private double valor;

    public Veiculo(String placa, String marca, String modelo, int ano, String cor, double quilometragem, double valor) {
        this.placa = placa;
        this.marca = marca;
        this.modelo = modelo;
        this.ano = ano;
        this.cor = cor;
        this.quilometragem = quilometragem;
        this.valor = valor;
    }

    public String getPlaca() { return placa; }
    public void setPlaca(String placa) { this.placa = placa; }
    public String getMarca() { return marca; }
    public void setMarca(String marca) { this.marca = marca; }
    public String getModelo() { return modelo; }
    public void setModelo(String modelo) { this.modelo = modelo; }
    public int getAno() { return ano; }
    public void setAno(int ano) { this.ano = ano; }
    public String getCor() { return cor; }
    public void setCor(String cor) { this.cor = cor; }
    public double getQuilometragem() { return quilometragem; }
    public void setQuilometragem(double quilometragem) { this.quilometragem = quilometragem; }
    public double getValor() { return valor; }
    public void setValor(double valor) { this.valor = valor; }

    @Override
    public String toString() {
        return String.format("veiculo [placa: %s, marca: %s, modelo: %s, ano: %d, cor: %s, KM: %.2f, valor: R$ %.2f]",
               placa, marca, modelo, ano, cor, quilometragem, valor);
    }
}