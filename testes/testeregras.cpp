// Testes unitarios das estrategias de analise (padrao Strategy) e do MotorAnalise.
//
// Para cada uma das quatro regras ha um cenario que forca Compra, um que forca
// Venda (ou a pontuacao negativa maxima que a regra consegue emitir) e um em que
// a regra se abstem devolvendo std::nullopt. O motor e exercitado na agregacao
// das pontuacoes, no respeito ao limiar de decisao e no caso em que nenhuma
// regra opina.
//
// As regras fundamentalistas sao testadas com Acao e FundoImobiliario concretos,
// exercitando o dynamic_cast que faz o polimorfismo em tempo de execucao.
//
// Comparacoes de ponto flutuante usam tolerancia explicita; nunca operador ==.

#include <memory>
#include <optional>

#include <QDate>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QTest>
#include <QVector>

#include "analise/motoranalise.h"
#include "analise/regraanalise.h"
#include "analise/regracruzamentomedias.h"
#include "analise/regradividendyield.h"
#include "analise/regraprecolucro.h"
#include "analise/regrarsi.h"
#include "dominio/acao.h"
#include "dominio/cotacao.h"
#include "dominio/fundoimobiliario.h"
#include "dominio/recomendacao.h"
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

// Acao de demonstracao com os fundamentos informados.
static Acao acaoDeTeste(double precoLucro, double dividendYield)
{
    return Acao(1,
                QStringLiteral("PETR4"),
                QStringLiteral("Petroleo Brasileiro S.A."),
                QStringLiteral("Petroleo, Gas e Biocombustiveis"),
                precoLucro,
                dividendYield,
                495000000000.0);
}

// Fundo imobiliario de demonstracao com os fundamentos informados.
static FundoImobiliario fundoDeTeste(double dividendYield, double valorPatrimonial)
{
    return FundoImobiliario(2,
                            QStringLiteral("HGLG11"),
                            QStringLiteral("CSHG Logistica FII"),
                            QStringLiteral("Fundos Imobiliarios"),
                            dividendYield,
                            valorPatrimonial,
                            QStringLiteral("Logistica"));
}

class TesteRegras : public QObject
{
    Q_OBJECT

private slots:
    // Cruzamento de medias moveis
    void cruzamentoGeraCompraNoCruzamentoParaCima();
    void cruzamentoGeraVendaNoCruzamentoParaBaixo();
    void cruzamentoIndicaTendenciaSemCruzamentoRecente();
    void cruzamentoSeAbstemComHistoricoCurto();
    void cruzamentoUsaPeriodosPadraoComParametrosInvalidos();

    // Indice de forca relativa
    void rsiGeraCompraEmSerieSobrevendida();
    void rsiGeraVendaEmSerieSobrecomprada();
    void rsiSeAbstemNaFaixaNeutra();
    void rsiSeAbstemComHistoricoCurto();

    // Dividend yield
    void dividendYieldGeraCompraParaAcaoExcelentePagadora();
    void dividendYieldGeraCompraParaFundoImobiliario();
    void dividendYieldPontuaNegativoAbaixoDoMinimo();
    void dividendYieldSeAbstemSemFundamentoCadastrado();

    // Preco sobre lucro
    void precoLucroGeraCompraComMultiploBaixo();
    void precoLucroGeraVendaComMultiploElevado();
    void precoLucroFicaNeutroNaFaixaIntermediaria();
    void precoLucroSeAbstemParaFundoImobiliario();
    void precoLucroSeAbstemSemLucroInformado();

    // Motor de analise
    void motorAgregaPontuacoesPelaMedia();
    void motorConsolidaVendaComFundamentosFracos();
    void motorRespeitaLimiarDeDecisao();
    void motorDevolveNeutroQuandoNenhumaRegraOpina();
    void motorListaPareceresIndividuais();
    void motorMontaConfiguracaoPadrao();
    void motorIgnoraRegrasInativasEDesconhecidas();
    void motorAnalisaSerieTecnicaComoCompra();
};

// ---------------------------------------------------------------- cruzamento

