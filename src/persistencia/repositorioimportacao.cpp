#include "persistencia/repositorioimportacao.h"

#include <QDateTime>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

#include "persistencia/bancodedados.h"

namespace analisador
{

RepositorioImportacao::RepositorioImportacao()
{
}

Importacao RepositorioImportacao::montarImportacao(const QSqlQuery& consulta)
{
    Importacao importacao;
    importacao.definirId(consulta.value(QStringLiteral("id")).toLongLong());
    importacao.definirAtivoId(consulta.value(QStringLiteral("ativo_id")).toLongLong());
    importacao.definirArquivo(consulta.value(QStringLiteral("arquivo")).toString());
    importacao.definirEstado(estadoImportacaoDeTexto(consulta.value(QStringLiteral("estado")).toString()));
    importacao.definirLinhasLidas(consulta.value(QStringLiteral("linhas_lidas")).toInt());
    importacao.definirLinhasInseridas(consulta.value(QStringLiteral("linhas_inseridas")).toInt());
    importacao.definirMensagemErro(consulta.value(QStringLiteral("mensagem_erro")).toString());

    const QVariant executadaEm = consulta.value(QStringLiteral("executada_em"));
    if (!executadaEm.isNull())
    {
        importacao.definirExecutadaEm(QDateTime::fromString(executadaEm.toString(), Qt::ISODate));
    }
    return importacao;
}

bool RepositorioImportacao::salvar(Importacao& importacao)
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(QStringLiteral(
        "INSERT INTO importacao (ativo_id, arquivo, estado, linhas_lidas, linhas_inseridas, "
        "mensagem_erro, executada_em) "
        "VALUES (:ativo_id, :arquivo, :estado, :linhas_lidas, :linhas_inseridas, "
        ":mensagem_erro, :executada_em)"));
    consulta.bindValue(QStringLiteral(":ativo_id"), importacao.ativoId());
    consulta.bindValue(QStringLiteral(":arquivo"), importacao.arquivo());
    consulta.bindValue(QStringLiteral(":estado"), paraTexto(importacao.estado()));
    consulta.bindValue(QStringLiteral(":linhas_lidas"), importacao.linhasLidas());
    consulta.bindValue(QStringLiteral(":linhas_inseridas"), importacao.linhasInseridas());
    consulta.bindValue(QStringLiteral(":mensagem_erro"), importacao.mensagemErro());
    consulta.bindValue(QStringLiteral(":executada_em"),
                       importacao.executadaEm().isValid()
                           ? QVariant(importacao.executadaEm().toString(Qt::ISODate))
                           : QVariant());

    if (!consulta.exec())
    {
        m_ultimoErro = consulta.lastError().text();
        return false;
    }
    importacao.definirId(consulta.lastInsertId().toLongLong());
    return true;
}

bool RepositorioImportacao::atualizar(const Importacao& importacao)
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(QStringLiteral(
        "UPDATE importacao SET ativo_id = :ativo_id, arquivo = :arquivo, estado = :estado, "
        "linhas_lidas = :linhas_lidas, linhas_inseridas = :linhas_inseridas, "
        "mensagem_erro = :mensagem_erro, executada_em = :executada_em WHERE id = :id"));
    consulta.bindValue(QStringLiteral(":ativo_id"), importacao.ativoId());
    consulta.bindValue(QStringLiteral(":arquivo"), importacao.arquivo());
    consulta.bindValue(QStringLiteral(":estado"), paraTexto(importacao.estado()));
    consulta.bindValue(QStringLiteral(":linhas_lidas"), importacao.linhasLidas());
    consulta.bindValue(QStringLiteral(":linhas_inseridas"), importacao.linhasInseridas());
    consulta.bindValue(QStringLiteral(":mensagem_erro"), importacao.mensagemErro());
    consulta.bindValue(QStringLiteral(":executada_em"),
                       importacao.executadaEm().isValid()
                           ? QVariant(importacao.executadaEm().toString(Qt::ISODate))
                           : QVariant());
    consulta.bindValue(QStringLiteral(":id"), importacao.id());

    if (!consulta.exec())
    {
        m_ultimoErro = consulta.lastError().text();
        return false;
    }
    return true;
}

std::optional<Importacao> RepositorioImportacao::buscarPorId(qint64 id) const
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(QStringLiteral("SELECT * FROM importacao WHERE id = :id"));
    consulta.bindValue(QStringLiteral(":id"), id);
    if (!consulta.exec())
    {
        m_ultimoErro = consulta.lastError().text();
        return std::nullopt;
    }
    if (!consulta.next())
    {
        return std::nullopt;
    }
    return montarImportacao(consulta);
}

QVector<Importacao> RepositorioImportacao::listarRecentes(int limite) const
{
    QVector<Importacao> lista;
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    if (limite > 0)
    {
        consulta.prepare(QStringLiteral(
            "SELECT * FROM importacao ORDER BY id DESC LIMIT :limite"));
        consulta.bindValue(QStringLiteral(":limite"), limite);
    }
    else
    {
        consulta.prepare(QStringLiteral("SELECT * FROM importacao ORDER BY id DESC"));
    }
    if (!consulta.exec())
    {
        m_ultimoErro = consulta.lastError().text();
        return lista;
    }
    while (consulta.next())
    {
        lista.append(montarImportacao(consulta));
    }
    return lista;
}

QVector<Importacao> RepositorioImportacao::listarPorAtivo(qint64 ativoId) const
{
    QVector<Importacao> lista;
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(QStringLiteral(
        "SELECT * FROM importacao WHERE ativo_id = :ativo_id ORDER BY id DESC"));
    consulta.bindValue(QStringLiteral(":ativo_id"), ativoId);
    if (!consulta.exec())
    {
        m_ultimoErro = consulta.lastError().text();
        return lista;
    }
    while (consulta.next())
    {
        lista.append(montarImportacao(consulta));
    }
    return lista;
}

QString RepositorioImportacao::ultimoErro() const
{
    return m_ultimoErro;
}

}
