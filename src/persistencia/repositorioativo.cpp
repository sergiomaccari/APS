#include "persistencia/repositorioativo.h"

#include <utility>

#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

#include "dominio/acao.h"
#include "dominio/fundoimobiliario.h"
#include "persistencia/bancodedados.h"

namespace analisador
{

RepositorioAtivo::RepositorioAtivo()
{
}

QString RepositorioAtivo::colunas()
{
    return QStringLiteral(
        "id, ticker, nome_empresa, setor, tipo, preco_lucro, valor_mercado, "
        "dividend_yield, valor_patrimonial_por_cota, segmento");
}

std::unique_ptr<Ativo> RepositorioAtivo::montarAtivo(const QSqlQuery& consulta)
{
    const qint64 id = consulta.value(0).toLongLong();
    const QString ticker = consulta.value(1).toString();
    const QString nomeEmpresa = consulta.value(2).toString();
    const QString setor = consulta.value(3).toString();
    const QString tipo = consulta.value(4).toString().trimmed().toUpper();
    const double dividendYield = consulta.value(7).toDouble();

    if (tipo == QStringLiteral("FII"))
    {
        std::unique_ptr<FundoImobiliario> fundo = std::make_unique<FundoImobiliario>();
        fundo->definirId(id);
        fundo->definirTicker(ticker);
        fundo->definirNomeEmpresa(nomeEmpresa);
        fundo->definirSetor(setor);
        fundo->definirDividendYield(dividendYield);
        fundo->definirValorPatrimonialPorCota(consulta.value(8).toDouble());
        fundo->definirSegmento(consulta.value(9).toString());
        return fundo;
    }

    std::unique_ptr<Acao> acao = std::make_unique<Acao>();
    acao->definirId(id);
    acao->definirTicker(ticker);
    acao->definirNomeEmpresa(nomeEmpresa);
    acao->definirSetor(setor);
    acao->definirPrecoLucro(consulta.value(5).toDouble());
    acao->definirValorMercado(consulta.value(6).toDouble());
    acao->definirDividendYield(dividendYield);
    return acao;
}

void RepositorioAtivo::vincularCampos(QSqlQuery& consulta, const Ativo& ativo)
{
    const Acao* acao = dynamic_cast<const Acao*>(&ativo);
    const FundoImobiliario* fundo = dynamic_cast<const FundoImobiliario*>(&ativo);

    consulta.bindValue(QStringLiteral(":ticker"), ativo.ticker().trimmed().toUpper());
    consulta.bindValue(QStringLiteral(":nome_empresa"), ativo.nomeEmpresa());
    consulta.bindValue(QStringLiteral(":setor"), ativo.setor());

    if (fundo != nullptr)
    {
        // Fundo imobiliario: preco_lucro e valor_mercado nao se aplicam.
        consulta.bindValue(QStringLiteral(":tipo"), QStringLiteral("FII"));
        consulta.bindValue(QStringLiteral(":preco_lucro"), QVariant());
        consulta.bindValue(QStringLiteral(":valor_mercado"), QVariant());
        consulta.bindValue(QStringLiteral(":dividend_yield"), fundo->dividendYield());
        consulta.bindValue(QStringLiteral(":valor_patrimonial_por_cota"),
                           fundo->valorPatrimonialPorCota());
        consulta.bindValue(QStringLiteral(":segmento"), fundo->segmento());
        return;
    }

    // Acao (tipo padrao): as colunas de FII ficam nulas.
    consulta.bindValue(QStringLiteral(":tipo"), QStringLiteral("ACAO"));
    consulta.bindValue(QStringLiteral(":preco_lucro"), acao != nullptr ? acao->precoLucro() : 0.0);
    consulta.bindValue(QStringLiteral(":valor_mercado"),
                       acao != nullptr ? acao->valorMercado() : 0.0);
    consulta.bindValue(QStringLiteral(":dividend_yield"),
                       acao != nullptr ? acao->dividendYield() : 0.0);
    consulta.bindValue(QStringLiteral(":valor_patrimonial_por_cota"), QVariant());
    consulta.bindValue(QStringLiteral(":segmento"), QVariant());
}

std::vector<std::unique_ptr<Ativo>> RepositorioAtivo::lerTodos(QSqlQuery& consulta)
{
    std::vector<std::unique_ptr<Ativo>> ativos;
    while (consulta.next())
    {
        std::unique_ptr<Ativo> ativo = montarAtivo(consulta);
        if (ativo != nullptr)
        {
            ativos.push_back(std::move(ativo));
        }
    }
    return ativos;
}

bool RepositorioAtivo::salvar(Ativo& ativo)
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(QStringLiteral(
        "INSERT INTO ativo (ticker, nome_empresa, setor, tipo, preco_lucro, valor_mercado, "
        "dividend_yield, valor_patrimonial_por_cota, segmento) "
        "VALUES (:ticker, :nome_empresa, :setor, :tipo, :preco_lucro, :valor_mercado, "
        ":dividend_yield, :valor_patrimonial_por_cota, :segmento)"));
    vincularCampos(consulta, ativo);