void TesteRegras::cruzamentoGeraCompraNoCruzamentoParaCima()
{
    // Com medias de 3 e 5 dias: MMS3 vem abaixo da MMS5 e a ultrapassa no
    // ultimo pregao (14,6667 -> 17,6667 contra 15,2 -> 16,4).
    RegraCruzamentoMedias regra;
    regra.configurar(3.0, 5.0);
    QCOMPARE(regra.periodoCurto(), 3);
    QCOMPARE(regra.periodoLongo(), 5);

    const Acao acao = acaoDeTeste(4.8, 12.4);
    const QVector<Cotacao> historico =
        serie({20.0, 19.0, 18.0, 17.0, 16.0, 15.0, 14.0, 15.0, 16.0, 22.0});

    const std::optional<Recomendacao> parecer = regra.avaliar(acao, historico);
    QVERIFY(parecer.has_value());
    QVERIFY(qAbs(parecer.value().pontuacao() - 0.80) < TOLERANCIA);
    QVERIFY(parecer.value().tipo() == TipoRecomendacao::Compra);
    QVERIFY(!parecer.value().justificativa().isEmpty());
    QCOMPARE(parecer.value().regraAplicada(), regra.nome());
    QCOMPARE(parecer.value().ativoId(), acao.id());
}

void TesteRegras::cruzamentoGeraVendaNoCruzamentoParaBaixo()
{
    // MMS3 vem acima da MMS5 e cai abaixo dela no ultimo pregao.
    RegraCruzamentoMedias regra;
    regra.configurar(3.0, 5.0);

    const Acao acao = acaoDeTeste(9.1, 6.3);
    const QVector<Cotacao> historico =
        serie({10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 15.5, 14.5, 8.0});

    const std::optional<Recomendacao> parecer = regra.avaliar(acao, historico);
    QVERIFY(parecer.has_value());
    QVERIFY(qAbs(parecer.value().pontuacao() + 0.80) < TOLERANCIA);
    QVERIFY(parecer.value().tipo() == TipoRecomendacao::Venda);
    QVERIFY(!parecer.value().justificativa().isEmpty());
}

void TesteRegras::cruzamentoIndicaTendenciaSemCruzamentoRecente()
{
    RegraCruzamentoMedias regra;
    regra.configurar(3.0, 5.0);
    const Acao acao = acaoDeTeste(9.1, 6.3);

    const QVector<Cotacao> alta =
        serie({10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 17.0, 18.0, 19.0});
    const std::optional<Recomendacao> parecerAlta = regra.avaliar(acao, alta);
    QVERIFY(parecerAlta.has_value());
    QVERIFY(qAbs(parecerAlta.value().pontuacao() - 0.35) < TOLERANCIA);
    QVERIFY(parecerAlta.value().tipo() == TipoRecomendacao::Compra);

    const QVector<Cotacao> baixa =
        serie({19.0, 18.0, 17.0, 16.0, 15.0, 14.0, 13.0, 12.0, 11.0, 10.0});
    const std::optional<Recomendacao> parecerBaixa = regra.avaliar(acao, baixa);
    QVERIFY(parecerBaixa.has_value());
    QVERIFY(qAbs(parecerBaixa.value().pontuacao() + 0.35) < TOLERANCIA);
    QVERIFY(parecerBaixa.value().tipo() == TipoRecomendacao::Venda);
}

void TesteRegras::cruzamentoSeAbstemComHistoricoCurto()
{
    RegraCruzamentoMedias regra;
    regra.configurar(3.0, 5.0);
    const Acao acao = acaoDeTeste(4.8, 12.4);

    // Faltam pregoes para dois pontos da media longa: a regra nao opina.
    const QVector<Cotacao> curto = serie({10.0, 11.0, 12.0, 13.0, 14.0});
    QVERIFY(!regra.avaliar(acao, curto).has_value());

    const QVector<Cotacao> vazio;
    QVERIFY(!regra.avaliar(acao, vazio).has_value());
}

void TesteRegras::cruzamentoUsaPeriodosPadraoComParametrosInvalidos()
{
    RegraCruzamentoMedias regra;
    regra.configurar(0.0, 0.0);
    QCOMPARE(regra.periodoCurto(), 9);
    QCOMPARE(regra.periodoLongo(), 21);
    QVERIFY(!regra.nome().isEmpty());
    QVERIFY(!regra.descricao().isEmpty());
}

// ---------------------------------------------------------------- RSI

