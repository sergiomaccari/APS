#include "analise/motoranalise.h"

#include <QDate>
#include <QLocale>
#include <QStringList>

#include "analise/regracruzamentomedias.h"
#include "analise/regradividendyield.h"
#include "analise/regraprecolucro.h"
#include "analise/regrarsi.h"

namespace analisador
{

MotorAnalise::MotorAnalise()
{
}

MotorAnalise::~MotorAnalise()
{
}

void MotorAnalise::registrarRegra(std::unique_ptr<RegraAnalise> regra)
{
    if (regra)
    {
        m_regras.push_back(std::move(regra));
    }
}

int MotorAnalise::quantidadeDeRegras() const
{
    return static_cast<int>(m_regras.size());
}

QStringList MotorAnalise::nomesDasRegras() const
{
    QStringList nomes;
    for (const auto& regra : m_regras)
    {
        nomes.append(regra->nome());
    }
    return nomes;
}

QVector<Recomendacao> MotorAnalise::avaliarRegrasIndividualmente(const Ativo& ativo,
                                                                 const QVector<Cotacao>& historico) const
{
    QVector<Recomendacao> pareceres;
    for (const auto& regra : m_regras)
    {
        const std::optional<Recomendacao> parecer = regra->avaliar(ativo, historico);
        if (parecer.has_value())
        {
            pareceres.append(parecer.value());
        }
    }
    return pareceres;
}

Recomendacao MotorAnalise::analisar(const Ativo& ativo, const QVector<Cotacao>& historico) const
{
    const QVector<Recomendacao> pareceres = avaliarRegrasIndividualmente(ativo, historico);

    Recomendacao consolidada;
    consolidada.definirAtivoId(ativo.id());
    const QDate dataReferencia = historico.isEmpty() ? QDate::currentDate() : historico.last().data();
    consolidada.definirGeradaEm(dataReferencia);

    if (pareceres.isEmpty())
    {
        consolidada.definirPontuacao(0.0);
        consolidada.definirTipo(TipoRecomendacao::Neutro);
        consolidada.definirRegraAplicada(QString::fromUtf8("Nenhuma regra aplicável"));
        consolidada.definirJustificativa(QString::fromUtf8(
            "Nenhuma das %1 regras ativas pôde opinar sobre %2. Verifique se o ativo possui "
            "histórico de cotações importado e fundamentos cadastrados.")
            .arg(quantidadeDeRegras())
            .arg(ativo.ticker()));
        return consolidada;
    }

    double soma = 0.0;
    QStringList justificativas;
    QStringList nomesAplicados;
    const QLocale brasil(QLocale::Portuguese, QLocale::Brazil);

    for (int i = 0; i < pareceres.size(); ++i)
    {
        const Recomendacao& parecer = pareceres.at(i);
        soma += parecer.pontuacao();
        nomesAplicados.append(parecer.regraAplicada());
        justificativas.append(QStringLiteral("%1. [%2 | %3 | %4] %5")
                                  .arg(i + 1)
                                  .arg(parecer.regraAplicada(),
                                       paraTexto(parecer.tipo()),
                                       brasil.toString(parecer.pontuacao(), 'f', 2),
                                       parecer.justificativa()));
    }

    const double pontuacaoFinal = soma / pareceres.size();
    consolidada.definirPontuacao(pontuacaoFinal);
    consolidada.definirTipo(Recomendacao::tipoPorPontuacao(pontuacaoFinal));
    consolidada.definirRegraAplicada(nomesAplicados.join(QStringLiteral(", ")));

    const QString cabecalho = QString::fromUtf8(
        "Parecer consolidado de %1 regra(s) para %2 em %3: %4 (pontuação %5, limiar de decisão %6).")
        .arg(pareceres.size())
        .arg(ativo.ticker(),
             dataReferencia.toString(QStringLiteral("dd/MM/yyyy")),
             paraTexto(consolidada.tipo()),
             brasil.toString(pontuacaoFinal, 'f', 2),
             brasil.toString(Recomendacao::LIMIAR_DECISAO, 'f', 2));

    consolidada.definirJustificativa(cabecalho + QStringLiteral("\n\n") + justificativas.join(QStringLiteral("\n")));
    return consolidada;
}

std::unique_ptr<RegraAnalise> MotorAnalise::criarRegra(const QString& nomeRegra)
{
    const QString nome = nomeRegra.trimmed();
    if (nome.compare(QStringLiteral("Cruzamento de Medias Moveis"), Qt::CaseInsensitive) == 0)
    {
        return std::make_unique<RegraCruzamentoMedias>();
    }
    if (nome.compare(QStringLiteral("Indice de Forca Relativa"), Qt::CaseInsensitive) == 0)
    {
        return std::make_unique<RegraRsi>();
    }
    if (nome.compare(QStringLiteral("Dividend Yield"), Qt::CaseInsensitive) == 0)
    {
        return std::make_unique<RegraDividendYield>();
    }
    if (nome.compare(QStringLiteral("Preco sobre Lucro"), Qt::CaseInsensitive) == 0)
    {
        return std::make_unique<RegraPrecoLucro>();
    }
    return nullptr;
}

std::unique_ptr<MotorAnalise> MotorAnalise::comRegrasPadrao(const QVector<RegraConfigurada>& regras)
{
    auto motor = std::make_unique<MotorAnalise>();
    const QVector<RegraConfigurada> aUsar = regras.isEmpty() ? configuracaoPadrao() : regras;

    for (const RegraConfigurada& configuracao : aUsar)
    {
        if (!configuracao.ativa())
        {
            continue;
        }
        std::unique_ptr<RegraAnalise> regra = criarRegra(configuracao.nomeRegra());
        if (!regra)
        {
            continue;
        }
        regra->configurar(configuracao.parametroPrincipal(), configuracao.parametroSecundario());
        motor->registrarRegra(std::move(regra));
    }
    return motor;
}

QVector<RegraConfigurada> MotorAnalise::configuracaoPadrao()
{
    QVector<RegraConfigurada> padrao;
    padrao.append(RegraConfigurada(0, QStringLiteral("Cruzamento de Medias Moveis"), true, 9, 21));
    padrao.append(RegraConfigurada(0, QStringLiteral("Indice de Forca Relativa"), true, 30, 70));
    padrao.append(RegraConfigurada(0, QStringLiteral("Dividend Yield"), true, 6, 10));
    padrao.append(RegraConfigurada(0, QStringLiteral("Preco sobre Lucro"), true, 8, 20));
    return padrao;
}

}
