#pragma once

// Fonte remota de cotacoes apoiada na chart API v8 do Yahoo Finance (ADR 0003).
//
// Endpoint usado (um request por ativo, intervalo diario):
//     https://query1.finance.yahoo.com/v8/finance/chart/PETR4.SA
//         ?period1=<epoch>&period2=<epoch>&interval=1d
// Tickers da B3 recebem o sufixo ".SA".
//
// A requisicao e SINCRONA (QEventLoop local + transferTimeout) porque a
// sincronizacao e disparada por um clique e ja roda com cursor de espera: o
// codigo do servico fica linear e transacional, sem maquina de callbacks.
//
// A conversao da resposta vive em converterJson(), um metodo estatico e puro -
// sem rede e sem banco - justamente para poder ser testada com um JSON fixo.

#include <QByteArray>
#include <QDate>
#include <QString>
#include <QVector>

#include "dominio/cotacao.h"
#include "servicos/fontecotacoesremota.h"

QT_FORWARD_DECLARE_CLASS(QNetworkAccessManager)

namespace analisador
{

class FonteYahooFinance : public FonteCotacoesRemota
{
public:
    // Tempo maximo de espera por resposta, em milissegundos.
    static const int TEMPO_LIMITE_MS;

    // Sufixo dos tickers brasileiros na API do Yahoo.
    static const QString SUFIXO_B3;

    FonteYahooFinance();
    ~FonteYahooFinance() override;

    FonteYahooFinance(const FonteYahooFinance&) = delete;
    FonteYahooFinance& operator=(const FonteYahooFinance&) = delete;

    QString nome() const override;

    bool buscar(const QString& ticker,
                qint64 ativoId,
                const QDate& de,
                const QDate& ate,
                QVector<Cotacao>* destino,
                QString* erro) override;

    // Converte o corpo JSON devolvido pela API em cotacoes ordenadas por data.
    // Puro e testavel sem rede. Pregoes com qualquer campo nulo (feriado, leilao
    // sem negocio) sao pulados; candles que nao passem em Cotacao::valida()
    // tambem sao descartados. Devolve false com *erro preenchido quando o corpo
    // nao e JSON valido ou quando a propria API reportou erro em chart.error.
    static bool converterJson(const QByteArray& corpo,
                              qint64 ativoId,
                              QVector<Cotacao>* destino,
                              QString* erro);

    // Monta a URL do endpoint para o intervalo informado (period2 = fim do dia
    // de "ate", para que o ultimo pregao entre na janela).
    static QString montarUrl(const QString& ticker, const QDate& de, const QDate& ate);

private:
    QNetworkAccessManager* m_rede = nullptr;
};

}