void TesteRegras::rsiGeraCompraEmSerieSobrevendida()
{
    // Serie so de quedas leva o RSI a zero: intensidade maxima de compra (0,90).
    QVector<double> fechamentos;
    for (int i = 0; i < 20; ++i)
    {
        fechamentos.append(100.0 - i);
    }

    RegraRsi regra;
    regra.configurar(30.0, 70.0);
    const Acao acao = acaoDeTeste(9.1, 6.3);

    const std::optional<Recomendacao> parecer = regra.avaliar(acao, serie(fechamentos));
    QVERIFY(parecer.has_value());
    QVERIFY(qAbs(parecer.value().pontuacao() - 0.90) < TOLERANCIA);
    QVERIFY(parecer.value().tipo() == TipoRecomendacao::Compra);
    QVERIFY(!parecer.value().justificativa().isEmpty());
}

void TesteRegras::rsiGeraVendaEmSerieSobrecomprada()
{
    // Serie so de altas leva o RSI a cem: intensidade maxima de venda (-0,90).
    QVector<double> fechamentos;
    for (int i = 0; i < 20; ++i)
    {
        fechamentos.append(100.0 + i);
    }

    RegraRsi regra;
    regra.configurar(30.0, 70.0);
    const Acao acao = acaoDeTeste(9.1, 6.3);

    const std::optional<Recomendacao> parecer = regra.avaliar(acao, serie(fechamentos));
    QVERIFY(parecer.has_value());
    QVERIFY(qAbs(parecer.value().pontuacao() + 0.90) < TOLERANCIA);
    QVERIFY(parecer.value().tipo() == TipoRecomendacao::Venda);
}

void TesteRegras::rsiSeAbstemNaFaixaNeutra()
{
    // Ganhos e perdas equilibrados mantem o RSI entre 30 e 70.
    QVector<double> fechamentos;
    for (int i = 0; i < 20; ++i)
    {
        fechamentos.append(i % 2 == 0 ? 100.0 : 101.0);
    }

    RegraRsi regra;
    const Acao acao = acaoDeTeste(9.1, 6.3);
    QVERIFY(!regra.avaliar(acao, serie(fechamentos)).has_value());
}

void TesteRegras::rsiSeAbstemComHistoricoCurto()
{
    RegraRsi regra;
    const Acao acao = acaoDeTeste(9.1, 6.3);

    const QVector<Cotacao> curto = serie({10.0, 11.0, 12.0});
    QVERIFY(!regra.avaliar(acao, curto).has_value());

    const QVector<Cotacao> vazio;
    QVERIFY(!regra.avaliar(acao, vazio).has_value());
}

// ---------------------------------------------------------------- dividend yield

void TesteRegras::dividendYieldGeraCompraParaAcaoExcelentePagadora()
{
    RegraDividendYield regra;
    regra.configurar(6.0, 10.0);
    QVERIFY(qAbs(regra.dividendYieldMinimo() - 6.0) < TOLERANCIA);
    QVERIFY(qAbs(regra.dividendYieldExcelente() - 10.0) < TOLERANCIA);

    const Acao acao = acaoDeTeste(4.8, 12.4);
    const std::optional<Recomendacao> parecer = regra.avaliar(acao, serie({35.0, 36.0}));

    QVERIFY(parecer.has_value());
    QVERIFY(qAbs(parecer.value().pontuacao() - 0.75) < TOLERANCIA);
    QVERIFY(parecer.value().tipo() == TipoRecomendacao::Compra);
    QVERIFY(!parecer.value().justificativa().isEmpty());
}

void TesteRegras::dividendYieldGeraCompraParaFundoImobiliario()
{
    // Exercita o dynamic_cast para FundoImobiliario.
    RegraDividendYield regra;
    regra.configurar(6.0, 10.0);

    const FundoImobiliario fundo = fundoDeTeste(9.2, 168.40);
    const std::optional<Recomendacao> parecer = regra.avaliar(fundo, serie({160.0, 161.0}));

    QVERIFY(parecer.has_value());
    QVERIFY(qAbs(parecer.value().pontuacao() - 0.40) < TOLERANCIA);
    QVERIFY(parecer.value().tipo() == TipoRecomendacao::Compra);
    QCOMPARE(parecer.value().ativoId(), fundo.id());
}

