#include "persistencia/bancodedados.h"

#include <QDateTime>
#include <QFile>
#include <QSqlError>
#include <QSqlQuery>
#include <QTextStream>
#include <QVariant>

namespace analisador
{

const QString BancoDeDados::NOME_CONEXAO = QStringLiteral("analisador-b3");

BancoDeDados& BancoDeDados::instancia()
{
    static BancoDeDados unica;
    return unica;
}

BancoDeDados::BancoDeDados()
{
}

BancoDeDados::~BancoDeDados()
{
    fechar();
}

bool BancoDeDados::abrir(const QString& caminho)
{
    if (m_conexao.isOpen())
    {
        fechar();
    }

    if (QSqlDatabase::contains(NOME_CONEXAO))
    {
        m_conexao = QSqlDatabase::database(NOME_CONEXAO, false);
    }
    else
    {
        m_conexao = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), NOME_CONEXAO);
    }

    if (!m_conexao.isValid())
    {
        m_ultimoErro = QString::fromUtf8(
            "Driver QSQLITE não disponível. Instale o pacote libqt6sql6-sqlite.");
        return false;
    }

    m_conexao.setDatabaseName(caminho);
    if (!m_conexao.open())
    {
        m_ultimoErro = QString::fromUtf8("Não foi possível abrir o banco em %1: %2")
                           .arg(caminho, m_conexao.lastError().text());
        return false;
    }

    m_caminhoBanco = caminho;

    // Integridade referencial precisa ser habilitada em cada conexao do SQLite.
    QSqlQuery pragma(m_conexao);
    if (!pragma.exec(QStringLiteral("PRAGMA foreign_keys = ON")))
    {
        m_ultimoErro = QString::fromUtf8("Falha ao habilitar chaves estrangeiras: %1")
                           .arg(pragma.lastError().text());
        return false;
    }

    m_ultimoErro.clear();
    return true;
}

void BancoDeDados::fechar()
{
    if (m_conexao.isOpen())
    {
        m_conexao.close();
    }
}

bool BancoDeDados::estaAberto() const
{
    return m_conexao.isOpen();
}

QString BancoDeDados::caminhoBanco() const
{
    return m_caminhoBanco;
}

QSqlDatabase& BancoDeDados::conexao()
{
    return m_conexao;
}

bool BancoDeDados::garantirTabelaDeVersao()
{
    QSqlQuery consulta(m_conexao);
    const QString comando = QStringLiteral(
        "CREATE TABLE IF NOT EXISTS schema_versao ("
        "versao INTEGER PRIMARY KEY, "
        "descricao TEXT NOT NULL, "
        "aplicada_em TEXT NOT NULL)");
    if (!consulta.exec(comando))
    {
        m_ultimoErro = QString::fromUtf8("Falha ao criar schema_versao: %1")
                           .arg(consulta.lastError().text());
        return false;
    }
    return true;
}

int BancoDeDados::versaoAplicada() const
{
    if (!m_conexao.isOpen())
    {
        return 0;
    }
    QSqlQuery consulta(m_conexao);
    if (!consulta.exec(QStringLiteral("SELECT COALESCE(MAX(versao), 0) FROM schema_versao")))
    {
        return 0;
    }
    if (consulta.next())
    {
        return consulta.value(0).toInt();
    }
    return 0;
}

QStringList BancoDeDados::comandosDoRecurso(const QString& caminhoRecurso, QString* erro)
{
    QStringList comandos;
    QFile arquivo(caminhoRecurso);
    if (!arquivo.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        if (erro != nullptr)
        {
            *erro = QString::fromUtf8("Recurso de migração não encontrado: %1").arg(caminhoRecurso);
        }
        return comandos;
    }

    QTextStream fluxo(&arquivo);
    fluxo.setEncoding(QStringConverter::Utf8);

    QString conteudo;
    while (!fluxo.atEnd())
    {
        const QString linha = fluxo.readLine();
        const QString semEspacos = linha.trimmed();
        // Descarta comentarios de linha e linhas vazias antes de juntar o script.
        if (semEspacos.isEmpty() || semEspacos.startsWith(QStringLiteral("--")))
        {
            continue;
        }
        conteudo.append(linha);
        conteudo.append(QLatin1Char('\n'));
    }
    arquivo.close();

    const QStringList partes = conteudo.split(QLatin1Char(';'), Qt::SkipEmptyParts);
    for (const QString& parte : partes)
    {
        const QString comando = parte.trimmed();
        if (!comando.isEmpty())
        {
            comandos.append(comando);
        }
    }
    return comandos;
}

