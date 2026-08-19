#include "servicos/servicorecomendacao.h"

#include <memory>

#include "analise/motoranalise.h"
#include "dominio/ativo.h"
#include "dominio/cotacao.h"
#include "dominio/regraconfigurada.h"

namespace analisador
{

const int ServicoRecomendacao::COTACOES_MINIMAS = 2;

ServicoRecomendacao::ServicoRecomendacao(RepositorioRecomendacao& repositorioRecomendacao,
                                         RepositorioCotacao& repositorioCotacao,
                                         RepositorioAtivo& repositorioAtivo,
                                         RepositorioRegra& repositorioRegra)
    : m_repositorioRecomendacao(repositorioRecomendacao)
    , m_repositorioCotacao(repositorioCotacao)
    , m_repositorioAtivo(repositorioAtivo)
    , m_repositorioRegra(repositorioRegra)
{
}

std::optional<Recomendacao> ServicoRecomendacao::gerarParaAtivo(qint64 ativoId)
{
    m_ultimoErro.clear();

    const std::unique_ptr<Ativo> ativo = m_repositorioAtivo.buscarPorId(ativoId);
    if (!ativo)
    {
        m_ultimoErro = QString::fromUtf8("Ativo não encontrado no cadastro.");
        return std::nullopt;
    }

    const QVector<Cotacao> historico = m_repositorioCotacao.listarPorAtivo(ativoId);
    const QVector<RegraConfigurada> regras = m_repositorioRegra.listarAtivas();
    const std::unique_ptr<MotorAnalise> motor = MotorAnalise::comRegrasPadrao(regras);

    Recomendacao recomendacao = motor->analisar(*ativo, historico);

    // A recomendacao anterior deixa de valer no momento em que uma nova e publicada.
    m_repositorioRecomendacao.invalidarVigentesDoAtivo(ativoId);

    if (!recomendacao.marcarComoVigente())
    {
        m_ultimoErro = QString::fromUtf8("Não foi possível publicar a recomendação gerada.");
        return std::nullopt;
    }
    if (!m_repositorioRecomendacao.salvar(recomendacao))
    {
        m_ultimoErro = m_repositorioRecomendacao.ultimoErro();
        return std::nullopt;
    }
    return recomendacao;
}

QVector<Recomendacao> ServicoRecomendacao::gerarParaTodos()
{
    m_ultimoErro.clear();
    QVector<Recomendacao> publicadas;

    const std::vector<std::unique_ptr<Ativo>> ativos = m_repositorioAtivo.listarTodos();
    for (const std::unique_ptr<Ativo>& ativo : ativos)
    {
        if (!ativo)
        {
            continue;
        }
        const std::optional<Recomendacao> gerada = gerarParaAtivo(ativo->id());
        if (gerada.has_value())
        {
            publicadas.append(gerada.value());
        }
    }
    return publicadas;
}

QVector<Recomendacao> ServicoRecomendacao::listarVigentes() const
{
    return m_repositorioRecomendacao.listarVigentes();
}

std::optional<Recomendacao> ServicoRecomendacao::vigenteDoAtivo(qint64 ativoId) const
{
    return m_repositorioRecomendacao.buscarVigenteDoAtivo(ativoId);
}

QVector<Recomendacao> ServicoRecomendacao::detalharPorRegra(qint64 ativoId) const
{
    const std::unique_ptr<Ativo> ativo = m_repositorioAtivo.buscarPorId(ativoId);
    if (!ativo)
    {
        return QVector<Recomendacao>();
    }
    const QVector<Cotacao> historico = m_repositorioCotacao.listarPorAtivo(ativoId);
    const QVector<RegraConfigurada> regras = m_repositorioRegra.listarAtivas();
    const std::unique_ptr<MotorAnalise> motor = MotorAnalise::comRegrasPadrao(regras);
    return motor->avaliarRegrasIndividualmente(*ativo, historico);
}

QString ServicoRecomendacao::ultimoErro() const
{
    return m_ultimoErro;
}

}
