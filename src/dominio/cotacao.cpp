#include "dominio/cotacao.h"

namespace analisador
{

Cotacao::Cotacao()
{
}

Cotacao::Cotacao(qint64 id,
                 qint64 ativoId,
                 const QDate& data,
                 double abertura,
                 double maxima,
                 double minima,
                 double fechamento,
                 qint64 volume)
    : m_id(id)
    , m_ativoId(ativoId)
    , m_data(data)
    , m_abertura(abertura)
    , m_maxima(maxima)
    , m_minima(minima)
    , m_fechamento(fechamento)
    , m_volume(volume)
{
}

qint64 Cotacao::id() const
{
    return m_id;
}

qint64 Cotacao::ativoId() const
{
    return m_ativoId;
}

QDate Cotacao::data() const
{
    return m_data;
}

double Cotacao::abertura() const
{
    return m_abertura;
}

double Cotacao::maxima() const
{
    return m_maxima;
}

double Cotacao::minima() const
{
    return m_minima;
}

double Cotacao::fechamento() const
{
    return m_fechamento;
}

qint64 Cotacao::volume() const
{
    return m_volume;
}

void Cotacao::definirId(qint64 id)
{
    m_id = id;
}

void Cotacao::definirAtivoId(qint64 ativoId)
{
    m_ativoId = ativoId;
}

void Cotacao::definirData(const QDate& data)
{
    m_data = data;
}

void Cotacao::definirAbertura(double abertura)
{
    m_abertura = abertura;
}

void Cotacao::definirMaxima(double maxima)
{
    m_maxima = maxima;
}

void Cotacao::definirMinima(double minima)
{
    m_minima = minima;
}

void Cotacao::definirFechamento(double fechamento)
{
    m_fechamento = fechamento;
}

void Cotacao::definirVolume(qint64 volume)
{
    m_volume = volume;
}

double Cotacao::variacaoPercentual() const
{
    if (m_abertura <= 0.0)
    {
        return 0.0;
    }
    return ((m_fechamento - m_abertura) / m_abertura) * 100.0;
}

double Cotacao::amplitude() const
{
    return m_maxima - m_minima;
}

bool Cotacao::ehAlta() const
{
    return m_fechamento >= m_abertura;
}

bool Cotacao::valida() const
{
    if (!m_data.isValid())
    {
        return false;
    }
    if (m_abertura <= 0.0 || m_maxima <= 0.0 || m_minima <= 0.0 || m_fechamento <= 0.0)
    {
        return false;
    }
    if (m_volume < 0)
    {
        return false;
    }
    if (m_minima > m_maxima)
    {
        return false;
    }
    if (m_abertura < m_minima || m_abertura > m_maxima)
    {
        return false;
    }
    if (m_fechamento < m_minima || m_fechamento > m_maxima)
    {
        return false;
    }
    return true;
}

bool Cotacao::operator<(const Cotacao& outra) const
{
    return m_data < outra.m_data;
}

}
