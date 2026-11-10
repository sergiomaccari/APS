#include "dominio/ativo.h"

namespace analisador
{

Ativo::Ativo()
{
}

Ativo::Ativo(qint64 id, const QString& ticker, const QString& nomeEmpresa, const QString& setor)
    : m_id(id)
    , m_ticker(ticker.trimmed().toUpper())
    , m_nomeEmpresa(nomeEmpresa)
    , m_setor(setor)
{
}

Ativo::~Ativo()
{
}

QString Ativo::descricaoFundamentos() const
{
    return QStringLiteral("%1 (%2) - setor %3. Indice de valor: %4 (%5).")
        .arg(m_ticker,
             tipo(),
             m_setor.isEmpty() ? QStringLiteral("nao informado") : m_setor,
             QString::number(indiceValor(), 'f', 1),
             classificarIndice(indiceValor()));
}

qint64 Ativo::id() const
{
    return m_id;
}

QString Ativo::ticker() const
{
    return m_ticker;
}

QString Ativo::nomeEmpresa() const
{
    return m_nomeEmpresa;
}

QString Ativo::setor() const
{
    return m_setor;
}

void Ativo::definirId(qint64 id)
{
    m_id = id;
}

void Ativo::definirTicker(const QString& ticker)
{
    m_ticker = ticker.trimmed().toUpper();
}

void Ativo::definirNomeEmpresa(const QString& nomeEmpresa)
{
    m_nomeEmpresa = nomeEmpresa;
}

void Ativo::definirSetor(const QString& setor)
{
    m_setor = setor;
}

QString Ativo::rotuloCompleto() const
{
    if (m_nomeEmpresa.trimmed().isEmpty())
    {
        return m_ticker;
    }
    return QStringLiteral("%1 - %2").arg(m_ticker, m_nomeEmpresa);
}

bool Ativo::valido() const
{
    return !m_ticker.trimmed().isEmpty() && !m_nomeEmpresa.trimmed().isEmpty();
}

QString Ativo::classificarIndice(double indice)
{
    if (indice >= 75.0)
    {
        return QString::fromUtf8("muito atrativo");
    }
    if (indice >= 55.0)
    {
        return QString::fromUtf8("atrativo");
    }
    if (indice >= 35.0)
    {
        return QString::fromUtf8("neutro");
    }
    return QString::fromUtf8("pouco atrativo");
}

}
