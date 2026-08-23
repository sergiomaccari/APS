// Testes da sincronizacao online de cotacoes (RF020).
//
// Rodam SEM rede e SEM banco: exercitam apenas as duas partes puras do
// mecanismo, que sao justamente as que concentram a logica.
//
//   FonteYahooFinance::converterJson  - traducao da resposta da API em candles,
//       incluindo os pregoes com campo nulo (feriado/leilao sem negocio) e os
//       erros reportados pela propria API.
//   ServicoSincronizacao::inicioDaJanela - calculo da janela incremental, que
//       decide quanto historico sera pedido a fonte remota.
//
// Os timestamps usados sao epoch UTC das 13h (horario de abertura da B3 em
// UTC), exatamente como a API do Yahoo devolve.

#include <optional>

#include <QByteArray>
#include <QDate>
#include <QObject>
#include <QString>
#include <QTest>
#include <QVector>

#include "dominio/cotacao.h"
#include "servicos/fonteyahoofinance.h"
#include "servicos/servicosincronizacao.h"

namespace analisador
{

// Tolerancia usada nas comparacoes de ponto flutuante dos testes.
static const double TOLERANCIA = 1e-9;

// Epoch (UTC, 13h) dos pregoes de 05, 06, 07 e 08 de janeiro de 2026.
static const qint64 PREGAO_05 = 1767618000;
static const qint64 PREGAO_06 = 1767704400;
static const qint64 PREGAO_07 = 1767790800;
static const qint64 PREGAO_08 = 1767877200;

// Monta uma resposta no formato da chart API v8 do Yahoo Finance. Os textos de
// cada vetor entram crus para que os testes possam inserir "null" onde quiserem.
static QByteArray respostaYahoo(const QString& timestamps,
                                const QString& aberturas,
                                const QString& maximas,
                                const QString& minimas,
                                const QString& fechamentos,
                                const QString& volumes)
{
    const QString modelo = QStringLiteral(
        "{\"chart\":{\"result\":[{"
        "\"meta\":{\"symbol\":\"PETR4.SA\",\"currency\":\"BRL\"},"
        "\"timestamp\":[%1],"
        "\"indicators\":{\"quote\":[{"
        "\"open\":[%2],\"high\":[%3],\"low\":[%4],\"close\":[%5],\"volume\":[%6]}]}"
        "}],\"error\":null}}");

    return modelo.arg(timestamps, aberturas, maximas, minimas, fechamentos, volumes).toUtf8();
}

class TesteSincronizacao : public QObject
{
    Q_OBJECT

private slots:
    void converterJsonLeTresPregoes();
    void converterJsonArredondaPrecosParaCentavos();
    void converterJsonPulaPregaoComPrecoNulo();
    void converterJsonPulaPregaoComVolumeNulo();
    void converterJsonOrdenaPorData();
    void converterJsonFalhaQuandoApiReportaErro();
    void converterJsonFalhaComCorpoQueNaoEhJson();
    void converterJsonAceitaIntervaloSemPregao();

