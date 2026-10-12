#pragma once

// Regras de negocio do cadastro de ativos (acoes e fundos imobiliarios).
//
// Concentra as validacoes de ticker e setor para que a interface grafica nao
// precise conhece-las, e devolve os ativos como ponteiros para a classe base
// Ativo, preservando o polimorfismo.

#include <memory>
#include <vector>

#include <QString>

#include "dominio/ativo.h"
#include "persistencia/repositorioativo.h"

namespace analisador
{

class ServicoAtivo
{
public:
    explicit ServicoAtivo(RepositorioAtivo& repositorio);

    // Insere (id == 0) ou atualiza o ativo, validando ticker unico e campos obrigatorios.
    bool salvar(Ativo& ativo);

    bool remover(qint64 id);

    std::vector<std::unique_ptr<Ativo>> listarTodos() const;

    // Busca por ticker ou por parte do nome da empresa.
    std::vector<std::unique_ptr<Ativo>> buscar(const QString& termo) const;

    std::unique_ptr<Ativo> buscarPorId(qint64 id) const;
    std::unique_ptr<Ativo> buscarPorTicker(const QString& ticker) const;

    // Ticker valido: 4 a 6 caracteres, letras e digitos, sem espacos.
    static bool tickerValido(const QString& ticker);

    QString ultimoErro() const;

private:
    RepositorioAtivo& m_repositorio;
    QString m_ultimoErro;
};

}
