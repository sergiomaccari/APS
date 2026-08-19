// Testes unitarios da CalculadoraIndicadores (Qt Test).
//
// Cobre os calculos tecnicos com valores conferidos a mao e os casos de borda
// (historico vazio, historico menor que o periodo, periodo zero ou negativo),
// em que os metodos precisam devolver vetor vazio ou std::nullopt sem falhar.
//
// Comparacoes de ponto flutuante usam tolerancia explicita; nunca operador ==.

#include <optional>

#include <QDate>
#include <QObject>
#include <QString>
#include <QTest>
#include <QVector>

#include "analise/calculadoraindicadores.h"
#include "dominio/cotacao.h"

namespace analisador
{

// Tolerancia usada em todas as comparacoes de ponto flutuante dos testes.
static const double TOLERANCIA = 1e-6;

// Monta um historico valido a partir de uma lista de fechamentos.
// As datas sao sequenciais a partir de 05/01/2026 e os demais precos do candle
// sao derivados do fechamento, mantendo minima <= abertura/fechamento <= maxima.
static QVector<Cotacao> serie(const QVector<double>& fechamentos)
{
    QVector<Cotacao> historico;
    historico.reserve(fechamentos.size());

    const QDate primeiraData(2026, 1, 5);
    for (int i = 0; i < fechamentos.size(); ++i)
    {
        const double fechamento = fechamentos.at(i);
        const double maxima = fechamento >= 0.0 ? fechamento * 1.01 : fechamento * 0.99;
        const double minima = fechamento >= 0.0 ? fechamento * 0.99 : fechamento * 1.01;
        historico.append(Cotacao(i + 1,
                                 1,
                                 primeiraData.addDays(i),
                                 fechamento,
                                 maxima,
                                 minima,
                                 fechamento,
                                 1000000 + i));
    }
    return historico;
}

class TesteIndicadores : public QObject
{
    Q_OBJECT

private slots:
    // Media movel simples
    void mediaMovelSimplesComValoresConhecidos();
    void mediaMovelSimplesNomeiaEDataOsPontos();
    void mediaMovelSimplesComPeriodoIgualAoHistorico();
    void mediaMovelSimplesComHistoricoMenorQuePeriodo();
    void mediaMovelSimplesComPeriodoInvalido();
    void mediaMovelSimplesComHistoricoVazio();

    // Media movel exponencial
    void mediaMovelExponencialSemeadaPelaSimples();
    void mediaMovelExponencialComHistoricoInsuficiente();
    void mediaMovelExponencialComPeriodoInvalido();

    // Indice de forca relativa
    void indiceForcaRelativaSerieSoDeAltasVaiACem();
    void indiceForcaRelativaSerieSoDeQuedasVaiAZero();
    void indiceForcaRelativaSerieAlternadaFicaNaFaixaNeutra();
    void indiceForcaRelativaExigeUmaCotacaoAlemDoPeriodo();
    void indiceForcaRelativaComPeriodoInvalido();
    void indiceForcaRelativaComHistoricoVazio();

    // Variacao percentual
    void variacaoPercentualEntreDoisPregoes();
    void variacaoPercentualLimitadaPeloTamanhoDaSerie();
    void variacaoPercentualNegativa();
    void variacaoPercentualComEntradasInvalidas();

    // Volatilidade anualizada
    void volatilidadeAnualizadaZeradaComRetornosConstantes();
    void volatilidadeAnualizadaPositivaComOscilacao();
    void volatilidadeAnualizadaComEntradasInvalidas();

