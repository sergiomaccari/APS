#include "dominio/carteira.h"

namespace analisador
{

Carteira::Carteira()
{
}

Carteira::Carteira(qint64 id, qint64 usuarioId, const QString& nome, const QDate& criadaEm)
    : m_id(id)
    , m_usuarioId(usuarioId)
    , m_nome(nome)
    , m_criadaEm(criadaEm)
{
}

qint64 Carteira::id() const
{
    return m_id;
}

qint64 Carteira::usuarioId() const
{
    return m_usuarioId;
}

QString Carteira::nome() const
{
    return m_nome;
}

QDate Carteira::criadaEm() const
{
    return m_criadaEm;
}

const QVector<Posicao>& Carteira::posicoes() const
{
    return m_posicoes;
}

void Carteira::definirId(qint64 id)
{
    m_id = id;
}

void Carteira::definirUsuarioId(qint64 usuarioId)
{
    m_usuarioId = usuarioId;
}

void Carteira::definirNome(const QString& nome)
{
    m_nome = nome;
}

void Carteira::definirCriadaEm(const QDate& criadaEm)
{
    m_criadaEm = criadaEm;
}

void Carteira::definirPosicoes(const QVector<Posicao>& posicoes)
{
    m_posicoes = posicoes;
}

void Carteira::adicionarPosicao(const Posicao& posicao)
{
    Posicao* existente = posicaoDoAtivo(posicao.ativoId());
    if (existente != nullptr)
    {
        existente->incorporarCompra(posicao.quantidade(), posicao.precoMedio());
        return;
    }

    Posicao copia = posicao;
    copia.definirCarteiraId(m_id);
    m_posicoes.append(copia);
}

bool Carteira::removerPosicao(qint64 posicaoId)
{
    for (int indice = 0; indice < m_posicoes.size(); ++indice)
    {
        if (m_posicoes.at(indice).id() == posicaoId)
        {
            m_posicoes.removeAt(indice);
            return true;
        }
    }
    return false;
}

bool Carteira::removerPosicaoDoAtivo(qint64 ativoId)
{
    for (int indice = 0; indice < m_posicoes.size(); ++indice)
    {
        if (m_posicoes.at(indice).ativoId() == ativoId)
        {
            m_posicoes.removeAt(indice);
            return true;
        }
    }
    return false;
}

Posicao* Carteira::posicaoDoAtivo(qint64 ativoId)
{
    for (int indice = 0; indice < m_posicoes.size(); ++indice)
    {
        if (m_posicoes.at(indice).ativoId() == ativoId)
        {
            return &m_posicoes[indice];
        }
    }
    return nullptr;
}

const Posicao* Carteira::posicaoDoAtivo(qint64 ativoId) const
{
    for (int indice = 0; indice < m_posicoes.size(); ++indice)
    {
        if (m_posicoes.at(indice).ativoId() == ativoId)
        {
            return &m_posicoes.at(indice);
        }
    }
    return nullptr;
}

int Carteira::quantidadeDeAtivos() const
{
    return static_cast<int>(m_posicoes.size());
}

bool Carteira::vazia() const
{
    return m_posicoes.isEmpty();
}

double Carteira::custoTotal() const
{
    double total = 0.0;
    for (const Posicao& posicao : m_posicoes)
    {
        total += posicao.custoTotal();
    }
    return total;
}

double Carteira::valorDeMercado(const QHash<qint64, double>& precosAtuais) const
{
    double total = 0.0;
    for (const Posicao& posicao : m_posicoes)
    {
        const double preco = precosAtuais.value(posicao.ativoId(), posicao.precoMedio());
        total += posicao.valorAtual(preco);
    }
    return total;
}

double Carteira::lucroPrejuizo(const QHash<qint64, double>& precosAtuais) const
{
    return valorDeMercado(precosAtuais) - custoTotal();
}

double Carteira::rentabilidadePercentual(const QHash<qint64, double>& precosAtuais) const
{
    const double custo = custoTotal();
    if (custo <= 0.0)
    {
        return 0.0;
    }
    return (lucroPrejuizo(precosAtuais) / custo) * 100.0;
}

}
