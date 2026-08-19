#include "dominio/recomendacao.h"

#include <QLocale>
#include <QtGlobal>

namespace analisador
{

const double Recomendacao::LIMIAR_DECISAO = 0.25;

QString paraTexto(TipoRecomendacao tipo)
{
    switch (tipo)
    {
    case TipoRecomendacao::Compra:
        return QStringLiteral("Compra");
    case TipoRecomendacao::Venda:
        return QStringLiteral("Venda");
    case TipoRecomendacao::Neutro:
        break;
    }
    return QStringLiteral("Neutro");
}

TipoRecomendacao tipoRecomendacaoDeTexto(const QString& texto)
{
    const QString normalizado = texto.trimmed();
    if (normalizado.compare(QStringLiteral("Compra"), Qt::CaseInsensitive) == 0)
    {
        return TipoRecomendacao::Compra;
    }
    if (normalizado.compare(QStringLiteral("Venda"), Qt::CaseInsensitive) == 0)
    {
        return TipoRecomendacao::Venda;
    }
    return TipoRecomendacao::Neutro;
}

QString paraTexto(EstadoRecomendacao estado)
{
    switch (estado)
    {
    case EstadoRecomendacao::Vigente:
        return QStringLiteral("Vigente");
    case EstadoRecomendacao::Expirada:
        return QStringLiteral("Expirada");
    case EstadoRecomendacao::Invalidada:
        return QStringLiteral("Invalidada");
    case EstadoRecomendacao::Gerada:
        break;
    }
    return QStringLiteral("Gerada");
}

EstadoRecomendacao estadoRecomendacaoDeTexto(const QString& texto)
{
    const QString normalizado = texto.trimmed();
    if (normalizado.compare(QStringLiteral("Vigente"), Qt::CaseInsensitive) == 0)
    {
        return EstadoRecomendacao::Vigente;
    }
    if (normalizado.compare(QStringLiteral("Expirada"), Qt::CaseInsensitive) == 0)
    {
        return EstadoRecomendacao::Expirada;
    }
    if (normalizado.compare(QStringLiteral("Invalidada"), Qt::CaseInsensitive) == 0)
    {
        return EstadoRecomendacao::Invalidada;
    }
    return EstadoRecomendacao::Gerada;
}

Recomendacao::Recomendacao()
{
}

Recomendacao::Recomendacao(qint64 id,
                           qint64 ativoId,
                           TipoRecomendacao tipo,
                           const QString& justificativa,
                           const QString& regraAplicada,
                           const QDate& geradaEm,
                           double pontuacao)
    : m_id(id)
    , m_ativoId(ativoId)
    , m_tipo(tipo)
    , m_justificativa(justificativa)
    , m_regraAplicada(regraAplicada)
    , m_geradaEm(geradaEm)
    , m_pontuacao(qBound(-1.0, pontuacao, 1.0))
{
}

qint64 Recomendacao::id() const
{
    return m_id;
}

qint64 Recomendacao::ativoId() const
{
    return m_ativoId;
}

TipoRecomendacao Recomendacao::tipo() const
{
    return m_tipo;
}

QString Recomendacao::justificativa() const
{
    return m_justificativa;
}

QString Recomendacao::regraAplicada() const
{
    return m_regraAplicada;
}

QDate Recomendacao::geradaEm() const
{
    return m_geradaEm;
}

EstadoRecomendacao Recomendacao::estado() const
{
    return m_estado;
}

double Recomendacao::pontuacao() const
{
    return m_pontuacao;
}

void Recomendacao::definirId(qint64 id)
{
    m_id = id;
}

void Recomendacao::definirAtivoId(qint64 ativoId)
{
    m_ativoId = ativoId;
}

void Recomendacao::definirTipo(TipoRecomendacao tipo)
{
    m_tipo = tipo;
}

void Recomendacao::definirJustificativa(const QString& justificativa)
{
    m_justificativa = justificativa;
}

void Recomendacao::definirRegraAplicada(const QString& regraAplicada)
{
    m_regraAplicada = regraAplicada;
}

void Recomendacao::definirGeradaEm(const QDate& geradaEm)
{
    m_geradaEm = geradaEm;
}

void Recomendacao::definirEstado(EstadoRecomendacao estado)
{
    m_estado = estado;
}

void Recomendacao::definirPontuacao(double pontuacao)
{
    m_pontuacao = qBound(-1.0, pontuacao, 1.0);
}

bool Recomendacao::marcarComoVigente()
{
    if (m_estado != EstadoRecomendacao::Gerada)
    {
        return false;
    }
    m_estado = EstadoRecomendacao::Vigente;
    return true;
}

bool Recomendacao::expirar()
{
    if (m_estado != EstadoRecomendacao::Vigente)
    {
        return false;
    }
    m_estado = EstadoRecomendacao::Expirada;
    return true;
}

bool Recomendacao::invalidar()
{
    if (m_estado != EstadoRecomendacao::Gerada && m_estado != EstadoRecomendacao::Vigente)
    {
        return false;
    }
    m_estado = EstadoRecomendacao::Invalidada;
    return true;
}

bool Recomendacao::estaVigente() const
{
    return m_estado == EstadoRecomendacao::Vigente;
}

QString Recomendacao::descricaoCurta() const
{
    const QLocale brasil(QLocale::Portuguese, QLocale::Brazil);
    return QStringLiteral("%1 (%2)").arg(paraTexto(m_tipo), brasil.toString(m_pontuacao, 'f', 2));
}

TipoRecomendacao Recomendacao::tipoPorPontuacao(double pontuacao)
{
    if (pontuacao >= LIMIAR_DECISAO)
    {
        return TipoRecomendacao::Compra;
    }
    if (pontuacao <= -LIMIAR_DECISAO)
    {
        return TipoRecomendacao::Venda;
    }
    return TipoRecomendacao::Neutro;
}

}
