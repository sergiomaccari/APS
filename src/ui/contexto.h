#pragma once

// Contexto da aplicacao: dono dos repositorios e dos servicos.
//
// Centralizar a construcao aqui deixa explicita a injecao de dependencia entre
// as camadas (as telas recebem Contexto& e nunca criam repositorios) e garante
// a ordem de destruicao correta: os servicos guardam referencias para os
// repositorios, portanto os repositorios sao declarados primeiro.

#include "dominio/usuario.h"
#include "persistencia/repositorioalerta.h"
#include "persistencia/repositorioativo.h"
#include "persistencia/repositoriocarteira.h"
#include "persistencia/repositoriocotacao.h"
#include "persistencia/repositorioimportacao.h"
#include "persistencia/repositoriorecomendacao.h"
#include "persistencia/repositorioregra.h"
#include "persistencia/repositoriousuario.h"
#include "servicos/fonteyahoofinance.h"
#include "servicos/servicoalerta.h"
#include "servicos/servicoativo.h"
#include "servicos/servicoautenticacao.h"
#include "servicos/servicocarteira.h"
#include "servicos/servicoimportacao.h"
#include "servicos/servicorecomendacao.h"
#include "servicos/servicosincronizacao.h"

namespace analisador
{

class Contexto
{
public:
    Contexto();

    // Usuario autenticado na sessao atual.
    Usuario usuarioLogado;

    bool ehAdministrador() const;

    // Repositorios (declarados antes dos servicos - ordem de inicializacao).
    RepositorioUsuario repositorioUsuario;
    RepositorioAtivo repositorioAtivo;
    RepositorioCotacao repositorioCotacao;
    RepositorioCarteira repositorioCarteira;
    RepositorioAlerta repositorioAlerta;
    RepositorioRecomendacao repositorioRecomendacao;
    RepositorioRegra repositorioRegra;
    RepositorioImportacao repositorioImportacao;

    // Servicos.
    ServicoAutenticacao servicoAutenticacao;
    ServicoAtivo servicoAtivo;
    ServicoImportacao servicoImportacao;
    ServicoCarteira servicoCarteira;
    ServicoAlerta servicoAlerta;
    ServicoRecomendacao servicoRecomendacao;

    // Sincronizacao online (RF020): a fonte remota e declarada antes do servico,
    // que a recebe por referencia.
    FonteYahooFinance fonteCotacoes;
    ServicoSincronizacao servicoSincronizacao;
};

}
