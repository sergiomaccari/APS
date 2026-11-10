// Repositórios do protótipo de telas: mesmas classes de src/persistencia/,
// mas sobre o estado em memória de DadosFixos (nenhum acesso a banco).
//
// Só estão definidos os construtores e os métodos que a interface chama
// diretamente (e buscarPorEmail/buscarPorTicker, usados por capturar-telas).
// Os serviços do protótipo leem dados() sem passar pelos repositórios. Nenhum
// repositório tem método virtual, então os métodos não usados podem ficar sem
// definição sem quebrar o link.

#include <algorithm>

#include "persistencia/repositorioalerta.h"
#include "persistencia/repositorioativo.h"
#include "persistencia/repositoriocarteira.h"
#include "persistencia/repositoriocotacao.h"
#include "persistencia/repositorioimportacao.h"
#include "persistencia/repositoriorecomendacao.h"
#include "persistencia/repositorioregra.h"
#include "persistencia/repositoriousuario.h"
#include "prototipo/dadosfixos.h"

namespace analisador
{

using prototipo::dados;

// --- Usuário -----------------------------------------------------------------

RepositorioUsuario::RepositorioUsuario()
{
}

std::optional<Usuario> RepositorioUsuario::buscarPorId(qint64 id) const
{
    const Usuario* usuario = prototipo::usuarioPorId(id);
    if (usuario == nullptr)
    {
        return std::nullopt;
    }
    return *usuario;
}

std::optional<Usuario> RepositorioUsuario::buscarPorEmail(const QString& email) const
{
    const Usuario* usuario = prototipo::usuarioPorEmail(email);
    if (usuario == nullptr)
    {
        return std::nullopt;
    }
    return *usuario;
}

// --- Ativo -------------------------------------------------------------------

RepositorioAtivo::RepositorioAtivo()
{
}

std::unique_ptr<Ativo> RepositorioAtivo::buscarPorTicker(const QString& ticker) const
{
    const Ativo* ativo = prototipo::ativoPorTicker(ticker);
    return ativo == nullptr ? nullptr : prototipo::clonarAtivo(*ativo);
}

std::vector<std::unique_ptr<Ativo>> RepositorioAtivo::listarTodos() const
{
    std::vector<std::unique_ptr<Ativo>> ativos;
    for (const auto& ativo : dados().ativos)
    {
        ativos.push_back(prototipo::clonarAtivo(*ativo));
    }
    // Mesmo ORDER BY ticker do repositório real.
    std::sort(ativos.begin(), ativos.end(), [](const auto& a, const auto& b) {
        return a->ticker() < b->ticker();
    });
    return ativos;
}

// --- Cotação -----------------------------------------------------------------

RepositorioCotacao::RepositorioCotacao()
{
}

QVector<Cotacao> RepositorioCotacao::listarPorAtivo(qint64 ativoId, int limite) const
{
    const QVector<Cotacao>& serie = prototipo::cotacoesDoAtivo(ativoId);
    if (limite > 0 && limite < serie.size())
    {
        return serie.mid(serie.size() - limite);
    }
    return serie;
}

std::optional<Cotacao> RepositorioCotacao::buscarUltima(qint64 ativoId) const
{
    const QVector<Cotacao>& serie = prototipo::cotacoesDoAtivo(ativoId);
    if (serie.isEmpty())
    {
        return std::nullopt;
    }
    return serie.last();
}

int RepositorioCotacao::contarPorAtivo(qint64 ativoId) const
{
    return prototipo::cotacoesDoAtivo(ativoId).size();
}

// --- Regra -------------------------------------------------------------------

RepositorioRegra::RepositorioRegra()
{
}

bool RepositorioRegra::atualizar(const RegraConfigurada& regra)
{
    RegraConfigurada* existente = prototipo::regraPorId(regra.id());
    if (existente == nullptr)
    {
        m_ultimoErro = QString::fromUtf8("Regra não encontrada.");
        return false;
    }
    *existente = regra;
    m_ultimoErro.clear();
    return true;
}

std::optional<RegraConfigurada> RepositorioRegra::buscarPorId(qint64 id) const
{
    const RegraConfigurada* regra = prototipo::regraPorId(id);
    if (regra == nullptr)
    {
        return std::nullopt;
    }
    return *regra;
}

QVector<RegraConfigurada> RepositorioRegra::listarTodas() const
{
    // Mesmo ORDER BY nome_regra do repositório real.
    QVector<RegraConfigurada> regras = dados().regras;
    std::sort(regras.begin(), regras.end(), [](const RegraConfigurada& a, const RegraConfigurada& b) {
        return a.nomeRegra() < b.nomeRegra();
    });
    return regras;
}

QVector<RegraConfigurada> RepositorioRegra::listarAtivas() const
{
    QVector<RegraConfigurada> ativas;
    for (const RegraConfigurada& regra : listarTodas())
    {
        if (regra.ativa())
        {
            ativas.append(regra);
        }
    }
    return ativas;
}

QString RepositorioRegra::ultimoErro() const
{
    return m_ultimoErro;
}

// --- Demais repositórios: só o construtor (a UI não os chama diretamente) ----

RepositorioCarteira::RepositorioCarteira()
{
}

RepositorioAlerta::RepositorioAlerta()
{
}

RepositorioRecomendacao::RepositorioRecomendacao()
{
}

RepositorioImportacao::RepositorioImportacao()
{
}

}
