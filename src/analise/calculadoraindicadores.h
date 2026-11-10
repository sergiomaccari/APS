#pragma once

// Calculos tecnicos sobre series historicas de cotacoes.
//
// Todos os metodos sao estaticos e assumem o historico ORDENADO POR DATA
// CRESCENTE. Series curtas nunca provocam acesso invalido: os metodos devolvem
// vetor vazio ou std::nullopt quando nao ha dados suficientes.

#include <optional>

#include <QVector>

#include "dominio/cotacao.h"
#include "dominio/indicador.h"

namespace analisador
{

class CalculadoraIndicadores
{
public:
    // Media Movel Simples (MMS): media aritmetica dos ultimos N fechamentos.
    //     MMS(t) = (F(t) + F(t-1) + ... + F(t-N+1)) / N
    static QVector<Indicador> mediaMovelSimples(const QVector<Cotacao>& historico, int periodo);

    // Media Movel Exponencial (MME): da mais peso aos fechamentos recentes.
    //     k = 2 / (N + 1);  MME(t) = F(t) * k + MME(t-1) * (1 - k)
    // A primeira MME e semeada com a media simples dos N primeiros fechamentos.
    static QVector<Indicador> mediaMovelExponencial(const QVector<Cotacao>& historico, int periodo);

    // Indice de Forca Relativa (RSI) de Wilder, em [0, 100].
    //     RS = media dos ganhos / media das perdas;  RSI = 100 - 100 / (1 + RS)
    // As medias iniciais sao simples e as seguintes suavizadas por Wilder:
    //     media(t) = (media(t-1) * (N - 1) + valor(t)) / N
    static QVector<Indicador> indiceForcaRelativa(const QVector<Cotacao>& historico, int periodo = 14);

    // Variacao percentual do fechamento nos ultimos "dias" pregoes.
    //     ((F(final) - F(inicial)) / F(inicial)) * 100
    static std::optional<double> variacaoPercentual(const QVector<Cotacao>& historico, int dias);

    // Volatilidade anualizada (%): desvio padrao dos retornos diarios da janela
    // multiplicado pela raiz de 252 (pregoes por ano).
    static std::optional<double> volatilidadeAnualizada(const QVector<Cotacao>& historico, int janela);

    // Retorno percentual acumulado de toda a serie.
    static std::optional<double> retornoAcumulado(const QVector<Cotacao>& historico);

    // Ultimo preco de fechamento da serie.
    static std::optional<double> ultimoFechamento(const QVector<Cotacao>& historico);

    // Ultimo valor de uma serie de indicadores (ex.: ultima MMS calculada).
    static std::optional<double> ultimoValor(const QVector<Indicador>& serie);

    // Recorte com as ultimas "quantidade" cotacoes (a serie inteira se for menor).
    static QVector<Cotacao> ultimasCotacoes(const QVector<Cotacao>& historico, int quantidade);

    // Pregoes considerados em um ano, usado na anualizacao da volatilidade.
    static const int PREGOES_POR_ANO;
};

}