    if (!consulta.exec())
    {
        m_ultimoErro = consulta.lastError().text();
        return false;
    }

    ativo.definirId(consulta.lastInsertId().toLongLong());
    m_ultimoErro.clear();
    return true;
}

bool RepositorioAtivo::atualizar(const Ativo& ativo)
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(QStringLiteral(
        "UPDATE ativo SET ticker = :ticker, nome_empresa = :nome_empresa, setor = :setor, "
        "tipo = :tipo, preco_lucro = :preco_lucro, valor_mercado = :valor_mercado, "
        "dividend_yield = :dividend_yield, "
        "valor_patrimonial_por_cota = :valor_patrimonial_por_cota, segmento = :segmento "
        "WHERE id = :id"));
    vincularCampos(consulta, ativo);
    consulta.bindValue(QStringLiteral(":id"), ativo.id());

    if (!consulta.exec())
    {
        m_ultimoErro = consulta.lastError().text();
        return false;
    }

    m_ultimoErro.clear();
    return true;
}

bool RepositorioAtivo::remover(qint64 id)
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(QStringLiteral("DELETE FROM ativo WHERE id = :id"));
    consulta.bindValue(QStringLiteral(":id"), id);

    if (!consulta.exec())
    {
        m_ultimoErro = consulta.lastError().text();
        return false;
    }

    m_ultimoErro.clear();
    return true;
}

std::unique_ptr<Ativo> RepositorioAtivo::buscarPorId(qint64 id) const
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(QStringLiteral("SELECT %1 FROM ativo WHERE id = :id").arg(colunas()));
    consulta.bindValue(QStringLiteral(":id"), id);

    if (!consulta.exec())
    {
        m_ultimoErro = consulta.lastError().text();
        return nullptr;
    }

    m_ultimoErro.clear();
    if (!consulta.next())
    {
        return nullptr;
    }
    return montarAtivo(consulta);
}

std::unique_ptr<Ativo> RepositorioAtivo::buscarPorTicker(const QString& ticker) const
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(
        QStringLiteral("SELECT %1 FROM ativo WHERE UPPER(ticker) = UPPER(:ticker)").arg(colunas()));
    consulta.bindValue(QStringLiteral(":ticker"), ticker.trimmed());

    if (!consulta.exec())
    {
        m_ultimoErro = consulta.lastError().text();
        return nullptr;
    }

    m_ultimoErro.clear();
    if (!consulta.next())
    {
        return nullptr;
    }
    return montarAtivo(consulta);
}

std::vector<std::unique_ptr<Ativo>> RepositorioAtivo::listarTodos() const
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());

    if (!consulta.exec(QStringLiteral("SELECT %1 FROM ativo ORDER BY ticker").arg(colunas())))
    {
        m_ultimoErro = consulta.lastError().text();
        return std::vector<std::unique_ptr<Ativo>>();
    }

    m_ultimoErro.clear();
    return lerTodos(consulta);
}

std::vector<std::unique_ptr<Ativo>> RepositorioAtivo::buscar(const QString& termo) const
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(QStringLiteral("SELECT %1 FROM ativo "
                                    "WHERE UPPER(ticker) LIKE UPPER(:padraoTicker) "
                                    "OR UPPER(nome_empresa) LIKE UPPER(:padraoNome) "
                                    "ORDER BY ticker")
                         .arg(colunas()));

    // O padrao e montado como parametro; a consulta em si nunca e concatenada.
    const QString padrao = QStringLiteral("%") + termo.trimmed() + QStringLiteral("%");
    consulta.bindValue(QStringLiteral(":padraoTicker"), padrao);
    consulta.bindValue(QStringLiteral(":padraoNome"), padrao);

    if (!consulta.exec())
    {
        m_ultimoErro = consulta.lastError().text();
        return std::vector<std::unique_ptr<Ativo>>();
    }

    m_ultimoErro.clear();
    return lerTodos(consulta);
}

bool RepositorioAtivo::tickerEmUso(const QString& ticker, qint64 idIgnorado) const
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(QStringLiteral(
        "SELECT COUNT(*) FROM ativo WHERE UPPER(ticker) = UPPER(:ticker) AND id <> :idIgnorado"));
    consulta.bindValue(QStringLiteral(":ticker"), ticker.trimmed());
    consulta.bindValue(QStringLiteral(":idIgnorado"), idIgnorado);

    if (!consulta.exec() || !consulta.next())
    {
        m_ultimoErro = consulta.lastError().text();
        return false;
    }

    m_ultimoErro.clear();
    return consulta.value(0).toInt() > 0;
}

QString RepositorioAtivo::ultimoErro() const
{
    return m_ultimoErro;
}

}
