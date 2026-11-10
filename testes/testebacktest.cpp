// Testes unitarios do Backtester (RF021).
//
// O Backtester e uma classe pura: nao toca no banco nem na interface. Por isso
// os cenarios usam series sinteticas calculadas a mao, com uma unica regra
// ativa de cada vez, de modo que o parecer emitido em cada pregao e conhecido
// de antemao e as asserções sao deterministicas.
//
// Series usadas (todas com 50 pregoes, horizonte padrao de 10):
//   - ascendente 100, 101, ..., 149: com medias 3/6 a media curta fica sempre
//     acima da longa (pontuacao +0,35 = Compra) e o preco sobe no horizonte,
//     logo todo sinal acerta;
//   - descendente 200, 199, ..., 151: a media curta fica sempre abaixo da longa
//     (pontuacao -0,35 = Venda) e o preco cai, logo toda venda tambem acerta;
//   - a mesma serie descendente avaliada pelo RSI (30/70) produz RSI zero,
//     isto e, sinal de Compra em plena queda: todo sinal erra.
//
// Comparacoes de ponto flutuante usam tolerancia explicita; nunca operador ==.

#include <QDate>
#include <QObject>
#include <QString>
#include <QTest>
#include <QVector>

#include "analise/backtester.h"
#include "dominio/acao.h"
#include "dominio/cotacao.h"
#include "dominio/regraconfigurada.h"

