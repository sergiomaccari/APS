#pragma once

// Cartao compacto para exibir um indicador no painel: titulo, valor destacado e
// uma linha de detalhe que pode ser colorida conforme o sinal do numero.

#include <QFrame>

class QLabel;

namespace analisador
{

class CartaoIndicador : public QFrame
{
    Q_OBJECT

public:
    explicit CartaoIndicador(const QString& titulo, QWidget* pai = nullptr);

    // Define o valor principal e, opcionalmente, o detalhe abaixo dele.
    void definirValor(const QString& valor, const QString& detalhe = QString());

    // Colore o detalhe: positivo em verde, negativo em vermelho, zero em cinza.
    void definirDetalheComSinal(double valor, const QString& texto);

    void limpar();

private:
    QLabel* m_rotuloTitulo = nullptr;
    QLabel* m_rotuloValor = nullptr;
    QLabel* m_rotuloDetalhe = nullptr;
};

}
