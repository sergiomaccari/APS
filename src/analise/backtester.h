#pragma once

// Backtesting das regras de analise (RF021).
//
// Reexecuta o MotorAnalise em cada ponto do historico ja importado, como se o
// pregao daquele dia fosse o mais recente, e confere o parecer emitido contra o
// que o preco fez nos pregoes seguintes (horizonte de avaliacao). O resultado
// mede, por regra e para o parecer consolidado, quantos sinais foram emitidos,
// quantos acertaram a direcao (RN024) e qual o retorno medio de quem tivesse
// seguido o sinal.
//
// A classe e PURA: nao conhece banco de dados nem interface grafica. Recebe o
// ativo, a serie historica e a configuracao das regras; devolve os numeros.
// Isso a torna testavel sem infraestrutura e reutilizavel por qualquer tela.
//
// Convencao de acerto (RN024):
//     Compra acerta quando o retorno futuro e positivo;
//     Venda  acerta quando o retorno futuro e negativo;
//     pareceres Neutro nao sao sinal e nao entram na contabilidade.
// O "retorno ao seguir o sinal" soma +retorno nas compras e -retorno nas
// vendas, de modo que operar vendido em uma queda conta como ganho.

#include <QString>
#include <QVector>

#include "dominio/ativo.h"
#include "dominio/cotacao.h"
#include "dominio/regraconfigurada.h"

namespace analisador
{

// Desempenho historico de uma unica regra (ou do parecer consolidado).
struct ResultadoBacktestRegra
{
    // Nome canonico da regra, ou Backtester::NOME_CONSOLIDADO.
    QString regra;

    // Pareceres diferentes de Neutro emitidos ao longo do historico.
    int sinais = 0;

    // Sinais cuja direcao se confirmou no horizonte avaliado.
    int acertos = 0;

    // Soma dos retornos "ao seguir o sinal" (fracao, nao percentual).
    double somaDosRetornos = 0.0;

    // Proporcao de acertos em [0, 1]. Zero quando nao houve sinal.
    double taxaDeAcerto() const;

    // Retorno medio por sinal (fracao). Zero quando nao houve sinal.
    double retornoMedio() const;
};

// Resultado completo do backtesting de um ativo.
struct ResultadoBacktest
{
    QString ticker;

    // Quantidade de pregoes em que houve reavaliacao das regras.
    int pregoesAvaliados = 0;

    // Pregoes a frente usados para medir o resultado de cada sinal.
    int horizonte = 0;

    ResultadoBacktestRegra consolidado;

    // Uma entrada por regra ativa, na ordem em que o motor as registrou.
    QVector<ResultadoBacktestRegra> porRegra;

    // Vazio quando o ativo foi avaliado; caso contrario explica o motivo
    // (historico curto demais, por exemplo).
    QString observacao;
};

class Backtester
{
public:
    // Pregoes a frente considerados para medir o resultado de um sinal.
    static constexpr int HORIZONTE_PADRAO = 10;

    // Pregoes de historico exigidos antes do primeiro sinal, para que as
    // medias e o RSI tenham serie suficiente para se formar.
    static constexpr int MINIMO_DE_PREGOES = 30;

    // Rotulo da linha do parecer consolidado.
    static const QString NOME_CONSOLIDADO;

    explicit Backtester(const QVector<RegraConfigurada>& regras);

    // Percorre o historico reavaliando as regras e devolve a apuracao.
    // O historico deve estar ordenado por data crescente.
    ResultadoBacktest executar(const Ativo& ativo,
                               const QVector<Cotacao>& historico,
                               int horizonte = HORIZONTE_PADRAO) const;

private:
    QVector<RegraConfigurada> m_regras;
};

}
