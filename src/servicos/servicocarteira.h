#pragma once

// Regras de negocio da carteira de investimentos.
//
// Compra soma quantidade e recalcula o preco medio ponderado; venda nunca
// permite quantidade maior do que a posicao existente. O resumo cruza as
// posicoes com a ultima cotacao de cada ativo para apurar lucro ou prejuizo.

#include <optional>

#include <QDate>
#include <QString>
#include <QVector>

#include "dominio/carteira.h"
#include "dominio/posicao.h"
#include "persistencia/repositorioativo.h"
#include "persistencia/repositoriocarteira.h"
#include "persistencia/repositoriocotacao.h"

namespace analisador
{

// Uma linha do resumo: a posicao mais os valores calculados de mercado.
struct LinhaResumoCarteira
{
    qint64 ativoId = 0;
    QString ticker;
    QString nomeAtivo;
    int quantidade = 0;
    double precoMedio = 0.0;
    double precoAtual = 0.0;
    double valorInvestido = 0.0;
    double valorAtual = 0.0;
    double lucroPrejuizo = 0.0;
    double rentabilidadePercentual = 0.0;
    bool temCotacao = false;
};

struct ResumoCarteira
{
    double valorInvestido = 0.0;
    double valorAtual = 0.0;
    double lucroPrejuizo = 0.0;
    double rentabilidadePercentual = 0.0;
    QVector<LinhaResumoCarteira> linhas;

    bool vazia() const;
};

class ServicoCarteira
{
public:
    ServicoCarteira(RepositorioCarteira& repositorioCarteira,
                    RepositorioCotacao& repositorioCotacao,
                    RepositorioAtivo& repositorioAtivo);

    // Devolve a carteira principal do usuario, criando-a na primeira vez.
    std::optional<Carteira> carteiraDoUsuario(qint64 usuarioId);

    // Compra: cria a posicao ou incorpora a compra recalculando o preco medio.
    bool comprar(qint64 carteiraId, qint64 ativoId, int quantidade, double preco, const QDate& data);

    // Venda parcial ou total. Quando a posicao zera, ela e removida.
    bool vender(qint64 carteiraId, qint64 ativoId, int quantidade);

    ResumoCarteira calcularResumo(qint64 carteiraId) const;

    QString ultimoErro() const;

private:
    RepositorioCarteira& m_repositorioCarteira;
    RepositorioCotacao& m_repositorioCotacao;
    RepositorioAtivo& m_repositorioAtivo;
    QString m_ultimoErro;
};

}
