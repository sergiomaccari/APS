#include "servicos/servicocarteira.h"

#include <memory>

#include "dominio/ativo.h"
#include "dominio/cotacao.h"

namespace analisador
{

bool ResumoCarteira::vazia() const
{
    return linhas.isEmpty();
}

ServicoCarteira::ServicoCarteira(RepositorioCarteira& repositorioCarteira,
                                 RepositorioCotacao& repositorioCotacao,
                                 RepositorioAtivo& repositorioAtivo)
    : m_repositorioCarteira(repositorioCarteira)
    , m_repositorioCotacao(repositorioCotacao)
    , m_repositorioAtivo(repositorioAtivo)
{
}

std::optional<Carteira> ServicoCarteira::carteiraDoUsuario(qint64 usuarioId)
{
    m_ultimoErro.clear();
    if (usuarioId <= 0)
    {
        m_ultimoErro = QString::fromUtf8("Usuário inválido.");
        return std::nullopt;
    }

    std::optional<Carteira> existente = m_repositorioCarteira.buscarPrincipalDoUsuario(usuarioId);
    if (existente.has_value())
    {
        return existente;
    }

    // Primeiro acesso do investidor: cria a carteira principal automaticamente.
    Carteira nova;
    nova.definirUsuarioId(usuarioId);
    nova.definirNome(QStringLiteral("Carteira Principal"));
    nova.definirCriadaEm(QDate::currentDate());
    if (!m_repositorioCarteira.salvar(nova))
    {
        m_ultimoErro = m_repositorioCarteira.ultimoErro();
        return std::nullopt;
    }
    return nova;
}

bool ServicoCarteira::comprar(qint64 carteiraId,
                              qint64 ativoId,
                              int quantidade,
                              double preco,
                              const QDate& data)
{
    m_ultimoErro.clear();
    if (carteiraId <= 0 || ativoId <= 0)
    {
        m_ultimoErro = QString::fromUtf8("Selecione a carteira e o ativo.");
        return false;
    }
    if (quantidade <= 0)
    {
        m_ultimoErro = QString::fromUtf8("A quantidade comprada deve ser maior que zero.");
        return false;
    }
    if (preco <= 0.0)
    {
        m_ultimoErro = QString::fromUtf8("O preço de compra deve ser maior que zero.");
        return false;
    }

    std::optional<Posicao> existente = m_repositorioCarteira.buscarPosicao(carteiraId, ativoId);
    if (existente.has_value())
    {
        Posicao posicao = existente.value();
        if (!posicao.incorporarCompra(quantidade, preco))
        {
            m_ultimoErro = QString::fromUtf8("Não foi possível incorporar a compra à posição existente.");
            return false;
        }
        if (!m_repositorioCarteira.atualizarPosicao(posicao))
        {
            m_ultimoErro = m_repositorioCarteira.ultimoErro();
            return false;
        }
        return true;
    }

    Posicao nova;
    nova.definirCarteiraId(carteiraId);
    nova.definirAtivoId(ativoId);
    nova.definirQuantidade(quantidade);
    nova.definirPrecoMedio(preco);
    nova.definirCompradaEm(data.isValid() ? data : QDate::currentDate());
    if (!m_repositorioCarteira.salvarPosicao(nova))
    {
        m_ultimoErro = m_repositorioCarteira.ultimoErro();
        return false;
    }
    return true;
}

bool ServicoCarteira::vender(qint64 carteiraId, qint64 ativoId, int quantidade)
{
    m_ultimoErro.clear();
    if (quantidade <= 0)
    {
        m_ultimoErro = QString::fromUtf8("A quantidade vendida deve ser maior que zero.");
        return false;
    }

    std::optional<Posicao> existente = m_repositorioCarteira.buscarPosicao(carteiraId, ativoId);
    if (!existente.has_value())
    {
        m_ultimoErro = QString::fromUtf8("Não há posição desse ativo na carteira.");
        return false;
    }

    Posicao posicao = existente.value();
    if (quantidade > posicao.quantidade())
    {
        m_ultimoErro = QString::fromUtf8("Você possui apenas %1 unidade(s); não é possível vender %2.")
                           .arg(posicao.quantidade())
                           .arg(quantidade);
        return false;
    }

    if (!posicao.venderParcial(quantidade))
    {
        m_ultimoErro = QString::fromUtf8("Não foi possível registrar a venda.");
        return false;
    }

    if (posicao.zerada())
    {
        // Posicao totalmente vendida deixa de existir na carteira.
        if (!m_repositorioCarteira.removerPosicao(posicao.id()))
        {
            m_ultimoErro = m_repositorioCarteira.ultimoErro();
            return false;
        }
        return true;
    }

    if (!m_repositorioCarteira.atualizarPosicao(posicao))
    {
        m_ultimoErro = m_repositorioCarteira.ultimoErro();
        return false;
    }
    return true;
}

ResumoCarteira ServicoCarteira::calcularResumo(qint64 carteiraId) const
{
    ResumoCarteira resumo;
    const QVector<Posicao> posicoes = m_repositorioCarteira.listarPosicoes(carteiraId);

    for (const Posicao& posicao : posicoes)
    {
        LinhaResumoCarteira linha;
        linha.ativoId = posicao.ativoId();
        linha.quantidade = posicao.quantidade();
        linha.precoMedio = posicao.precoMedio();
        linha.valorInvestido = posicao.custoTotal();

        const std::unique_ptr<Ativo> ativo = m_repositorioAtivo.buscarPorId(posicao.ativoId());
        if (ativo)
        {
            linha.ticker = ativo->ticker();
            linha.nomeAtivo = ativo->nomeEmpresa();
        }
        else
        {
            linha.ticker = QString::fromUtf8("(removido)");
        }

        const std::optional<Cotacao> ultima = m_repositorioCotacao.buscarUltima(posicao.ativoId());
        if (ultima.has_value())
        {
            linha.temCotacao = true;
            linha.precoAtual = ultima.value().fechamento();
        }
        else
        {
            // Sem cotacao importada, o preco medio e o melhor valor disponivel.
            linha.temCotacao = false;
            linha.precoAtual = posicao.precoMedio();
        }

        linha.valorAtual = posicao.valorAtual(linha.precoAtual);
        linha.lucroPrejuizo = posicao.lucroPrejuizo(linha.precoAtual);
        linha.rentabilidadePercentual = posicao.rentabilidadePercentual(linha.precoAtual);

        resumo.valorInvestido += linha.valorInvestido;
        resumo.valorAtual += linha.valorAtual;
        resumo.linhas.append(linha);
    }

    resumo.lucroPrejuizo = resumo.valorAtual - resumo.valorInvestido;
    resumo.rentabilidadePercentual = resumo.valorInvestido > 0.0
        ? (resumo.lucroPrejuizo / resumo.valorInvestido) * 100.0
        : 0.0;
    return resumo;
}

QString ServicoCarteira::ultimoErro() const
{
    return m_ultimoErro;
}

}
