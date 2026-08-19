#include "persistencia/repositoriorecomendacao.h"

#include <QDate>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

#include "persistencia/bancodedados.h"

namespace analisador
{

RepositorioRecomendacao::RepositorioRecomendacao()
{
}

QString RepositorioRecomendacao::colunas()
{
    return QStringLiteral("id, ativo_id, tipo, justificativa, regra_aplicada, gerada_em, "
                          "estado, pontuacao");
}

Recomendacao RepositorioRecomendacao::montarRecomendacao(const QSqlQuery& consulta)
{
    Recomendacao recomendacao;
    recomendacao.definirId(consulta.value(0).toLongLong());
    recomendacao.definirAtivoId(consulta.value(1).toLongLong());
    recomendacao.definirTipo(tipoRecomendacaoDeTexto(consulta.value(2).toString()));
    recomendacao.definirJustificativa(consulta.value(3).toString());
    recomendacao.definirRegraAplicada(consulta.value(4).toString());
    recomendacao.definirGeradaEm(QDate::fromString(consulta.value(5).toString(), Qt::ISODate));
    recomendacao.definirEstado(estadoRecomendacaoDeTexto(consulta.value(6).toString()));
    recomendacao.definirPontuacao(consulta.value(7).toDouble());
    return recomendacao;
}

QVector<Recomendacao> RepositorioRecomendacao::lerTodas(QSqlQuery& consulta)
{
    QVector<Recomendacao> recomendacoes;
    while (consulta.next())
    {
        recomendacoes.append(montarRecomendacao(consulta));
    }
    return recomendacoes;
}

bool RepositorioRecomendacao::salvar(Recomendacao& recomendacao)
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(QStringLiteral(
        "INSERT INTO recomendacao (ativo_id, tipo, justificativa, regra_aplicada, gerada_em, "
        "estado, pontuacao) "
        "VALUES (:ativo_id, :tipo, :justificativa, :regra_aplicada, :gerada_em, "
        ":estado, :pontuacao)"));
    consulta.bindValue(QStringLiteral(":ativo_id"), recomendacao.ativoId());
    consulta.bindValue(QStringLiteral(":tipo"), paraTexto(recomendacao.tipo()));
    consulta.bindValue(QStringLiteral(":justificativa"), recomendacao.justificativa());
    consulta.bindValue(QStringLiteral(":regra_aplicada"), recomendacao.regraAplicada());
    consulta.bindValue(QStringLiteral(":gerada_em"), recomendacao.geradaEm().toString(Qt::ISODate));
    consulta.bindValue(QStringLiteral(":estado"), paraTexto(recomendacao.estado()));
    consulta.bindValue(QStringLiteral(":pontuacao"), recomendacao.pontuacao());

    if (!consulta.exec())
    {
        m_ultimoErro = consulta.lastError().text();
        return false;
    }

    recomendacao.definirId(consulta.lastInsertId().toLongLong());
    m_ultimoErro.clear();
    return true;
}

bool RepositorioRecomendacao::atualizar(const Recomendacao& recomendacao)
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(QStringLiteral(
        "UPDATE recomendacao SET ativo_id = :ativo_id, tipo = :tipo, "
        "justificativa = :justificativa, regra_aplicada = :regra_aplicada, "
        "gerada_em = :gerada_em, estado = :estado, pontuacao = :pontuacao WHERE id = :id"));
    consulta.bindValue(QStringLiteral(":ativo_id"), recomendacao.ativoId());
    consulta.bindValue(QStringLiteral(":tipo"), paraTexto(recomendacao.tipo()));
    consulta.bindValue(QStringLiteral(":justificativa"), recomendacao.justificativa());
    consulta.bindValue(QStringLiteral(":regra_aplicada"), recomendacao.regraAplicada());
    consulta.bindValue(QStringLiteral(":gerada_em"), recomendacao.geradaEm().toString(Qt::ISODate));
    consulta.bindValue(QStringLiteral(":estado"), paraTexto(recomendacao.estado()));
    consulta.bindValue(QStringLiteral(":pontuacao"), recomendacao.pontuacao());
    consulta.bindValue(QStringLiteral(":id"), recomendacao.id());

    if (!consulta.exec())
    {
        m_ultimoErro = consulta.lastError().text();
        return false;
    }

    m_ultimoErro.clear();
    return true;
}