    void inicioDaJanelaSegueUltimaCotacaoLocal();
    void inicioDaJanelaSemHistoricoBuscaUmAno();
    void inicioDaJanelaVaziaQuandoJaAtualizado();
};

void TesteSincronizacao::converterJsonLeTresPregoes()
{
    const QByteArray corpo = respostaYahoo(
        QStringLiteral("%1,%2,%3").arg(PREGAO_05).arg(PREGAO_06).arg(PREGAO_07),
        QStringLiteral("38.40,38.80,39.10"),
        QStringLiteral("38.90,39.20,39.50"),
        QStringLiteral("38.10,38.60,38.95"),
        QStringLiteral("38.75,39.05,39.40"),
        QStringLiteral("41250300,38900100,44120700"));

    QVector<Cotacao> cotacoes;
    QString erro;
    QVERIFY2(FonteYahooFinance::converterJson(corpo, 7, &cotacoes, &erro), qPrintable(erro));
    QVERIFY(erro.isEmpty());
    QCOMPARE(static_cast<int>(cotacoes.size()), 3);

    const Cotacao& primeira = cotacoes.first();
    QCOMPARE(primeira.ativoId(), static_cast<qint64>(7));
    QCOMPARE(primeira.data(), QDate(2026, 1, 5));
    QVERIFY(qAbs(primeira.abertura() - 38.40) < TOLERANCIA);
    QVERIFY(qAbs(primeira.maxima() - 38.90) < TOLERANCIA);
    QVERIFY(qAbs(primeira.minima() - 38.10) < TOLERANCIA);
    QVERIFY(qAbs(primeira.fechamento() - 38.75) < TOLERANCIA);
    QCOMPARE(primeira.volume(), static_cast<qint64>(41250300));
    QVERIFY(primeira.valida());

    QCOMPARE(cotacoes.at(2).data(), QDate(2026, 1, 7));
    QVERIFY(qAbs(cotacoes.at(2).fechamento() - 39.40) < TOLERANCIA);
}

void TesteSincronizacao::converterJsonArredondaPrecosParaCentavos()
{
    // A API devolve varias casas decimais; a B3 negocia em centavos.
    const QByteArray corpo = respostaYahoo(QString::number(PREGAO_05),
                                           QStringLiteral("38.404999732971191"),
                                           QStringLiteral("38.897777557373047"),
                                           QStringLiteral("38.102222442626953"),
                                           QStringLiteral("38.754999160766602"),
                                           QStringLiteral("41250300"));

    QVector<Cotacao> cotacoes;
    QString erro;
    QVERIFY(FonteYahooFinance::converterJson(corpo, 1, &cotacoes, &erro));
    QCOMPARE(static_cast<int>(cotacoes.size()), 1);
    QVERIFY(qAbs(cotacoes.first().abertura() - 38.40) < TOLERANCIA);
    QVERIFY(qAbs(cotacoes.first().maxima() - 38.90) < TOLERANCIA);
    QVERIFY(qAbs(cotacoes.first().minima() - 38.10) < TOLERANCIA);
    QVERIFY(qAbs(cotacoes.first().fechamento() - 38.75) < TOLERANCIA);
}

void TesteSincronizacao::converterJsonPulaPregaoComPrecoNulo()
{
    // Pregao do meio sem fechamento: a linha inteira e descartada, mas as
    // vizinhas continuam sendo convertidas.
    const QByteArray corpo = respostaYahoo(
        QStringLiteral("%1,%2,%3").arg(PREGAO_05).arg(PREGAO_06).arg(PREGAO_07),
        QStringLiteral("38.40,38.80,39.10"),
        QStringLiteral("38.90,39.20,39.50"),
        QStringLiteral("38.10,38.60,38.95"),
        QStringLiteral("38.75,null,39.40"),
        QStringLiteral("41250300,38900100,44120700"));

    QVector<Cotacao> cotacoes;
    QString erro;
    QVERIFY(FonteYahooFinance::converterJson(corpo, 1, &cotacoes, &erro));
    QCOMPARE(static_cast<int>(cotacoes.size()), 2);
    QCOMPARE(cotacoes.at(0).data(), QDate(2026, 1, 5));
    QCOMPARE(cotacoes.at(1).data(), QDate(2026, 1, 7));
}

void TesteSincronizacao::converterJsonPulaPregaoComVolumeNulo()
{
    // Volume nulo com precos validos tambem invalida o candle: preferimos pular
    // o pregao a gravar um OHLCV incompleto.
    const QByteArray corpo = respostaYahoo(
        QStringLiteral("%1,%2").arg(PREGAO_05).arg(PREGAO_06),
        QStringLiteral("38.40,38.80"),
        QStringLiteral("38.90,39.20"),
        QStringLiteral("38.10,38.60"),
        QStringLiteral("38.75,39.05"),
        QStringLiteral("41250300,null"));

    QVector<Cotacao> cotacoes;
    QString erro;
    QVERIFY(FonteYahooFinance::converterJson(corpo, 1, &cotacoes, &erro));
    QCOMPARE(static_cast<int>(cotacoes.size()), 1);
    QCOMPARE(cotacoes.first().data(), QDate(2026, 1, 5));
}

void TesteSincronizacao::converterJsonOrdenaPorData()
{
    const QByteArray corpo = respostaYahoo(
        QStringLiteral("%1,%2,%3").arg(PREGAO_08).arg(PREGAO_05).arg(PREGAO_07),
        QStringLiteral("38.40,38.80,39.10"),
        QStringLiteral("38.90,39.20,39.50"),
        QStringLiteral("38.10,38.60,38.95"),
        QStringLiteral("38.75,39.05,39.40"),
        QStringLiteral("41250300,38900100,44120700"));

    QVector<Cotacao> cotacoes;
    QString erro;
    QVERIFY(FonteYahooFinance::converterJson(corpo, 1, &cotacoes, &erro));
    QCOMPARE(static_cast<int>(cotacoes.size()), 3);
    QCOMPARE(cotacoes.at(0).data(), QDate(2026, 1, 5));
    QCOMPARE(cotacoes.at(1).data(), QDate(2026, 1, 7));
    QCOMPARE(cotacoes.at(2).data(), QDate(2026, 1, 8));
}

void TesteSincronizacao::converterJsonFalhaQuandoApiReportaErro()
{
    const QByteArray corpo = QByteArray(
        "{\"chart\":{\"result\":null,\"error\":{\"code\":\"Not Found\","
        "\"description\":\"No data found, symbol may be delisted\"}}}");

    QVector<Cotacao> cotacoes;
    QString erro;
    QVERIFY(!FonteYahooFinance::converterJson(corpo, 1, &cotacoes, &erro));
    QVERIFY(cotacoes.isEmpty());
    QVERIFY(erro.contains(QStringLiteral("symbol may be delisted")));
}

void TesteSincronizacao::converterJsonFalhaComCorpoQueNaoEhJson()
{
    // Pagina de erro HTML ou resposta truncada nao pode virar cotacao.
    const QByteArray corpo = QByteArray("<html><body>503 Service Unavailable</body></html>");

    QVector<Cotacao> cotacoes;
    QString erro;
    QVERIFY(!FonteYahooFinance::converterJson(corpo, 1, &cotacoes, &erro));
    QVERIFY(cotacoes.isEmpty());
    QVERIFY(!erro.isEmpty());
}

void TesteSincronizacao::converterJsonAceitaIntervaloSemPregao()
{
    // Janela caindo em feriado prolongado: a API devolve resultado sem
    // "timestamp". Nao e erro - simplesmente nao ha nada novo.
    const QByteArray corpo = QByteArray(
        "{\"chart\":{\"result\":[{\"meta\":{\"symbol\":\"PETR4.SA\"},\"indicators\":{}}],"
        "\"error\":null}}");

    QVector<Cotacao> cotacoes;
    QString erro;
    QVERIFY(FonteYahooFinance::converterJson(corpo, 1, &cotacoes, &erro));
    QVERIFY(cotacoes.isEmpty());
    QVERIFY(erro.isEmpty());
}

void TesteSincronizacao::inicioDaJanelaSegueUltimaCotacaoLocal()
{
    Cotacao ultima;
    ultima.definirAtivoId(1);
    ultima.definirData(QDate(2026, 1, 5));
    ultima.definirAbertura(38.40);
    ultima.definirMaxima(38.90);
    ultima.definirMinima(38.10);
    ultima.definirFechamento(38.75);
    ultima.definirVolume(41250300);

    const QDate hoje(2026, 1, 20);
    QCOMPARE(ServicoSincronizacao::inicioDaJanela(std::optional<Cotacao>(ultima), hoje),
             QDate(2026, 1, 6));
}

void TesteSincronizacao::inicioDaJanelaSemHistoricoBuscaUmAno()
{
    const QDate hoje(2026, 1, 20);
    const QDate esperado = hoje.addDays(-ServicoSincronizacao::JANELA_INICIAL_DIAS);

    QCOMPARE(ServicoSincronizacao::inicioDaJanela(std::nullopt, hoje), esperado);
    QCOMPARE(esperado, QDate(2025, 1, 20));
}

void TesteSincronizacao::inicioDaJanelaVaziaQuandoJaAtualizado()
{
    // Ultima cotacao local e a de hoje: o inicio cai depois de hoje, sinal de
    // que o servico nem precisa consultar a fonte remota.
    const QDate hoje(2026, 1, 20);

    Cotacao ultima;
    ultima.definirAtivoId(1);
    ultima.definirData(hoje);
    ultima.definirAbertura(38.40);
    ultima.definirMaxima(38.90);
    ultima.definirMinima(38.10);
    ultima.definirFechamento(38.75);
    ultima.definirVolume(41250300);

    QVERIFY(ServicoSincronizacao::inicioDaJanela(std::optional<Cotacao>(ultima), hoje) > hoje);
}

}

QTEST_APPLESS_MAIN(analisador::TesteSincronizacao)
#include "testesincronizacao.moc"