bool BancoDeDados::aplicarScript(const QString& caminhoRecurso)
{
    QString erroLeitura;
    const QStringList comandos = comandosDoRecurso(caminhoRecurso, &erroLeitura);
    if (comandos.isEmpty())
    {
        m_ultimoErro = erroLeitura.isEmpty()
            ? QString::fromUtf8("Script de migração vazio: %1").arg(caminhoRecurso)
            : erroLeitura;
        return false;
    }

    QSqlQuery consulta(m_conexao);
    for (const QString& comando : comandos)
    {
        if (!consulta.exec(comando))
        {
            m_ultimoErro = QString::fromUtf8("Erro ao aplicar %1: %2\nComando: %3")
                               .arg(caminhoRecurso, consulta.lastError().text(), comando.left(160));
            return false;
        }
    }
    return true;
}

bool BancoDeDados::registrarVersao(int versao, const QString& descricao)
{
    QSqlQuery consulta(m_conexao);
    consulta.prepare(QStringLiteral(
        "INSERT OR REPLACE INTO schema_versao (versao, descricao, aplicada_em) "
        "VALUES (:versao, :descricao, :aplicada_em)"));
    consulta.bindValue(QStringLiteral(":versao"), versao);
    consulta.bindValue(QStringLiteral(":descricao"), descricao);
    consulta.bindValue(QStringLiteral(":aplicada_em"),
                       QDateTime::currentDateTime().toString(Qt::ISODate));
    if (!consulta.exec())
    {
        m_ultimoErro = QString::fromUtf8("Falha ao registrar versão %1: %2")
                           .arg(versao)
                           .arg(consulta.lastError().text());
        return false;
    }
    return true;
}

bool BancoDeDados::executarMigracoes()
{
    if (!m_conexao.isOpen())
    {
        m_ultimoErro = QString::fromUtf8("Banco de dados não está aberto.");
        return false;
    }
    if (!garantirTabelaDeVersao())
    {
        return false;
    }

    struct Migracao
    {
        int versao;
        QString recurso;
        QString descricao;
    };

    const QVector<Migracao> migracoes = {
        {1, QStringLiteral(":/sql/001_esquema_inicial.sql"), QStringLiteral("Esquema inicial")},
        {2, QStringLiteral(":/sql/002_dados_semente.sql"), QStringLiteral("Dados de semente")}
    };

    const int versaoAtual = versaoAplicada();
    for (const Migracao& migracao : migracoes)
    {
        if (migracao.versao <= versaoAtual)
        {
            continue;
        }
        if (!iniciarTransacao())
        {
            return false;
        }
        if (!aplicarScript(migracao.recurso) || !registrarVersao(migracao.versao, migracao.descricao))
        {
            desfazer();
            return false;
        }
        if (!confirmar())
        {
            return false;
        }
    }

    m_ultimoErro.clear();
    return true;
}

bool BancoDeDados::iniciarTransacao()
{
    if (!m_conexao.transaction())
    {
        m_ultimoErro = QString::fromUtf8("Não foi possível iniciar a transação: %1")
                           .arg(m_conexao.lastError().text());
        return false;
    }
    return true;
}

bool BancoDeDados::confirmar()
{
    if (!m_conexao.commit())
    {
        m_ultimoErro = QString::fromUtf8("Não foi possível confirmar a transação: %1")
                           .arg(m_conexao.lastError().text());
        return false;
    }
    return true;
}

bool BancoDeDados::desfazer()
{
    if (!m_conexao.rollback())
    {
        m_ultimoErro = QString::fromUtf8("Não foi possível desfazer a transação: %1")
                           .arg(m_conexao.lastError().text());
        return false;
    }
    return true;
}

QString BancoDeDados::ultimoErro() const
{
    return m_ultimoErro;
}

}
