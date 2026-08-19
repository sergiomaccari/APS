#pragma once

// Configuracao persistida de uma regra de analise (padrao Strategy).
//
// O administrador liga/desliga regras e ajusta seus parametros pela tela de
// regras; o MotorAnalise monta as estrategias concretas a partir desses
// registros (fabrica por nomeRegra). Assim as regras do sistema sao
// configuraveis sem recompilar.

#include <QString>

namespace analisador
{

class RegraConfigurada
{
public:
    RegraConfigurada();
    RegraConfigurada(qint64 id,
                     const QString& nomeRegra,
                     bool ativa,
                     double parametroPrincipal,
                     double parametroSecundario);

    qint64 id() const;
    QString nomeRegra() const;
    bool ativa() const;

    // Primeiro parametro da regra. Significado depende da estrategia:
    // media curta (cruzamento), limite de sobrevenda (RSI), DY minimo, P/L maximo.
    double parametroPrincipal() const;

    // Segundo parametro da regra: media longa, limite de sobrecompra,
    // DY considerado excelente ou P/L a partir do qual sugere venda.
    double parametroSecundario() const;

    void definirId(qint64 id);
    void definirNomeRegra(const QString& nomeRegra);
    void definirAtiva(bool ativa);
    void definirParametroPrincipal(double parametroPrincipal);
    void definirParametroSecundario(double parametroSecundario);

    // Verdadeiro quando ha nome de regra e os parametros nao sao negativos.
    bool valida() const;

    // Texto para listas: "Cruzamento de Medias (9 / 21) - ativa".
    QString descricao() const;

private:
    qint64 m_id = 0;
    QString m_nomeRegra;
    bool m_ativa = true;
    double m_parametroPrincipal = 0.0;
    double m_parametroSecundario = 0.0;
};

}
