#pragma once

// Valor de um indicador tecnico calculado para uma data especifica.
// Exemplos de nome: "Media Movel 9", "Media Movel 21", "IFR 14", "Retorno Diario".

#include <QDate>
#include <QString>

namespace analisador
{

struct Indicador
{
    QString nome;
    QDate data;
    double valor = 0.0;

    Indicador() = default;

    Indicador(const QString& nomeIndicador, const QDate& dataIndicador, double valorIndicador)
        : nome(nomeIndicador)
        , data(dataIndicador)
        , valor(valorIndicador)
    {
    }

    // Verdadeiro quando o indicador tem nome e data utilizaveis.
    bool valido() const
    {
        return !nome.isEmpty() && data.isValid();
    }
};

}
