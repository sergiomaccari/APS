#include "dominio/alerta.h"

#include <QLocale>
#include <QtGlobal>

namespace analisador
{

QString paraTexto(EstadoAlerta estado)
{
    switch (estado)
    {
    case EstadoAlerta::Disparado:
        return QStringLiteral("Disparado");
    case EstadoAlerta::Arquivado:
        return QStringLiteral("Arquivado");
    case EstadoAlerta::Ativo:
        break;
    }
    return QStringLiteral("Ativo");
}

EstadoAlerta estadoAlertaDeTexto(const QString& texto)
{
    const QString normalizado = texto.trimmed();
    if (normalizado.compare(QStringLiteral("Disparado"), Qt::CaseInsensitive) == 0)
    {
        return EstadoAlerta::Disparado;
    }
    if (normalizado.compare(QStringLiteral("Arquivado"), Qt::CaseInsensitive) == 0)
    {
        return EstadoAlerta::Arquivado;
    }
    return EstadoAlerta::Ativo;
}

QString paraTexto(TipoCondicaoAlerta condicao)
{
    switch (condicao)
    {
    case TipoCondicaoAlerta::PrecoAbaixoDe:
        return QStringLiteral("PrecoAbaixoDe");
    case TipoCondicaoAlerta::VariacaoDiariaAcimaDe:
        return QStringLiteral("VariacaoDiariaAcimaDe");
    case TipoCondicaoAlerta::PrecoAcimaDe:
        break;
    }
    return QStringLiteral("PrecoAcimaDe");
}

TipoCondicaoAlerta tipoCondicaoAlertaDeTexto(const QString& texto)
{
    const QString normalizado = texto.trimmed();
    if (normalizado.compare(QStringLiteral("PrecoAbaixoDe"), Qt::CaseInsensitive) == 0)
    {
        return TipoCondicaoAlerta::PrecoAbaixoDe;
    }
    if (normalizado.compare(QStringLiteral("VariacaoDiariaAcimaDe"), Qt::CaseInsensitive) == 0)
    {
        return TipoCondicaoAlerta::VariacaoDiariaAcimaDe;
    }
    return TipoCondicaoAlerta::PrecoAcimaDe;
}

QString descricaoCondicao(TipoCondicaoAlerta condicao)
{
    switch (condicao)
    {
    case TipoCondicaoAlerta::PrecoAbaixoDe:
        return QString::fromUtf8("Preço abaixo de");
    case TipoCondicaoAlerta::VariacaoDiariaAcimaDe:
        return QString::fromUtf8("Variação diária acima de");
    case TipoCondicaoAlerta::PrecoAcimaDe:
        break;
    }
    return QString::fromUtf8("Preço acima de");
}

Alerta::Alerta()
{
}

Alerta::Alerta(qint64 id,
               qint64 usuarioId,
               qint64 ativoId,
               TipoCondicaoAlerta condicao,
               double valorReferencia,
               EstadoAlerta estado,
               const QDate& criadoEm)
    : m_id(id)
    , m_usuarioId(usuarioId)
    , m_ativoId(ativoId)
    , m_condicao(condicao)
    , m_valorReferencia(valorReferencia)
    , m_estado(estado)
    , m_criadoEm(criadoEm)
{
}

qint64 Alerta::id() const
{
    return m_id;
}

qint64 Alerta::usuarioId() const
{
    return m_usuarioId;
}

qint64 Alerta::ativoId() const
{
    return m_ativoId;
}

TipoCondicaoAlerta Alerta::condicao() const
{
    return m_condicao;
}

double Alerta::valorReferencia() const
{
    return m_valorReferencia;
}

EstadoAlerta Alerta::estado() const
{
    return m_estado;
}

QDate Alerta::criadoEm() const
{
    return m_criadoEm;
}

QDateTime Alerta::disparadoEm() const
{
    return m_disparadoEm;
}

void Alerta::definirId(qint64 id)
{
    m_id = id;
}

void Alerta::definirUsuarioId(qint64 usuarioId)
{
    m_usuarioId = usuarioId;
}

void Alerta::definirAtivoId(qint64 ativoId)
{
    m_ativoId = ativoId;
}

void Alerta::definirCondicao(TipoCondicaoAlerta condicao)
{
    m_condicao = condicao;
}

void Alerta::definirValorReferencia(double valorReferencia)
{
    m_valorReferencia = valorReferencia;
}

void Alerta::definirEstado(EstadoAlerta estado)
{
    m_estado = estado;
}

void Alerta::definirCriadoEm(const QDate& criadoEm)
{
    m_criadoEm = criadoEm;
}

void Alerta::definirDisparadoEm(const QDateTime& disparadoEm)
{
    m_disparadoEm = disparadoEm;
}

bool Alerta::condicaoSatisfeita(double valorObservado) const
{
    switch (m_condicao)
    {
    case TipoCondicaoAlerta::PrecoAcimaDe:
        return valorObservado > m_valorReferencia;
    case TipoCondicaoAlerta::PrecoAbaixoDe:
        return valorObservado < m_valorReferencia;
    case TipoCondicaoAlerta::VariacaoDiariaAcimaDe:
        // Compara o modulo: uma queda de 5% tambem satisfaz "variacao acima de 5%".
        return qAbs(valorObservado) > qAbs(m_valorReferencia);
    }
    return false;
}

bool Alerta::disparar(const QDateTime& momento)
{
    // Transicao valida somente a partir do estado Ativo (ver diagrama de estados).
    if (m_estado != EstadoAlerta::Ativo)
    {
        return false;
    }
    m_estado = EstadoAlerta::Disparado;
    m_disparadoEm = momento.isValid() ? momento : QDateTime::currentDateTime();
    return true;
}

bool Alerta::arquivar()
{
    // Somente um alerta ja disparado pode ser arquivado.
    if (m_estado != EstadoAlerta::Disparado)
    {
        return false;
    }
    m_estado = EstadoAlerta::Arquivado;
    return true;
}

bool Alerta::estaMonitorando() const
{
    return m_estado == EstadoAlerta::Ativo;
}

QString Alerta::descricao() const
{
    const QLocale brasil(QLocale::Portuguese, QLocale::Brazil);
    if (m_condicao == TipoCondicaoAlerta::VariacaoDiariaAcimaDe)
    {
        return QStringLiteral("%1 %2%")
            .arg(descricaoCondicao(m_condicao), brasil.toString(m_valorReferencia, 'f', 2));
    }
    return QStringLiteral("%1 R$ %2")
        .arg(descricaoCondicao(m_condicao), brasil.toString(m_valorReferencia, 'f', 2));
}

}
