#include "persistencia/repositoriocarteira.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

#include "persistencia/bancodedados.h"

namespace analisador
{

RepositorioCarteira::RepositorioCarteira()
{
}

QString RepositorioCarteira::colunasCarteira()
{
    return QStringLiteral("id, usuario_id, nome, criada_em");
}

QString RepositorioCarteira::colunasPosicao()
{
    return QStringLiteral("id, carteira_id, ativo_id, quantidade, preco_medio, comprada_em");
}

Carteira RepositorioCarteira::montarCarteira(const QSqlQuery& consulta)
{
    Carteira carteira;
    carteira.definirId(consulta.value(0).toLongLong());
    carteira.definirUsuarioId(consulta.value(1).toLongLong());
    carteira.definirNome(consulta.value(2).toString());
    carteira.definirCriadaEm(QDate::fromString(consulta.value(3).toString(), Qt::ISODate));
    return carteira;
}

Posicao RepositorioCarteira::montarPosicao(const QSqlQuery& consulta)
{
    Posicao posicao;
    posicao.definirId(consulta.value(0).toLongLong());
    posicao.definirCarteiraId(consulta.value(1).toLongLong());
    posicao.definirAtivoId(consulta.value(2).toLongLong());
    posicao.definirQuantidade(consulta.value(3).toInt());
    posicao.definirPrecoMedio(consulta.value(4).toDouble());
    posicao.definirCompradaEm(QDate::fromString(consulta.value(5).toString(), Qt::ISODate));
    return posicao;
}

bool RepositorioCarteira::salvar(Carteira& carteira)
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(QStringLiteral(
        "INSERT INTO carteira (usuario_id, nome, criada_em) "
        "VALUES (:usuario_id, :nome, :criada_em)"));
    consulta.bindValue(QStringLiteral(":usuario_id"), carteira.usuarioId());
    consulta.bindValue(QStringLiteral(":nome"), carteira.nome());
    consulta.bindValue(QStringLiteral(":criada_em"), carteira.criadaEm().toString(Qt::ISODate));

    if (!consulta.exec())
    {
        m_ultimoErro = consulta.lastError().text();
        return false;
    }

    carteira.definirId(consulta.lastInsertId().toLongLong());
    m_ultimoErro.clear();
    return true;
}

bool RepositorioCarteira::atualizar(const Carteira& carteira)
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(QStringLiteral(
        "UPDATE carteira SET usuario_id = :usuario_id, nome = :nome, criada_em = :criada_em "
        "WHERE id = :id"));
    consulta.bindValue(QStringLiteral(":usuario_id"), carteira.usuarioId());
    consulta.bindValue(QStringLiteral(":nome"), carteira.nome());
    consulta.bindValue(QStringLiteral(":criada_em"), carteira.criadaEm().toString(Qt::ISODate));
    consulta.bindValue(QStringLiteral(":id"), carteira.id());

    if (!consulta.exec())
    {
        m_ultimoErro = consulta.lastError().text();
        return false;
    }

    m_ultimoErro.clear();
    return true;
}

bool RepositorioCarteira::remover(qint64 id)
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(QStringLiteral("DELETE FROM carteira WHERE id = :id"));
    consulta.bindValue(QStringLiteral(":id"), id);

    if (!consulta.exec())
    {
        m_ultimoErro = consulta.lastError().text();
        return false;
    }

    m_ultimoErro.clear();
    return true;
}

std::optional<Carteira> RepositorioCarteira::buscarPorId(qint64 id) const
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(
        QStringLiteral("SELECT %1 FROM carteira WHERE id = :id").arg(colunasCarteira()));
    consulta.bindValue(QStringLiteral(":id"), id);

    if (!consulta.exec())
    {
        m_ultimoErro = consulta.lastError().text();
        return std::nullopt;
    }

    if (!consulta.next())
    {
        m_ultimoErro.clear();
        return std::nullopt;
    }

    Carteira carteira = montarCarteira(consulta);
    carteira.definirPosicoes(listarPosicoes(carteira.id()));
    return carteira;
}

std::optional<Carteira> RepositorioCarteira::buscarPrincipalDoUsuario(qint64 usuarioId) const
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    // A carteira principal e a mais antiga; o id desempata carteiras criadas no mesmo dia.
    consulta.prepare(QStringLiteral("SELECT %1 FROM carteira WHERE usuario_id = :usuario_id "
                                    "ORDER BY criada_em, id LIMIT 1")
                         .arg(colunasCarteira()));
    consulta.bindValue(QStringLiteral(":usuario_id"), usuarioId);

    if (!consulta.exec())
    {
        m_ultimoErro = consulta.lastError().text();
        return std::nullopt;
    }

    if (!consulta.next())
    {
        m_ultimoErro.clear();
        return std::nullopt;
    }

    Carteira carteira = montarCarteira(consulta);
    carteira.definirPosicoes(listarPosicoes(carteira.id()));
    return carteira;
}

