#pragma once

// Cotacao diaria de um ativo (candle OHLCV). Base de todos os calculos tecnicos.

#include <QDate>
#include <QString>

namespace analisador
{

class Cotacao
{
public:
    Cotacao();
    Cotacao(qint64 id,
            qint64 ativoId,
            const QDate& data,
            double abertura,
            double maxima,
            double minima,
            double fechamento,
            qint64 volume);

    qint64 id() const;
    qint64 ativoId() const;
    QDate data() const;
    double abertura() const;
    double maxima() const;
    double minima() const;
    double fechamento() const;
    qint64 volume() const;

    void definirId(qint64 id);
    void definirAtivoId(qint64 ativoId);
    void definirData(const QDate& data);
    void definirAbertura(double abertura);
    void definirMaxima(double maxima);
    void definirMinima(double minima);
    void definirFechamento(double fechamento);
    void definirVolume(qint64 volume);

    // Variacao percentual do dia: (fechamento - abertura) / abertura * 100.
    double variacaoPercentual() const;

    // Diferenca entre maxima e minima do dia.
    double amplitude() const;

    // Verdadeiro quando o candle fechou em alta (fechamento maior ou igual a abertura).
    bool ehAlta() const;

    // Consistencia do candle: data valida, precos positivos e minima <= precos <= maxima.
    bool valida() const;

    // Comparacao por data, usada para ordenar series historicas.
    bool operator<(const Cotacao& outra) const;

private:
    qint64 m_id = 0;
    qint64 m_ativoId = 0;
    QDate m_data;
    double m_abertura = 0.0;
    double m_maxima = 0.0;
    double m_minima = 0.0;
    double m_fechamento = 0.0;
    qint64 m_volume = 0;
};

}