namespace analisador
{

// Tolerancia usada em todas as comparacoes de ponto flutuante dos testes.
static const double TOLERANCIA = 1e-6;

// Monta um historico valido a partir de uma lista de fechamentos, com datas
// sequenciais a partir de 05/01/2026.
static QVector<Cotacao> serie(const QVector<double>& fechamentos)
{
    QVector<Cotacao> historico;
    historico.reserve(fechamentos.size());

    const QDate primeiraData(2026, 1, 5);
    for (int i = 0; i < fechamentos.size(); ++i)
    {
        const double fechamento = fechamentos.at(i);
        historico.append(Cotacao(i + 1,
                                 1,
                                 primeiraData.addDays(i),
                                 fechamento,
                                 fechamento * 1.01,
                                 fechamento * 0.99,
                                 fechamento,
                                 1000000 + i));
    }
    return historico;
}

// Serie aritmetica de "quantidade" pregoes comecando em "inicio" e variando
// "passo" por dia (passo negativo produz serie descendente).
static QVector<Cotacao> serieLinear(double inicio, double passo, int quantidade)
{
    QVector<double> fechamentos;
    fechamentos.reserve(quantidade);
    for (int i = 0; i < quantidade; ++i)
    {
        fechamentos.append(inicio + passo * i);
    }
    return serie(fechamentos);
}

// Configuracao com uma unica regra ativa, para isolar o comportamento avaliado.
static QVector<RegraConfigurada> apenasRegra(const QString& nome,
                                             double parametroPrincipal,
                                             double parametroSecundario)
{
    QVector<RegraConfigurada> regras;
    regras.append(RegraConfigurada(1, nome, true, parametroPrincipal, parametroSecundario));
    return regras;
}

// Acao de demonstracao usada em todos os cenarios.
static Acao acaoDeTeste()
{
    return Acao(1,
                QStringLiteral("PETR4"),
                QStringLiteral("Petroleo Brasileiro S.A."),
                QStringLiteral("Petroleo, Gas e Biocombustiveis"),
                0.0,
                0.0,
                495000000000.0);
}

class TesteBacktest : public QObject
{
    Q_OBJECT

private slots:
    void apuracaoSemSinaisDevolveZeros();
    void serieAscendenteAcertaTodasAsCompras();
    void serieDescendenteAcertaTodasAsVendas();
    void sinalContrarioATendenciaErraTodasAsVezes();
    void historicoCurtoNaoEAvaliado();
    void horizonteEscolhidoMudaOsPregoesAvaliados();
};

void TesteBacktest::apuracaoSemSinaisDevolveZeros()
{
    // Sem sinais nao ha divisao: taxa e retorno medio valem zero.
    const ResultadoBacktestRegra vazia;
    QCOMPARE(vazia.sinais, 0);
    QCOMPARE(vazia.acertos, 0);
    QVERIFY(qAbs(vazia.taxaDeAcerto()) < TOLERANCIA);
    QVERIFY(qAbs(vazia.retornoMedio()) < TOLERANCIA);
}

void TesteBacktest::serieAscendenteAcertaTodasAsCompras()
{
    // Medias 3/6 sobre 100..149: MMS3 = preco - 1 e MMS6 = preco - 2,5, ou seja,
    // a curta fica sempre acima da longa (+0,35 = Compra) e o preco sobe no
    // horizonte de 10 pregoes. Sao avaliados os pregoes de indice 29 a 39.
    const Backtester backtester(apenasRegra(QStringLiteral("Cruzamento de Medias Moveis"), 3.0, 6.0));
    const Acao acao = acaoDeTeste();

    const ResultadoBacktest resultado = backtester.executar(acao, serieLinear(100.0, 1.0, 50));

    QCOMPARE(resultado.ticker, acao.ticker());
    QCOMPARE(resultado.horizonte, Backtester::HORIZONTE_PADRAO);
    QVERIFY(resultado.observacao.isEmpty());
    QCOMPARE(resultado.pregoesAvaliados, 11);

    QCOMPARE(resultado.consolidado.sinais, 11);
    QCOMPARE(resultado.consolidado.acertos, 11);
    QVERIFY(resultado.consolidado.sinais > 0);
    QVERIFY(resultado.consolidado.acertos <= resultado.consolidado.sinais);
    QVERIFY(qAbs(resultado.consolidado.taxaDeAcerto() - 1.0) < TOLERANCIA);
    QVERIFY(resultado.consolidado.retornoMedio() > 0.0);

    // Uma unica regra ativa: a linha por regra repete o consolidado.
    QCOMPARE(static_cast<int>(resultado.porRegra.size()), 1);
    const ResultadoBacktestRegra& cruzamento = resultado.porRegra.first();
    QCOMPARE(cruzamento.regra, QStringLiteral("Cruzamento de Medias Moveis"));
    QCOMPARE(cruzamento.sinais, 11);
    QCOMPARE(cruzamento.acertos, 11);
    QVERIFY(qAbs(cruzamento.taxaDeAcerto() - 1.0) < TOLERANCIA);
    QVERIFY(cruzamento.retornoMedio() > 0.0);
}

void TesteBacktest::serieDescendenteAcertaTodasAsVendas()
{
    // Serie 200..151 com medias 3/6: a curta fica sempre abaixo da longa
    // (-0,35 = Venda) e o preco cai no horizonte. Quem vendeu acertou, e o
    // retorno de quem seguiu o sinal (vendido) e positivo.
    const Backtester backtester(apenasRegra(QStringLiteral("Cruzamento de Medias Moveis"), 3.0, 6.0));
    const Acao acao = acaoDeTeste();

    const ResultadoBacktest resultado = backtester.executar(acao, serieLinear(200.0, -1.0, 50));

    QVERIFY(resultado.observacao.isEmpty());
    QCOMPARE(resultado.pregoesAvaliados, 11);
    QCOMPARE(resultado.consolidado.sinais, 11);
    QCOMPARE(resultado.consolidado.acertos, resultado.consolidado.sinais);
    QVERIFY(qAbs(resultado.consolidado.taxaDeAcerto() - 1.0) < TOLERANCIA);
    QVERIFY(resultado.consolidado.retornoMedio() > 0.0);
    QVERIFY(resultado.consolidado.somaDosRetornos > 0.0);
}

void TesteBacktest::sinalContrarioATendenciaErraTodasAsVezes()
{
    // Na mesma queda, o RSI (30/70) marca zero e insiste em Compra: todos os
    // sinais erram a direcao e o retorno medio de quem os seguisse e negativo.
    const Backtester backtester(apenasRegra(QStringLiteral("Indice de Forca Relativa"), 30.0, 70.0));
    const Acao acao = acaoDeTeste();

    const ResultadoBacktest resultado = backtester.executar(acao, serieLinear(200.0, -1.0, 50));

    QVERIFY(resultado.observacao.isEmpty());
    QCOMPARE(resultado.consolidado.sinais, 11);
    QCOMPARE(resultado.consolidado.acertos, 0);
    QVERIFY(resultado.consolidado.acertos <= resultado.consolidado.sinais);
    QVERIFY(qAbs(resultado.consolidado.taxaDeAcerto()) < TOLERANCIA);
    QVERIFY(resultado.consolidado.retornoMedio() < 0.0);

    QCOMPARE(static_cast<int>(resultado.porRegra.size()), 1);
    QCOMPARE(resultado.porRegra.first().regra, QStringLiteral("Indice de Forca Relativa"));
    QCOMPARE(resultado.porRegra.first().acertos, 0);
}

void TesteBacktest::historicoCurtoNaoEAvaliado()
{
    // Faltam pregoes para formar as regras e ainda medir o horizonte: o
    // Backtester nao emite nenhum sinal e explica o motivo.
    const Backtester backtester(apenasRegra(QStringLiteral("Cruzamento de Medias Moveis"), 3.0, 6.0));
    const Acao acao = acaoDeTeste();

    const ResultadoBacktest resultado = backtester.executar(acao, serieLinear(100.0, 1.0, 20));

    QVERIFY(!resultado.observacao.isEmpty());
    QCOMPARE(resultado.pregoesAvaliados, 0);
    QCOMPARE(resultado.consolidado.sinais, 0);
    QCOMPARE(resultado.consolidado.acertos, 0);
    QVERIFY(qAbs(resultado.consolidado.taxaDeAcerto()) < TOLERANCIA);
    QVERIFY(qAbs(resultado.consolidado.retornoMedio()) < TOLERANCIA);

    // A serie vazia tambem e tratada sem acesso invalido.
    const QVector<Cotacao> vazio;
    const ResultadoBacktest semDados = backtester.executar(acao, vazio);
    QVERIFY(!semDados.observacao.isEmpty());
    QCOMPARE(semDados.pregoesAvaliados, 0);
}

void TesteBacktest::horizonteEscolhidoMudaOsPregoesAvaliados()
{
    // Com horizonte de 5 pregoes sobram mais pontos avaliaveis (indices 29 a 44).
    const Backtester backtester(apenasRegra(QStringLiteral("Cruzamento de Medias Moveis"), 3.0, 6.0));
    const Acao acao = acaoDeTeste();

    const ResultadoBacktest resultado = backtester.executar(acao, serieLinear(100.0, 1.0, 50), 5);

    QCOMPARE(resultado.horizonte, 5);
    QCOMPARE(resultado.pregoesAvaliados, 16);
    QCOMPARE(resultado.consolidado.sinais, 16);
    QCOMPARE(resultado.consolidado.acertos, 16);
    QVERIFY(resultado.consolidado.retornoMedio() > 0.0);

    // Horizonte invalido cai no padrao, sem quebrar a apuracao.
    const ResultadoBacktest padrao = backtester.executar(acao, serieLinear(100.0, 1.0, 50), 0);
    QCOMPARE(padrao.horizonte, Backtester::HORIZONTE_PADRAO);
    QCOMPARE(padrao.pregoesAvaliados, 11);
}

}

QTEST_APPLESS_MAIN(analisador::TesteBacktest)
#include "testebacktest.moc"
