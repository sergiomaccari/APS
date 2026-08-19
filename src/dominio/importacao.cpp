#include "dominio/importacao.h"

#include <QFileInfo>

namespace analisador
{

QString paraTexto(EstadoImportacao estado)
{
    switch (estado)
    {
    case EstadoImportacao::Validando:
        return QStringLiteral("Validando");
    case EstadoImportacao::Concluida:
        return QStringLiteral("Concluida");
    case EstadoImportacao::Rejeitada:
        return QStringLiteral("Rejeitada");
    case EstadoImportacao::Pendente:
        break;
    }
    return QStringLiteral("Pendente");
}

EstadoImportacao estadoImportacaoDeTexto(const QString& texto)
{
    const QString normalizado = texto.trimmed();
    if (normalizado.compare(QStringLiteral("Validando"), Qt::CaseInsensitive) == 0)
    {
        return EstadoImportacao::Validando;
    }
    if (normalizado.compare(QStringLiteral("Concluida"), Qt::CaseInsensitive) == 0)
    {
        return EstadoImportacao::Concluida;
    }
    if (normalizado.compare(QStringLiteral("Rejeitada"), Qt::CaseInsensitive) == 0)
    {
        return EstadoImportacao::Rejeitada;
    }
    return EstadoImportacao::Pendente;
}

Importacao::Importacao()
{
}

Importacao::Importacao(qint64 id, qint64 ativoId, const QString& arquivo)
    : m_id(id)
    , m_ativoId(ativoId)
    , m_arquivo(arquivo)
{
}

qint64 Importacao::id() const
{
    return m_id;
}

qint64 Importacao::ativoId() const
{
    return m_ativoId;
}

QString Importacao::arquivo() const
{
    return m_arquivo;
}

EstadoImportacao Importacao::estado() const
{
    return m_estado;
}

int Importacao::linhasLidas() const
{
    return m_linhasLidas;
}

int Importacao::linhasInseridas() const
{
    return m_linhasInseridas;
}

QString Importacao::mensagemErro() const
{
    return m_mensagemErro;
}

QDateTime Importacao::executadaEm() const
{
    return m_executadaEm;
}

void Importacao::definirId(qint64 id)
{
    m_id = id;
}

void Importacao::definirAtivoId(qint64 ativoId)
{
    m_ativoId = ativoId;
}

void Importacao::definirArquivo(const QString& arquivo)
{
    m_arquivo = arquivo;
}

void Importacao::definirEstado(EstadoImportacao estado)
{
    m_estado = estado;
}

void Importacao::definirLinhasLidas(int linhasLidas)
{
    m_linhasLidas = linhasLidas;
}

void Importacao::definirLinhasInseridas(int linhasInseridas)
{
    m_linhasInseridas = linhasInseridas;
}

void Importacao::definirMensagemErro(const QString& mensagemErro)
{
    m_mensagemErro = mensagemErro;
}

void Importacao::definirExecutadaEm(const QDateTime& executadaEm)
{
    m_executadaEm = executadaEm;
}

bool Importacao::iniciarValidacao()
{
    if (m_estado != EstadoImportacao::Pendente)
    {
        return false;
    }
    m_estado = EstadoImportacao::Validando;
    return true;
}

bool Importacao::concluir(int linhasLidas, int linhasInseridas)
{
    if (m_estado != EstadoImportacao::Validando)
    {
        return false;
    }
    m_estado = EstadoImportacao::Concluida;
    m_linhasLidas = linhasLidas;
    m_linhasInseridas = linhasInseridas;
    m_mensagemErro.clear();
    m_executadaEm = QDateTime::currentDateTime();
    return true;
}

bool Importacao::rejeitar(const QString& motivo)
{
    if (m_estado != EstadoImportacao::Pendente && m_estado != EstadoImportacao::Validando)
    {
        return false;
    }
    m_estado = EstadoImportacao::Rejeitada;
    m_linhasInseridas = 0;
    m_mensagemErro = motivo;
    m_executadaEm = QDateTime::currentDateTime();
    return true;
}

bool Importacao::concluidaComSucesso() const
{
    return m_estado == EstadoImportacao::Concluida;
}

QString Importacao::nomeArquivo() const
{
    return QFileInfo(m_arquivo).fileName();
}

QString Importacao::resumo() const
{
    if (m_estado == EstadoImportacao::Rejeitada)
    {
        return QStringLiteral("%1 - Rejeitada (%2)").arg(nomeArquivo(), m_mensagemErro);
    }
    return QStringLiteral("%1 - %2 (%3 lidas, %4 inseridas)")
        .arg(nomeArquivo(), paraTexto(m_estado))
        .arg(m_linhasLidas)
        .arg(m_linhasInseridas);
}

}
