#include "analise/regraprecolucro.h"

#include <QDate>
#include <QLocale>

#include "dominio/acao.h"

namespace analisador
{

RegraPrecoLucro::RegraPrecoLucro()
{
}

RegraPrecoLucro::~RegraPrecoLucro()
{
}

QString RegraPrecoLucro::nome() const
{
    return QStringLiteral("Preco sobre Lucro");
}

QString RegraPrecoLucro::descricao() const
{
    const QLocale brasil(QLocale::Portuguese, QLocale::Brazil);
    return QString::fromUtf8(
        "Compara o índice Preço/Lucro da ação com as faixas de referência: até %1 é "
        "considerado atrativo; acima de %2, elevado. Não se aplica a fundos imobiliários.")
        .arg(brasil.toString(m_precoLucroAtrativo, 'f', 1),
             brasil.toString(m_precoLucroElevado, 'f', 1));
}

void RegraPrecoLucro::configurar(double parametroPrincipal, double parametroSecundario)
{
    m_precoLucroAtrativo = parametroPrincipal > 0.0 ? parametroPrincipal : 8.0;
    m_precoLucroElevado = parametroSecundario > m_precoLucroAtrativo
        ? parametroSecundario
        : m_precoLucroAtrativo * 2.5;
}

double RegraPrecoLucro::precoLucroAtrativo() const
{
    return m_precoLucroAtrativo;
}

double RegraPrecoLucro::precoLucroElevado() const
{
    return m_precoLucroElevado;
}

std::optional<Recomendacao> RegraPrecoLucro::avaliar(const Ativo& ativo,
                                                     const QVector<Cotacao>& historico) const
{
    const auto* acao = dynamic_cast<const Acao*>(&ativo);
    if (acao == nullptr)
    {
        // Nao e acao: criterio inaplicavel.
        return std::nullopt;
    }
    if (acao->precoLucro() <= 0.0)
    {
        // P/L nao informado ou companhia com prejuizo: sem base para opinar.
        return std::nullopt;
    }

    const double precoLucro = acao->precoLucro();
    const QLocale brasil(QLocale::Portuguese, QLocale::Brazil);
    const QString plTexto = brasil.toString(precoLucro, 'f', 2);

    double pontuacao = 0.0;
    QString justificativa;

    if (precoLucro <= m_precoLucroAtrativo)
    {
        pontuacao = 0.60;
        justificativa = QString::fromUtf8(
            "Preço/Lucro de %1 está dentro da faixa atrativa (até %2): "
            "ação negociada a múltiplo baixo em relação ao lucro.")
            .arg(plTexto, brasil.toString(m_precoLucroAtrativo, 'f', 1));
    }
    else if (precoLucro >= m_precoLucroElevado)
    {
        pontuacao = -0.60;
        justificativa = QString::fromUtf8(
            "Preço/Lucro de %1 supera a faixa considerada elevada (%2): "
            "ação cara em relação ao lucro gerado.")
            .arg(plTexto, brasil.toString(m_precoLucroElevado, 'f', 1));
    }
    else
    {
        // Zona intermediaria: opiniao fraca proporcional a posicao na faixa.
        const double faixa = m_precoLucroElevado - m_precoLucroAtrativo;
        const double posicao = faixa > 0.0 ? (precoLucro - m_precoLucroAtrativo) / faixa : 0.5;
        pontuacao = 0.20 - (posicao * 0.40);
        justificativa = QString::fromUtf8(
            "Preço/Lucro de %1 está na faixa intermediária (entre %2 e %3): "
            "múltiplo dentro do esperado, sem sinal forte.")
            .arg(plTexto,
                 brasil.toString(m_precoLucroAtrativo, 'f', 1),
                 brasil.toString(m_precoLucroElevado, 'f', 1));
    }

    Recomendacao recomendacao;
    recomendacao.definirAtivoId(ativo.id());
    recomendacao.definirPontuacao(pontuacao);
    recomendacao.definirTipo(Recomendacao::tipoPorPontuacao(pontuacao));
    recomendacao.definirJustificativa(justificativa);
    recomendacao.definirRegraAplicada(nome());
    const QDate dataReferencia = historico.isEmpty() ? QDate::currentDate() : historico.last().data();
    recomendacao.definirGeradaEm(dataReferencia);
    return recomendacao;
}

}