QVector<Carteira> RepositorioCarteira::listarPorUsuario(qint64 usuarioId) const
{
    QVector<Carteira> carteiras;
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(QStringLiteral("SELECT %1 FROM carteira WHERE usuario_id = :usuario_id "
                                    "ORDER BY criada_em, id")
                         .arg(colunasCarteira()));
    consulta.bindValue(QStringLiteral(":usuario_id"), usuarioId);

    if (!consulta.exec())
    {
        m_ultimoErro = consulta.lastError().text();
        return carteiras;
    }

    while (consulta.next())
    {
        carteiras.append(montarCarteira(consulta));
    }

    // As posicoes sao carregadas depois de ler todas as carteiras para nao
    // reaproveitar a mesma consulta enquanto ela ainda esta sendo percorrida.
    for (Carteira& carteira : carteiras)
    {
        carteira.definirPosicoes(listarPosicoes(carteira.id()));
    }

    m_ultimoErro.clear();
    return carteiras;
}

bool RepositorioCarteira::salvarPosicao(Posicao& posicao)
{
    const std::optional<Posicao> existente =
        buscarPosicao(posicao.carteiraId(), posicao.ativoId());

    if (existente.has_value())
    {
        // Ja existe posicao do par (carteira, ativo): atualiza em vez de duplicar.
        posicao.definirId(existente.value().id());
        return atualizarPosicao(posicao);
    }

    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(QStringLiteral(
        "INSERT INTO posicao (carteira_id, ativo_id, quantidade, preco_medio, comprada_em) "
        "VALUES (:carteira_id, :ativo_id, :quantidade, :preco_medio, :comprada_em)"));
    consulta.bindValue(QStringLiteral(":carteira_id"), posicao.carteiraId());
    consulta.bindValue(QStringLiteral(":ativo_id"), posicao.ativoId());
    consulta.bindValue(QStringLiteral(":quantidade"), posicao.quantidade());
    consulta.bindValue(QStringLiteral(":preco_medio"), posicao.precoMedio());
    consulta.bindValue(QStringLiteral(":comprada_em"), posicao.compradaEm().toString(Qt::ISODate));

    if (!consulta.exec())
    {
        m_ultimoErro = consulta.lastError().text();
        return false;
    }

    posicao.definirId(consulta.lastInsertId().toLongLong());
    m_ultimoErro.clear();
    return true;
}

bool RepositorioCarteira::atualizarPosicao(const Posicao& posicao)
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(QStringLiteral(
        "UPDATE posicao SET quantidade = :quantidade, preco_medio = :preco_medio, "
        "comprada_em = :comprada_em WHERE id = :id"));
    consulta.bindValue(QStringLiteral(":quantidade"), posicao.quantidade());
    consulta.bindValue(QStringLiteral(":preco_medio"), posicao.precoMedio());
    consulta.bindValue(QStringLiteral(":comprada_em"), posicao.compradaEm().toString(Qt::ISODate));
    consulta.bindValue(QStringLiteral(":id"), posicao.id());

    if (!consulta.exec())
    {
        m_ultimoErro = consulta.lastError().text();
        return false;
    }

    m_ultimoErro.clear();
    return true;
}

bool RepositorioCarteira::removerPosicao(qint64 posicaoId)
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(QStringLiteral("DELETE FROM posicao WHERE id = :id"));
    consulta.bindValue(QStringLiteral(":id"), posicaoId);

    if (!consulta.exec())
    {
        m_ultimoErro = consulta.lastError().text();
        return false;
    }

    m_ultimoErro.clear();
    return true;
}

QVector<Posicao> RepositorioCarteira::listarPosicoes(qint64 carteiraId) const
{
    QVector<Posicao> posicoes;
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(QStringLiteral("SELECT %1 FROM posicao WHERE carteira_id = :carteira_id "
                                    "ORDER BY ativo_id")
                         .arg(colunasPosicao()));
    consulta.bindValue(QStringLiteral(":carteira_id"), carteiraId);

    if (!consulta.exec())
    {
        m_ultimoErro = consulta.lastError().text();
        return posicoes;
    }

    while (consulta.next())
    {
        posicoes.append(montarPosicao(consulta));
    }

    m_ultimoErro.clear();
    return posicoes;
}

std::optional<Posicao> RepositorioCarteira::buscarPosicao(qint64 carteiraId, qint64 ativoId) const
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(QStringLiteral("SELECT %1 FROM posicao "
                                    "WHERE carteira_id = :carteira_id AND ativo_id = :ativo_id")
                         .arg(colunasPosicao()));
    consulta.bindValue(QStringLiteral(":carteira_id"), carteiraId);
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
    return montarPosicao(consulta);
}

QString RepositorioCarteira::ultimoErro() const
{
    return m_ultimoErro;
}

}