void TesteRegras::dividendYieldPontuaNegativoAbaixoDoMinimo()
{
    // Abaixo do minimo a regra pontua -0,15: sinal negativo que, sozinho, ainda
    // nao alcanca o limiar de venda (-0,25).
    RegraDividendYield regra;
    regra.configurar(6.0, 10.0);

    const Acao acao = acaoDeTeste(9.1, 2.0);
    const std::optional<Recomendacao> parecer = regra.avaliar(acao, serie({30.0, 31.0}));

    QVERIFY(parecer.has_value());
    QVERIFY(qAbs(parecer.value().pontuacao() + 0.15) < TOLERANCIA);
    QVERIFY(parecer.value().pontuacao() < 0.0);
    QVERIFY(qAbs(parecer.value().pontuacao()) < Recomendacao::LIMIAR_DECISAO);
    QVERIFY(parecer.value().tipo() == TipoRecomendacao::Neutro);
}

void TesteRegras::dividendYieldSeAbstemSemFundamentoCadastrado()
{
    RegraDividendYield regra;
    const Acao semProventos = acaoDeTeste(9.1, 0.0);
    QVERIFY(!regra.avaliar(semProventos, serie({30.0, 31.0})).has_value());

    const FundoImobiliario fundoSemProventos = fundoDeTeste(0.0, 100.0);
    const QVector<Cotacao> vazio;
    QVERIFY(!regra.avaliar(fundoSemProventos, vazio).has_value());
}

// ---------------------------------------------------------------- preco sobre lucro

void TesteRegras::precoLucroGeraCompraComMultiploBaixo()
{
    RegraPrecoLucro regra;
    regra.configurar(8.0, 20.0);
    QVERIFY(qAbs(regra.precoLucroAtrativo() - 8.0) < TOLERANCIA);
    QVERIFY(qAbs(regra.precoLucroElevado() - 20.0) < TOLERANCIA);

    const Acao acao = acaoDeTeste(4.8, 12.4);
    const std::optional<Recomendacao> parecer = regra.avaliar(acao, serie({35.0, 36.0}));

    QVERIFY(parecer.has_value());
    QVERIFY(qAbs(parecer.value().pontuacao() - 0.60) < TOLERANCIA);
    QVERIFY(parecer.value().tipo() == TipoRecomendacao::Compra);
    QVERIFY(!parecer.value().justificativa().isEmpty());
}

void TesteRegras::precoLucroGeraVendaComMultiploElevado()
{
    RegraPrecoLucro regra;
    regra.configurar(8.0, 20.0);

    const Acao acao = acaoDeTeste(25.0, 1.5);
    const std::optional<Recomendacao> parecer = regra.avaliar(acao, serie({35.0, 36.0}));

    QVERIFY(parecer.has_value());
    QVERIFY(qAbs(parecer.value().pontuacao() + 0.60) < TOLERANCIA);
    QVERIFY(parecer.value().tipo() == TipoRecomendacao::Venda);
}

void TesteRegras::precoLucroFicaNeutroNaFaixaIntermediaria()
{
    // P/L 14 esta no meio da faixa 8-20: pontuacao 0,20 - 0,5 * 0,40 = 0,00.
    RegraPrecoLucro regra;
    regra.configurar(8.0, 20.0);

    const Acao acao = acaoDeTeste(14.0, 5.0);
    const std::optional<Recomendacao> parecer = regra.avaliar(acao, serie({35.0, 36.0}));

    QVERIFY(parecer.has_value());
    QVERIFY(qAbs(parecer.value().pontuacao()) < TOLERANCIA);
    QVERIFY(parecer.value().tipo() == TipoRecomendacao::Neutro);
}

void TesteRegras::precoLucroSeAbstemParaFundoImobiliario()
{
    // O dynamic_cast para Acao falha: criterio inaplicavel a um FII.
    RegraPrecoLucro regra;
    const FundoImobiliario fundo = fundoDeTeste(9.2, 168.40);
    QVERIFY(!regra.avaliar(fundo, serie({160.0, 161.0})).has_value());
}

void TesteRegras::precoLucroSeAbstemSemLucroInformado()
{
    RegraPrecoLucro regra;
    const Acao acao = acaoDeTeste(0.0, 6.5);
    QVERIFY(!regra.avaliar(acao, serie({30.0, 31.0})).has_value());
}

// ---------------------------------------------------------------- motor

