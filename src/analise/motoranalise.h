#pragma once

// MotorAnalise - contexto do padrao Strategy.
//
// Guarda as estrategias ativas (RegraAnalise) e combina os pareceres em uma
// recomendacao final para o ativo. O motor nao conhece nenhuma regra concreta:
// as estrategias sao montadas pela fabrica comRegrasPadrao() a partir dos
// registros de regra_configurada, de modo que ligar, desligar ou reparametrizar
// uma regra e uma operacao de banco de dados, nao de codigo.

#include <memory>
#include <vector>

#include <QStringList>
#include <QVector>

#include "analise/regraanalise.h"
#include "dominio/regraconfigurada.h"

namespace analisador
{

class MotorAnalise
{
public:
    MotorAnalise();
    ~MotorAnalise();

    MotorAnalise(const MotorAnalise&) = delete;
    MotorAnalise& operator=(const MotorAnalise&) = delete;

    // Acrescenta uma estrategia ao motor (assume a propriedade do ponteiro).
    void registrarRegra(std::unique_ptr<RegraAnalise> regra);

    // Quantidade de estrategias registradas.
    int quantidadeDeRegras() const;

    // Nomes das estrategias registradas, na ordem de execucao.
    QStringList nomesDasRegras() const;

    // Executa todas as estrategias e agrega os pareceres em uma unica
    // recomendacao: a pontuacao final e a media das pontuacoes das regras que
    // opinaram e a justificativa lista cada parecer numerado. Quando nenhuma
    // regra opina, devolve recomendacao Neutro com pontuacao zero explicando
    // a ausencia de dados.
    Recomendacao analisar(const Ativo& ativo, const QVector<Cotacao>& historico) const;

    // Pareceres individuais, para a tela mostrar regra por regra.
    QVector<Recomendacao> avaliarRegrasIndividualmente(const Ativo& ativo,
                                                       const QVector<Cotacao>& historico) const;

    // Fabrica: monta o motor com as estrategias correspondentes as regras
    // ATIVAS recebidas, aplicando os parametros persistidos.
    static std::unique_ptr<MotorAnalise> comRegrasPadrao(const QVector<RegraConfigurada>& regras);

    // Fabrica de uma estrategia pelo nome canonico (o mesmo de nome()).
    // Devolve nullptr quando o nome nao corresponde a nenhuma estrategia conhecida.
    static std::unique_ptr<RegraAnalise> criarRegra(const QString& nomeRegra);

    // Configuracao padrao usada quando o banco ainda nao tem regras cadastradas.
    static QVector<RegraConfigurada> configuracaoPadrao();

private:
    std::vector<std::unique_ptr<RegraAnalise>> m_regras;
};

}
