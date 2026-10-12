#pragma once

// Ativo negociado na B3. Classe ABSTRATA: e a raiz da hierarquia de heranca do
// sistema (Acao e FundoImobiliario). O polimorfismo aparece em tipo() e em
// indiceValor(), usados pelas telas e pelas regras de analise sem que elas
// precisem saber qual e a subclasse concreta.

#include <QString>

namespace analisador
{

class Ativo
{
public:
    Ativo();
    Ativo(qint64 id, const QString& ticker, const QString& nomeEmpresa, const QString& setor);
    virtual ~Ativo();

    Ativo(const Ativo& outro) = default;
    Ativo& operator=(const Ativo& outro) = default;

    // Nome legivel do tipo do ativo: "Acao" ou "Fundo Imobiliario" (com acentos).
    virtual QString tipo() const = 0;

    // Metrica fundamentalista normalizada no intervalo [0, 100].
    // Quanto maior, mais atrativo o ativo do ponto de vista dos fundamentos.
    virtual double indiceValor() const = 0;

    // Texto explicativo dos fundamentos. Implementacao padrao generica que as
    // subclasses complementam.
    virtual QString descricaoFundamentos() const;

    qint64 id() const;
    QString ticker() const;
    QString nomeEmpresa() const;
    QString setor() const;

    void definirId(qint64 id);
    void definirTicker(const QString& ticker);
    void definirNomeEmpresa(const QString& nomeEmpresa);
    void definirSetor(const QString& setor);

    // Rotulo para listas e graficos, no formato "PETR4 - Petroleo Brasileiro S.A.".
    QString rotuloCompleto() const;

    // Verdadeiro quando o ticker e o nome da empresa estao preenchidos.
    bool valido() const;

protected:
    // Classifica um indice de valor em faixas textuais reaproveitadas pelas subclasses.
    static QString classificarIndice(double indice);

    qint64 m_id = 0;
    QString m_ticker;
    QString m_nomeEmpresa;
    QString m_setor;
};

}
