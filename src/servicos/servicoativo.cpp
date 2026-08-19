#include "servicos/servicoativo.h"

#include <QRegularExpression>

namespace analisador
{

ServicoAtivo::ServicoAtivo(RepositorioAtivo& repositorio)
    : m_repositorio(repositorio)
{
}

bool ServicoAtivo::tickerValido(const QString& ticker)
{
    static const QRegularExpression padrao(QStringLiteral("^[A-Z0-9]{4,6}$"));
    return padrao.match(ticker.trimmed().toUpper()).hasMatch();
}

bool ServicoAtivo::salvar(Ativo& ativo)
{
    m_ultimoErro.clear();

    if (!tickerValido(ativo.ticker()))
    {
        m_ultimoErro = QString::fromUtf8(
            "Ticker inválido: use de 4 a 6 caracteres entre letras e números (ex.: PETR4).");
        return false;
    }
    if (ativo.nomeEmpresa().trimmed().isEmpty())
    {
        m_ultimoErro = QString::fromUtf8("Informe o nome da empresa ou do fundo.");
        return false;
    }
    if (ativo.setor().trimmed().isEmpty())
    {
        m_ultimoErro = QString::fromUtf8("Informe o setor do ativo.");
        return false;
    }
    if (m_repositorio.tickerEmUso(ativo.ticker(), ativo.id()))
    {
        m_ultimoErro = QString::fromUtf8("Já existe um ativo cadastrado com o ticker %1.")
                           .arg(ativo.ticker());
        return false;
    }

    const bool ok = ativo.id() == 0 ? m_repositorio.salvar(ativo) : m_repositorio.atualizar(ativo);
    if (!ok)
    {
        m_ultimoErro = m_repositorio.ultimoErro();
    }
    return ok;
}

bool ServicoAtivo::remover(qint64 id)
{
    m_ultimoErro.clear();
    if (id <= 0)
    {
        m_ultimoErro = QString::fromUtf8("Selecione um ativo válido para remover.");
        return false;
    }
    if (!m_repositorio.remover(id))
    {
        m_ultimoErro = m_repositorio.ultimoErro();
        return false;
    }
    return true;
}

std::vector<std::unique_ptr<Ativo>> ServicoAtivo::listarTodos() const
{
    return m_repositorio.listarTodos();
}

std::vector<std::unique_ptr<Ativo>> ServicoAtivo::buscar(const QString& termo) const
{
    if (termo.trimmed().isEmpty())
    {
        return m_repositorio.listarTodos();
    }
    return m_repositorio.buscar(termo.trimmed());
}

std::unique_ptr<Ativo> ServicoAtivo::buscarPorId(qint64 id) const
{
    return m_repositorio.buscarPorId(id);
}

std::unique_ptr<Ativo> ServicoAtivo::buscarPorTicker(const QString& ticker) const
{
    return m_repositorio.buscarPorTicker(ticker.trimmed().toUpper());
}

QString ServicoAtivo::ultimoErro() const
{
    return m_ultimoErro;
}

}
