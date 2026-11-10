#pragma once

// Posicao de um ativo dentro de uma carteira: quantidade, preco medio e data de compra.

#include <QDate>

namespace analisador
{

class Posicao
{
public:
    Posicao();
    Posicao(qint64 id,
            qint64 carteiraId,
            qint64 ativoId,
            int quantidade,
            double precoMedio,
            const QDate& compradaEm);

    qint64 id() const;
    qint64 carteiraId() const;
    qint64 ativoId() const;
    int quantidade() const;
    double precoMedio() const;
    QDate compradaEm() const;

    void definirId(qint64 id);
    void definirCarteiraId(qint64 carteiraId);
    void definirAtivoId(qint64 ativoId);
    void definirQuantidade(int quantidade);
    void definirPrecoMedio(double precoMedio);
    void definirCompradaEm(const QDate& compradaEm);

    // Capital investido na posicao: quantidade multiplicada pelo preco medio.
    double custoTotal() const;

    // Valor da posicao ao preco informado.
    double valorAtual(double precoAtual) const;

    // Lucro (positivo) ou prejuizo (negativo) em reais ao preco informado.
    double lucroPrejuizo(double precoAtual) const;

    // Rentabilidade percentual sobre o custo total.
    double rentabilidadePercentual(double precoAtual) const;

    // Incorpora uma nova compra recalculando o preco medio ponderado.
    // Retorna false quando a quantidade ou o preco informados nao sao positivos.
    bool incorporarCompra(int quantidadeComprada, double precoCompra);

    // Reduz a quantidade da posicao. Retorna false quando a venda excede o estoque.
    bool venderParcial(int quantidadeVendida);

    // Verdadeiro quando a posicao foi zerada.
    bool zerada() const;

private:
    qint64 m_id = 0;
    qint64 m_carteiraId = 0;
    qint64 m_ativoId = 0;
    int m_quantidade = 0;
    double m_precoMedio = 0.0;
    QDate m_compradaEm;
};

}
