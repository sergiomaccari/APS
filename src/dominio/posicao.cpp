#include "dominio/posicao.h"

namespace analisador
{

Posicao::Posicao()
{
}

Posicao::Posicao(qint64 id,
                 qint64 carteiraId,
                 qint64 ativoId,
                 int quantidade,
                 double precoMedio,
                 const QDate& compradaEm)
    : m_id(id)
    , m_carteiraId(carteiraId)
    , m_ativoId(ativoId)
    , m_quantidade(quantidade)
    , m_precoMedio(precoMedio)
    , m_compradaEm(compradaEm)
{
}

qint64 Posicao::id() const
{
    return m_id;
}

qint64 Posicao::carteiraId() const
{
    return m_carteiraId;
}

qint64 Posicao::ativoId() const
{
    return m_ativoId;
}

int Posicao::quantidade() const
{
    return m_quantidade;
}

double Posicao::precoMedio() const
{
    return m_precoMedio;
}

QDate Posicao::compradaEm() const
{
    return m_compradaEm;
}

void Posicao::definirId(qint64 id)
{
    m_id = id;
}

void Posicao::definirCarteiraId(qint64 carteiraId)
{
    m_carteiraId = carteiraId;
}

void Posicao::definirAtivoId(qint64 ativoId)
{
    m_ativoId = ativoId;
}

void Posicao::definirQuantidade(int quantidade)
{
    m_quantidade = quantidade < 0 ? 0 : quantidade;
}

void Posicao::definirPrecoMedio(double precoMedio)
{
    m_precoMedio = precoMedio < 0.0 ? 0.0 : precoMedio;
}

void Posicao::definirCompradaEm(const QDate& compradaEm)
{
    m_compradaEm = compradaEm;
}

double Posicao::custoTotal() const
{
    return static_cast<double>(m_quantidade) * m_precoMedio;
}

double Posicao::valorAtual(double precoAtual) const
{
    return static_cast<double>(m_quantidade) * precoAtual;
}

double Posicao::lucroPrejuizo(double precoAtual) const
{
    return valorAtual(precoAtual) - custoTotal();
}

double Posicao::rentabilidadePercentual(double precoAtual) const
{
    const double custo = custoTotal();
    if (custo <= 0.0)
    {
        return 0.0;
    }
    return (lucroPrejuizo(precoAtual) / custo) * 100.0;
}

bool Posicao::incorporarCompra(int quantidadeComprada, double precoCompra)
{
    if (quantidadeComprada <= 0 || precoCompra <= 0.0)
    {
        return false;
    }

    const double custoAnterior = custoTotal();
    const double custoNovo = static_cast<double>(quantidadeComprada) * precoCompra;
    const int quantidadeFinal = m_quantidade + quantidadeComprada;

    m_quantidade = quantidadeFinal;
    m_precoMedio = (custoAnterior + custoNovo) / static_cast<double>(quantidadeFinal);
    return true;
}

bool Posicao::venderParcial(int quantidadeVendida)
{
    if (quantidadeVendida <= 0 || quantidadeVendida > m_quantidade)
    {
        return false;
    }

    m_quantidade -= quantidadeVendida;
    if (m_quantidade == 0)
    {
        m_precoMedio = 0.0;
    }
    return true;
}

bool Posicao::zerada() const
{
    return m_quantidade <= 0;
}

}