std::optional<Recomendacao> RepositorioRecomendacao::buscarVigenteDoAtivo(qint64 ativoId) const
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(QStringLiteral("SELECT %1 FROM recomendacao "
                                    "WHERE ativo_id = :ativo_id AND estado = :estado "
                                    "ORDER BY gerada_em DESC, id DESC LIMIT 1")
                         .arg(colunas()));
    consulta.bindValue(QStringLiteral(":ativo_id"), ativoId);
    consulta.bindValue(QStringLiteral(":estado"), paraTexto(EstadoRecomendacao::Vigente));

    if (!consulta.exec())
    {
        m_ultimoErro = consulta.lastError().text();
        return std::nullopt;
    }

    m_ultimoErro.clear();
    if (!consulta.next())
    {
        return std::nullopt;
    }
    return montarRecomendacao(consulta);
}

QVector<Recomendacao> RepositorioRecomendacao::listarVigentes() const
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(QStringLiteral("SELECT %1 FROM recomendacao WHERE estado = :estado "
                                    "ORDER BY gerada_em DESC, id DESC")
                         .arg(colunas()));
    consulta.bindValue(QStringLiteral(":estado"), paraTexto(EstadoRecomendacao::Vigente));

    if (!consulta.exec())
    {
        m_ultimoErro = consulta.lastError().text();
        return QVector<Recomendacao>();
    }

    m_ultimoErro.clear();
    return lerTodas(consulta);
}

QVector<Recomendacao> RepositorioRecomendacao::listarPorAtivo(qint64 ativoId, int limite) const
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());

    if (limite > 0)
    {
        consulta.prepare(QStringLiteral("SELECT %1 FROM recomendacao WHERE ativo_id = :ativo_id "
                                        "ORDER BY gerada_em DESC, id DESC LIMIT :limite")
                             .arg(colunas()));
        consulta.bindValue(QStringLiteral(":limite"), limite);
    }
    else
    {
        consulta.prepare(QStringLiteral("SELECT %1 FROM recomendacao WHERE ativo_id = :ativo_id "
                                        "ORDER BY gerada_em DESC, id DESC")
                             .arg(colunas()));
    }
    consulta.bindValue(QStringLiteral(":ativo_id"), ativoId);

    if (!consulta.exec())
    {
        m_ultimoErro = consulta.lastError().text();
        return QVector<Recomendacao>();
    }

    m_ultimoErro.clear();
    return lerTodas(consulta);
}

int RepositorioRecomendacao::invalidarVigentesDoAtivo(qint64 ativoId) const
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(QStringLiteral(
        "UPDATE recomendacao SET estado = :novoEstado "
        "WHERE ativo_id = :ativo_id AND estado IN (:estadoGerada, :estadoVigente)"));
    consulta.bindValue(QStringLiteral(":novoEstado"),
                       paraTexto(EstadoRecomendacao::Invalidada));
    consulta.bindValue(QStringLiteral(":ativo_id"), ativoId);
    consulta.bindValue(QStringLiteral(":estadoGerada"), paraTexto(EstadoRecomendacao::Gerada));
    consulta.bindValue(QStringLiteral(":estadoVigente"), paraTexto(EstadoRecomendacao::Vigente));

    if (!consulta.exec())
    {
        m_ultimoErro = consulta.lastError().text();
        return -1;
    }

    m_ultimoErro.clear();
    return consulta.numRowsAffected();
}

QString RepositorioRecomendacao::ultimoErro() const
{
    return m_ultimoErro;
}

}