void TesteRegras::motorAgregaPontuacoesPelaMedia()
{
    // P/L 4,8 -> 0,60 e DY 12,4 -> 0,75. Media = 0,675 (Compra).
    MotorAnalise motor;
    motor.registrarRegra(std::make_unique<RegraPrecoLucro>());
    motor.registrarRegra(std::make_unique<RegraDividendYield>());
    QCOMPARE(motor.quantidadeDeRegras(), 2);

    const Acao acao = acaoDeTeste(4.8, 12.4);
    // Historico curto de proposito: so as regras fundamentalistas opinam.
    const Recomendacao consolidada = motor.analisar(acao, serie({35.0, 36.0}));

    QVERIFY(qAbs(consolidada.pontuacao() - 0.675) < TOLERANCIA);
    QVERIFY(consolidada.tipo() == TipoRecomendacao::Compra);
    QVERIFY(!consolidada.justificativa().isEmpty());
    QVERIFY(consolidada.regraAplicada().contains(QStringLiteral("Preco sobre Lucro")));
    QVERIFY(consolidada.regraAplicada().contains(QStringLiteral("Dividend Yield")));
    QCOMPARE(consolidada.ativoId(), acao.id());
    QVERIFY(consolidada.estado() == EstadoRecomendacao::Gerada);
}

void TesteRegras::motorConsolidaVendaComFundamentosFracos()
{
    // P/L 25 -> -0,60 e DY 2,0 -> -0,15. Media = -0,375 (Venda).
    MotorAnalise motor;
    motor.registrarRegra(std::make_unique<RegraPrecoLucro>());
    motor.registrarRegra(std::make_unique<RegraDividendYield>());

    const Acao acao = acaoDeTeste(25.0, 2.0);
    const Recomendacao consolidada = motor.analisar(acao, serie({30.0, 29.0}));

    QVERIFY(qAbs(consolidada.pontuacao() + 0.375) < TOLERANCIA);
    QVERIFY(consolidada.pontuacao() <= -Recomendacao::LIMIAR_DECISAO);
    QVERIFY(consolidada.tipo() == TipoRecomendacao::Venda);
    QVERIFY(!consolidada.justificativa().isEmpty());
}

void TesteRegras::motorRespeitaLimiarDeDecisao()
{
    MotorAnalise motor;
    motor.registrarRegra(std::make_unique<RegraPrecoLucro>());
    motor.registrarRegra(std::make_unique<RegraDividendYield>());

    // P/L 14 -> 0,00 e DY 8,0 -> 0,40. Media = 0,20, abaixo do limiar de 0,25.
    const Acao acao = acaoDeTeste(14.0, 8.0);
    const Recomendacao consolidada = motor.analisar(acao, serie({35.0, 36.0}));

    QVERIFY(qAbs(consolidada.pontuacao() - 0.20) < TOLERANCIA);
    QVERIFY(consolidada.pontuacao() > 0.0);
    QVERIFY(consolidada.pontuacao() < Recomendacao::LIMIAR_DECISAO);
    QVERIFY(consolidada.tipo() == TipoRecomendacao::Neutro);

    // Exatamente no limiar a classificacao ja e Compra; no simetrico, Venda.
    QVERIFY(Recomendacao::tipoPorPontuacao(Recomendacao::LIMIAR_DECISAO) == TipoRecomendacao::Compra);
    QVERIFY(Recomendacao::tipoPorPontuacao(-Recomendacao::LIMIAR_DECISAO) == TipoRecomendacao::Venda);
    QVERIFY(Recomendacao::tipoPorPontuacao(0.0) == TipoRecomendacao::Neutro);
}

void TesteRegras::motorDevolveNeutroQuandoNenhumaRegraOpina()
{
    // Um FII avaliado apenas pela regra de P/L: nenhuma regra consegue opinar.
    MotorAnalise motor;
    motor.registrarRegra(std::make_unique<RegraPrecoLucro>());

    const FundoImobiliario fundo = fundoDeTeste(9.2, 168.40);
    const Recomendacao consolidada = motor.analisar(fundo, serie({160.0, 161.0}));

    QVERIFY(qAbs(consolidada.pontuacao()) < TOLERANCIA);
    QVERIFY(consolidada.tipo() == TipoRecomendacao::Neutro);
    QVERIFY(!consolidada.justificativa().isEmpty());
    QVERIFY(!consolidada.regraAplicada().isEmpty());
    QVERIFY(motor.avaliarRegrasIndividualmente(fundo, serie({160.0, 161.0})).isEmpty());
}

