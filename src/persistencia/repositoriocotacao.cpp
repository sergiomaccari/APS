#include "persistencia/repositoriocotacao.h"

#include <algorithm>

#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

#include "persistencia/bancodedados.h"

namespace analisador
{

RepositorioCotacao::RepositorioCotacao()
{
}

QString RepositorioCotacao::colunas()
{
    return QStringLiteral("id, ativo_id, data, abertura, maxima, minima, fechamento, volume");
}

Cotacao RepositorioCotacao::montarCotacao(const QSqlQuery& consulta)
{
    Cotacao cotacao;
    cotacao.definirId(consulta.value(0).toLongLong());
    cotacao.definirAtivoId(consulta.value(1).toLongLong());
    cotacao.definirData(QDate::fromString(consulta.value(2).toString(), Qt::ISODate));
    cotacao.definirAbertura(consulta.value(3).toDouble());
    cotacao.definirMaxima(consulta.value(4).toDouble());
    cotacao.definirMinima(consulta.value(5).toDouble());
    cotacao.definirFechamento(consulta.value(6).toDouble());
    cotacao.definirVolume(consulta.value(7).toLongLong());
    return cotacao;
}

bool RepositorioCotacao::salvar(Cotacao& cotacao)
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(QStringLiteral(
        "INSERT INTO cotacao (ativo_id, data, abertura, maxima, minima, fechamento, volume) "
        "VALUES (:ativo_id, :data, :abertura, :maxima, :minima, :fechamento, :volume)"));
    consulta.bindValue(QStringLiteral(":ativo_id"), cotacao.ativoId());
    consulta.bindValue(QStringLiteral(":data"), cotacao.data().toString(Qt::ISODate));
    consulta.bindValue(QStringLiteral(":abertura"), cotacao.abertura());
    consulta.bindValue(QStringLiteral(":maxima"), cotacao.maxima());
    consulta.bindValue(QStringLiteral(":minima"), cotacao.minima());
    consulta.bindValue(QStringLiteral(":fechamento"), cotacao.fechamento());
    consulta.bindValue(QStringLiteral(":volume"), cotacao.volume());

    if (!consulta.exec())
    {
        m_ultimoErro = consulta.lastError().text();
        return false;
    }

    cotacao.definirId(consulta.lastInsertId().toLongLong());
    m_ultimoErro.clear();
    return true;
}

int RepositorioCotacao::inserirEmLote(const QVector<Cotacao>& cotacoes)
{
    if (cotacoes.isEmpty())
    {
        m_ultimoErro.clear();
        return 0;
    }

    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    // Uma unica preparacao reaproveitada em todo o laco (desempenho e seguranca).
    if (!consulta.prepare(QStringLiteral(
            "INSERT OR IGNORE INTO cotacao "
            "(ativo_id, data, abertura, maxima, minima, fechamento, volume) "
            "VALUES (:ativo_id, :data, :abertura, :maxima, :minima, :fechamento, :volume)")))
    {
        m_ultimoErro = consulta.lastError().text();
        return 0;
    }

    int inseridas = 0;
    for (const Cotacao& cotacao : cotacoes)
    {
        consulta.bindValue(QStringLiteral(":ativo_id"), cotacao.ativoId());
        consulta.bindValue(QStringLiteral(":data"), cotacao.data().toString(Qt::ISODate));
        consulta.bindValue(QStringLiteral(":abertura"), cotacao.abertura());
        consulta.bindValue(QStringLiteral(":maxima"), cotacao.maxima());
        consulta.bindValue(QStringLiteral(":minima"), cotacao.minima());
        consulta.bindValue(QStringLiteral(":fechamento"), cotacao.fechamento());
        consulta.bindValue(QStringLiteral(":volume"), cotacao.volume());

        if (!consulta.exec())
        {
            m_ultimoErro = consulta.lastError().text();
            return inseridas;
        }
        if (consulta.numRowsAffected() > 0)
        {
            ++inseridas;
        }
    }

    m_ultimoErro.clear();
    return inseridas;
}

QVector<Cotacao> RepositorioCotacao::listarPorAtivo(qint64 ativoId, int limite) const
{
    QVector<Cotacao> cotacoes;
    QSqlQuery consulta(BancoDeDados::instancia().conexao());

    if (limite > 0)
    {
        // Pega as ultimas N em ordem decrescente e depois inverte a lista.
        consulta.prepare(QStringLiteral("SELECT %1 FROM cotacao WHERE ativo_id = :ativo_id "
                                        "ORDER BY data DESC LIMIT :limite")
                             .arg(colunas()));
        consulta.bindValue(QStringLiteral(":limite"), limite);
    }
    else
    {
        consulta.prepare(
            QStringLiteral("SELECT %1 FROM cotacao WHERE ativo_id = :ativo_id ORDER BY data")
                .arg(colunas()));
    }
    consulta.bindValue(QStringLiteral(":ativo_id"), ativoId);

    if (!consulta.exec())
    {
        m_ultimoErro = consulta.lastError().text();
        return cotacoes;
    }

    while (consulta.next())
    {
        cotacoes.append(montarCotacao(consulta));
    }

    if (limite > 0)
    {
        std::reverse(cotacoes.begin(), cotacoes.end());
    }

    m_ultimoErro.clear();
    return cotacoes;
}

std::optional<Cotacao> RepositorioCotacao::buscarUltima(qint64 ativoId) const
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(QStringLiteral("SELECT %1 FROM cotacao WHERE ativo_id = :ativo_id "
                                    "ORDER BY data DESC LIMIT 1")
                         .arg(colunas()));
    consulta.bindValue(QStringLiteral(":ativo_id"), ativoId);

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
    return montarCotacao(consulta);
}

bool RepositorioCotacao::existe(qint64 ativoId, const QDate& data) const
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(QStringLiteral(
        "SELECT COUNT(*) FROM cotacao WHERE ativo_id = :ativo_id AND data = :data"));
    consulta.bindValue(QStringLiteral(":ativo_id"), ativoId);
    consulta.bindValue(QStringLiteral(":data"), data.toString(Qt::ISODate));

    if (!consulta.exec() || !consulta.next())
    {
        m_ultimoErro = consulta.lastError().text();
        return false;
    }

    m_ultimoErro.clear();
    return consulta.value(0).toInt() > 0;
}

int RepositorioCotacao::contarPorAtivo(qint64 ativoId) const
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(QStringLiteral("SELECT COUNT(*) FROM cotacao WHERE ativo_id = :ativo_id"));
    consulta.bindValue(QStringLiteral(":ativo_id"), ativoId);

    if (!consulta.exec() || !consulta.next())
    {
        m_ultimoErro = consulta.lastError().text();
        return -1;
    }

    m_ultimoErro.clear();
    return consulta.value(0).toInt();
}

bool RepositorioCotacao::removerPorAtivo(qint64 ativoId)
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(QStringLiteral("DELETE FROM cotacao WHERE ativo_id = :ativo_id"));
    consulta.bindValue(QStringLiteral(":ativo_id"), ativoId);

    if (!consulta.exec())
    {
        m_ultimoErro = consulta.lastError().text();
        return false;
    }

    m_ultimoErro.clear();
    return true;
}

QString RepositorioCotacao::ultimoErro() const
{
    return m_ultimoErro;
}

}
