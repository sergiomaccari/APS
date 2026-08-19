#include "ui/contexto.h"

namespace analisador
{

Contexto::Contexto()
    : servicoAutenticacao(repositorioUsuario)
    , servicoAtivo(repositorioAtivo)
    , servicoImportacao(repositorioCotacao, repositorioAtivo, repositorioImportacao)
    , servicoCarteira(repositorioCarteira, repositorioCotacao, repositorioAtivo)
    , servicoAlerta(repositorioAlerta, repositorioCotacao, repositorioAtivo)
    , servicoRecomendacao(repositorioRecomendacao, repositorioCotacao, repositorioAtivo, repositorioRegra)
{
}

bool Contexto::ehAdministrador() const
{
    return usuarioLogado.ehAdministrador();
}

}