void TesteRegras::motorListaPareceresIndividuais()
{
    MotorAnalise motor;
    motor.registrarRegra(std::make_unique<RegraPrecoLucro>());
    motor.registrarRegra(std::make_unique<RegraDividendYield>());
    motor.registrarRegra(std::make_unique<RegraRsi>());

    const Acao acao = acaoDeTeste(4.8, 12.4);
    // Historico curto: a regra de RSI se abstem e sobram dois pareceres.
    const QVector<Recomendacao> pareceres = motor.avaliarRegrasIndividualmente(acao, serie({35.0, 36.0}));

    QCOMPARE(static_cast<int>(pareceres.size()), 2);
    for (const Recomendacao& parecer : pareceres)
    {
        QVERIFY(!parecer.justificativa().isEmpty());
        QVERIFY(!parecer.regraAplicada().isEmpty());
        QVERIFY(parecer.pontuacao() >= -1.0);
        QVERIFY(parecer.pontuacao() <= 1.0);
    }
}

void TesteRegras::motorMontaConfiguracaoPadrao()
{
    const QVector<RegraConfigurada> padrao = MotorAnalise::configuracaoPadrao();
    QCOMPARE(static_cast<int>(padrao.size()), 4);

    const std::unique_ptr<MotorAnalise> motor = MotorAnalise::comRegrasPadrao(padrao);
    QVERIFY(motor != nullptr);
    QCOMPARE(motor->quantidadeDeRegras(), 4);

    const QStringList nomes = motor->nomesDasRegras();
    QVERIFY(nomes.contains(QStringLiteral("Cruzamento de Medias Moveis")));
    QVERIFY(nomes.contains(QStringLiteral("Indice de Forca Relativa")));
    QVERIFY(nomes.contains(QStringLiteral("Dividend Yield")));
    QVERIFY(nomes.contains(QStringLiteral("Preco sobre Lucro")));

    // Lista vazia cai na configuracao padrao.
    const QVector<RegraConfigurada> vazia;
    const std::unique_ptr<MotorAnalise> motorPadrao = MotorAnalise::comRegrasPadrao(vazia);
    QCOMPARE(motorPadrao->quantidadeDeRegras(), 4);
}

void TesteRegras::motorIgnoraRegrasInativasEDesconhecidas()
{
    QVector<RegraConfigurada> configuracoes;
    configuracoes.append(RegraConfigurada(1, QStringLiteral("Preco sobre Lucro"), true, 8, 20));
    configuracoes.append(RegraConfigurada(2, QStringLiteral("Dividend Yield"), false, 6, 10));
    configuracoes.append(RegraConfigurada(3, QStringLiteral("Regra Inexistente"), true, 1, 2));

    const std::unique_ptr<MotorAnalise> motor = MotorAnalise::comRegrasPadrao(configuracoes);
    QCOMPARE(motor->quantidadeDeRegras(), 1);
    QCOMPARE(motor->nomesDasRegras().first(), QStringLiteral("Preco sobre Lucro"));
    QVERIFY(MotorAnalise::criarRegra(QStringLiteral("Regra Inexistente")) == nullptr);
    QVERIFY(MotorAnalise::criarRegra(QStringLiteral("Dividend Yield")) != nullptr);
}

void TesteRegras::motorAnalisaSerieTecnicaComoCompra()
{
    // Fluxo completo com uma unica regra tecnica: o cruzamento para cima no
    // ultimo pregao leva a recomendacao consolidada a Compra.
    MotorAnalise motor;
    std::unique_ptr<RegraCruzamentoMedias> regra = std::make_unique<RegraCruzamentoMedias>();
    regra->configurar(3.0, 5.0);
    motor.registrarRegra(std::move(regra));

    const Acao acao = acaoDeTeste(4.8, 12.4);
    const QVector<Cotacao> historico =
        serie({20.0, 19.0, 18.0, 17.0, 16.0, 15.0, 14.0, 15.0, 16.0, 22.0});

    const Recomendacao consolidada = motor.analisar(acao, historico);
    QVERIFY(qAbs(consolidada.pontuacao() - 0.80) < TOLERANCIA);
    QVERIFY(consolidada.tipo() == TipoRecomendacao::Compra);
    QVERIFY(!consolidada.justificativa().isEmpty());
    QCOMPARE(consolidada.geradaEm(), historico.last().data());
}

}

QTEST_APPLESS_MAIN(analisador::TesteRegras)
#include "testeregras.moc"