    // Retorno acumulado e auxiliares
    void retornoAcumuladoPositivoENegativo();
    void retornoAcumuladoComHistoricoInsuficiente();
    void ultimoFechamentoEUltimoValor();
    void ultimasCotacoesRecortaOFinalDaSerie();
    void historicoVazioNaoQuebraNenhumMetodo();
};

// ---------------------------------------------------------------- MMS

void TesteIndicadores::mediaMovelSimplesComValoresConhecidos()
{
    // Fechamentos 10, 11, 12, 13, 14 com periodo 3:
    //   (10+11+12)/3 = 11 ; (11+12+13)/3 = 12 ; (12+13+14)/3 = 13
    const QVector<Cotacao> historico = serie({10.0, 11.0, 12.0, 13.0, 14.0});
    const QVector<Indicador> media = CalculadoraIndicadores::mediaMovelSimples(historico, 3);

    QCOMPARE(static_cast<int>(media.size()), 3);
    QVERIFY(qAbs(media.at(0).valor - 11.0) < TOLERANCIA);
    QVERIFY(qAbs(media.at(1).valor - 12.0) < TOLERANCIA);
    QVERIFY(qAbs(media.at(2).valor - 13.0) < TOLERANCIA);
}

void TesteIndicadores::mediaMovelSimplesNomeiaEDataOsPontos()
{
    const QVector<Cotacao> historico = serie({10.0, 11.0, 12.0, 13.0, 14.0});
    const QVector<Indicador> media = CalculadoraIndicadores::mediaMovelSimples(historico, 3);

    QVERIFY(!media.isEmpty());
    QCOMPARE(media.first().nome, QStringLiteral("MMS 3"));
    // O primeiro ponto pertence a terceira cotacao e o ultimo a cotacao final.
    QCOMPARE(media.first().data, historico.at(2).data());
    QCOMPARE(media.last().data, historico.last().data());
    QVERIFY(media.first().valido());
}

void TesteIndicadores::mediaMovelSimplesComPeriodoIgualAoHistorico()
{
    const QVector<Cotacao> historico = serie({10.0, 20.0, 30.0});
    const QVector<Indicador> media = CalculadoraIndicadores::mediaMovelSimples(historico, 3);

    QCOMPARE(static_cast<int>(media.size()), 1);
    QVERIFY(qAbs(media.first().valor - 20.0) < TOLERANCIA);
}

void TesteIndicadores::mediaMovelSimplesComHistoricoMenorQuePeriodo()
{
    const QVector<Cotacao> historico = serie({10.0, 11.0});
    QVERIFY(CalculadoraIndicadores::mediaMovelSimples(historico, 9).isEmpty());
}

void TesteIndicadores::mediaMovelSimplesComPeriodoInvalido()
{
    const QVector<Cotacao> historico = serie({10.0, 11.0, 12.0, 13.0});
    QVERIFY(CalculadoraIndicadores::mediaMovelSimples(historico, 0).isEmpty());
    QVERIFY(CalculadoraIndicadores::mediaMovelSimples(historico, -5).isEmpty());
}

void TesteIndicadores::mediaMovelSimplesComHistoricoVazio()
{
    const QVector<Cotacao> vazio;
    QVERIFY(CalculadoraIndicadores::mediaMovelSimples(vazio, 9).isEmpty());
    QVERIFY(CalculadoraIndicadores::mediaMovelSimples(vazio, 0).isEmpty());
}

// ---------------------------------------------------------------- MME

void TesteIndicadores::mediaMovelExponencialSemeadaPelaSimples()
{
    // Periodo 3 => k = 2 / 4 = 0,5.
    // Semente: (10+11+12)/3 = 11
    // 13 * 0,5 + 11 * 0,5 = 12 ; 14 * 0,5 + 12 * 0,5 = 13
    const QVector<Cotacao> historico = serie({10.0, 11.0, 12.0, 13.0, 14.0});
    const QVector<Indicador> media = CalculadoraIndicadores::mediaMovelExponencial(historico, 3);

    QCOMPARE(static_cast<int>(media.size()), 3);
    QCOMPARE(media.first().nome, QStringLiteral("MME 3"));
    QVERIFY(qAbs(media.at(0).valor - 11.0) < TOLERANCIA);
    QVERIFY(qAbs(media.at(1).valor - 12.0) < TOLERANCIA);
    QVERIFY(qAbs(media.at(2).valor - 13.0) < TOLERANCIA);

    // O primeiro ponto da MME coincide com o primeiro ponto da MMS.
    const QVector<Indicador> simples = CalculadoraIndicadores::mediaMovelSimples(historico, 3);
    QVERIFY(!simples.isEmpty());
    QVERIFY(qAbs(media.first().valor - simples.first().valor) < TOLERANCIA);
}

void TesteIndicadores::mediaMovelExponencialComHistoricoInsuficiente()
{
    const QVector<Cotacao> historico = serie({10.0, 11.0});
    QVERIFY(CalculadoraIndicadores::mediaMovelExponencial(historico, 9).isEmpty());

    const QVector<Cotacao> vazio;
    QVERIFY(CalculadoraIndicadores::mediaMovelExponencial(vazio, 3).isEmpty());
}

void TesteIndicadores::mediaMovelExponencialComPeriodoInvalido()
{
    const QVector<Cotacao> historico = serie({10.0, 11.0, 12.0, 13.0});
    QVERIFY(CalculadoraIndicadores::mediaMovelExponencial(historico, 0).isEmpty());
    QVERIFY(CalculadoraIndicadores::mediaMovelExponencial(historico, -2).isEmpty());
}

// ---------------------------------------------------------------- RSI

void TesteIndicadores::indiceForcaRelativaSerieSoDeAltasVaiACem()
{
    QVector<double> fechamentos;
    for (int i = 0; i < 20; ++i)
    {
        fechamentos.append(100.0 + i);
    }

    const QVector<Indicador> rsi = CalculadoraIndicadores::indiceForcaRelativa(serie(fechamentos), 14);
    QVERIFY(!rsi.isEmpty());
    QCOMPARE(rsi.first().nome, QStringLiteral("RSI 14"));
    // Sem nenhuma perda na janela, o RSI satura no maximo.
    QVERIFY(qAbs(rsi.last().valor - 100.0) < TOLERANCIA);
}

void TesteIndicadores::indiceForcaRelativaSerieSoDeQuedasVaiAZero()
{
    QVector<double> fechamentos;
    for (int i = 0; i < 20; ++i)
    {
        fechamentos.append(100.0 - i);
    }

    const QVector<Indicador> rsi = CalculadoraIndicadores::indiceForcaRelativa(serie(fechamentos), 14);
    QVERIFY(!rsi.isEmpty());
    // Sem nenhum ganho na janela, o RSI satura no minimo.
    QVERIFY(qAbs(rsi.last().valor - 0.0) < TOLERANCIA);
}

void TesteIndicadores::indiceForcaRelativaSerieAlternadaFicaNaFaixaNeutra()
{
    QVector<double> fechamentos;
    for (int i = 0; i < 20; ++i)
    {
        fechamentos.append(i % 2 == 0 ? 100.0 : 101.0);
    }

    const QVector<Indicador> rsi = CalculadoraIndicadores::indiceForcaRelativa(serie(fechamentos), 14);
    QVERIFY(!rsi.isEmpty());
    // Ganhos e perdas iguais na primeira janela produzem exatamente 50.
    QVERIFY(qAbs(rsi.first().valor - 50.0) < TOLERANCIA);
    for (const Indicador& ponto : rsi)
    {
        QVERIFY(ponto.valor > 30.0);
        QVERIFY(ponto.valor < 70.0);
    }
}

void TesteIndicadores::indiceForcaRelativaExigeUmaCotacaoAlemDoPeriodo()
{
    QVector<double> quatorze;
    for (int i = 0; i < 14; ++i)
    {
        quatorze.append(100.0 + i);
    }
    // Com exatamente 14 fechamentos ha apenas 13 variacoes: serie vazia.
    QVERIFY(CalculadoraIndicadores::indiceForcaRelativa(serie(quatorze), 14).isEmpty());

    QVector<double> quinze = quatorze;
    quinze.append(115.0);
    const QVector<Indicador> rsi = CalculadoraIndicadores::indiceForcaRelativa(serie(quinze), 14);
    QCOMPARE(static_cast<int>(rsi.size()), 1);
}

void TesteIndicadores::indiceForcaRelativaComPeriodoInvalido()
{
    const QVector<Cotacao> historico = serie({10.0, 11.0, 12.0, 13.0, 14.0});
    QVERIFY(CalculadoraIndicadores::indiceForcaRelativa(historico, 0).isEmpty());
    QVERIFY(CalculadoraIndicadores::indiceForcaRelativa(historico, -14).isEmpty());
}

void TesteIndicadores::indiceForcaRelativaComHistoricoVazio()
{
    const QVector<Cotacao> vazio;
    QVERIFY(CalculadoraIndicadores::indiceForcaRelativa(vazio, 14).isEmpty());
    QVERIFY(CalculadoraIndicadores::indiceForcaRelativa(vazio, 0).isEmpty());
}

// ---------------------------------------------------------------- variacao

void TesteIndicadores::variacaoPercentualEntreDoisPregoes()
{
    const QVector<Cotacao> historico = serie({100.0, 110.0});
    const std::optional<double> variacao = CalculadoraIndicadores::variacaoPercentual(historico, 1);

    QVERIFY(variacao.has_value());
    QVERIFY(qAbs(variacao.value() - 10.0) < TOLERANCIA);
}

void TesteIndicadores::variacaoPercentualLimitadaPeloTamanhoDaSerie()
{
    // Pedindo 90 pregoes em uma serie de 3, o calculo usa o primeiro fechamento.
    const QVector<Cotacao> historico = serie({100.0, 105.0, 120.0});
    const std::optional<double> variacao = CalculadoraIndicadores::variacaoPercentual(historico, 90);

    QVERIFY(variacao.has_value());
    QVERIFY(qAbs(variacao.value() - 20.0) < TOLERANCIA);
}

void TesteIndicadores::variacaoPercentualNegativa()
{
    const QVector<Cotacao> historico = serie({50.0, 40.0});
    const std::optional<double> variacao = CalculadoraIndicadores::variacaoPercentual(historico, 1);

    QVERIFY(variacao.has_value());
    QVERIFY(qAbs(variacao.value() + 20.0) < TOLERANCIA);
}

void TesteIndicadores::variacaoPercentualComEntradasInvalidas()
{
    const QVector<Cotacao> vazio;
    QVERIFY(!CalculadoraIndicadores::variacaoPercentual(vazio, 5).has_value());

    const QVector<Cotacao> umaSo = serie({100.0});
    QVERIFY(!CalculadoraIndicadores::variacaoPercentual(umaSo, 5).has_value());

    const QVector<Cotacao> historico = serie({100.0, 110.0});
    QVERIFY(!CalculadoraIndicadores::variacaoPercentual(historico, 0).has_value());
    QVERIFY(!CalculadoraIndicadores::variacaoPercentual(historico, -3).has_value());

    // Preco inicial zerado impede a divisao.
    const QVector<Cotacao> comZero = serie({0.0, 10.0});
    QVERIFY(!CalculadoraIndicadores::variacaoPercentual(comZero, 1).has_value());
}

// ---------------------------------------------------------------- volatilidade

void TesteIndicadores::volatilidadeAnualizadaZeradaComRetornosConstantes()
{
    // Retornos diarios identicos (+10%) => desvio padrao zero.
    const QVector<Cotacao> historico = serie({100.0, 110.0, 121.0, 133.1, 146.41});
    const std::optional<double> volatilidade = CalculadoraIndicadores::volatilidadeAnualizada(historico, 4);

    QVERIFY(volatilidade.has_value());
    QVERIFY(qAbs(volatilidade.value()) < TOLERANCIA);
}

void TesteIndicadores::volatilidadeAnualizadaPositivaComOscilacao()
{
    const QVector<Cotacao> historico = serie({100.0, 104.0, 99.0, 108.0, 96.0, 110.0});
    const std::optional<double> volatilidade = CalculadoraIndicadores::volatilidadeAnualizada(historico, 5);

    QVERIFY(volatilidade.has_value());
    QVERIFY(volatilidade.value() > 0.0);
}

void TesteIndicadores::volatilidadeAnualizadaComEntradasInvalidas()
{
    const QVector<Cotacao> vazio;
    QVERIFY(!CalculadoraIndicadores::volatilidadeAnualizada(vazio, 20).has_value());

    const QVector<Cotacao> curto = serie({100.0, 101.0});
    QVERIFY(!CalculadoraIndicadores::volatilidadeAnualizada(curto, 20).has_value());

    const QVector<Cotacao> historico = serie({100.0, 101.0, 102.0, 103.0});
    QVERIFY(!CalculadoraIndicadores::volatilidadeAnualizada(historico, 1).has_value());
    QVERIFY(!CalculadoraIndicadores::volatilidadeAnualizada(historico, 0).has_value());
    QVERIFY(!CalculadoraIndicadores::volatilidadeAnualizada(historico, -10).has_value());
}

// ---------------------------------------------------------------- retorno e auxiliares

void TesteIndicadores::retornoAcumuladoPositivoENegativo()
{
    const std::optional<double> alta = CalculadoraIndicadores::retornoAcumulado(serie({50.0, 60.0, 75.0}));
    QVERIFY(alta.has_value());
    QVERIFY(qAbs(alta.value() - 50.0) < TOLERANCIA);

    const std::optional<double> baixa = CalculadoraIndicadores::retornoAcumulado(serie({100.0, 95.0, 90.0}));
    QVERIFY(baixa.has_value());
    QVERIFY(qAbs(baixa.value() + 10.0) < TOLERANCIA);
}

void TesteIndicadores::retornoAcumuladoComHistoricoInsuficiente()
{
    const QVector<Cotacao> vazio;
    QVERIFY(!CalculadoraIndicadores::retornoAcumulado(vazio).has_value());
    QVERIFY(!CalculadoraIndicadores::retornoAcumulado(serie({100.0})).has_value());
    // Primeiro fechamento zerado impede a divisao.
    QVERIFY(!CalculadoraIndicadores::retornoAcumulado(serie({0.0, 10.0})).has_value());
}

void TesteIndicadores::ultimoFechamentoEUltimoValor()
{
    const QVector<Cotacao> historico = serie({10.0, 11.0, 12.0});
    const std::optional<double> fechamento = CalculadoraIndicadores::ultimoFechamento(historico);
    QVERIFY(fechamento.has_value());
    QVERIFY(qAbs(fechamento.value() - 12.0) < TOLERANCIA);

    const QVector<Indicador> media = CalculadoraIndicadores::mediaMovelSimples(historico, 3);
    const std::optional<double> ultimo = CalculadoraIndicadores::ultimoValor(media);
    QVERIFY(ultimo.has_value());
    QVERIFY(qAbs(ultimo.value() - 11.0) < TOLERANCIA);

    const QVector<Cotacao> vazio;
    QVERIFY(!CalculadoraIndicadores::ultimoFechamento(vazio).has_value());
    const QVector<Indicador> serieVazia;
    QVERIFY(!CalculadoraIndicadores::ultimoValor(serieVazia).has_value());
}

void TesteIndicadores::ultimasCotacoesRecortaOFinalDaSerie()
{
    const QVector<Cotacao> historico = serie({10.0, 11.0, 12.0, 13.0, 14.0});

    const QVector<Cotacao> recorte = CalculadoraIndicadores::ultimasCotacoes(historico, 2);
    QCOMPARE(static_cast<int>(recorte.size()), 2);
    QVERIFY(qAbs(recorte.first().fechamento() - 13.0) < TOLERANCIA);
    QVERIFY(qAbs(recorte.last().fechamento() - 14.0) < TOLERANCIA);

    // Quantidade maior que a serie devolve a serie inteira.
    const QVector<Cotacao> tudo = CalculadoraIndicadores::ultimasCotacoes(historico, 50);
    QCOMPARE(static_cast<int>(tudo.size()), static_cast<int>(historico.size()));

    QVERIFY(CalculadoraIndicadores::ultimasCotacoes(historico, 0).isEmpty());
    QVERIFY(CalculadoraIndicadores::ultimasCotacoes(historico, -4).isEmpty());

    const QVector<Cotacao> vazio;
    QVERIFY(CalculadoraIndicadores::ultimasCotacoes(vazio, 10).isEmpty());
}

void TesteIndicadores::historicoVazioNaoQuebraNenhumMetodo()
{
    // Varredura final: nenhum metodo pode acessar posicao invalida com serie vazia.
    const QVector<Cotacao> vazio;

    QVERIFY(CalculadoraIndicadores::mediaMovelSimples(vazio, 21).isEmpty());
    QVERIFY(CalculadoraIndicadores::mediaMovelExponencial(vazio, 21).isEmpty());
    QVERIFY(CalculadoraIndicadores::indiceForcaRelativa(vazio, 14).isEmpty());
    QVERIFY(!CalculadoraIndicadores::variacaoPercentual(vazio, 30).has_value());
    QVERIFY(!CalculadoraIndicadores::volatilidadeAnualizada(vazio, 30).has_value());
    QVERIFY(!CalculadoraIndicadores::retornoAcumulado(vazio).has_value());
    QVERIFY(!CalculadoraIndicadores::ultimoFechamento(vazio).has_value());
    QVERIFY(CalculadoraIndicadores::ultimasCotacoes(vazio, 5).isEmpty());
}

}

QTEST_APPLESS_MAIN(analisador::TesteIndicadores)
