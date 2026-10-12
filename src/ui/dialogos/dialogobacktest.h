#pragma once

// Dialogo de backtesting das regras de analise (RF021).
//
// Aberto pela tela de regras, permite escolher um ativo (ou todos) e o
// horizonte de avaliacao e mostra, regra por regra, quantos sinais a
// configuracao atual teria emitido no historico importado, quantos acertaram a
// direcao e qual o retorno medio. O calculo fica no Backtester; aqui so ha
// coleta dos dados nos repositorios e apresentacao.

#include <QDialog>

class QComboBox;
class QLabel;
class QPushButton;
class QSpinBox;
class QTableWidget;

namespace analisador
{

class Contexto;
struct ResultadoBacktest;
struct ResultadoBacktestRegra;

class DialogoBacktest : public QDialog
{
    Q_OBJECT

public:
    explicit DialogoBacktest(Contexto& contexto, QWidget* pai = nullptr);

private slots:
    void executar();

private:
    // Acrescenta a tabela as linhas de um ativo avaliado.
    void acrescentarResultado(const ResultadoBacktest& resultado);

    // Monta uma linha da tabela; "destacada" deixa o texto em negrito
    // (usado na linha do parecer consolidado).
    void acrescentarLinha(const QString& ticker,
                          const ResultadoBacktestRegra& apuracao,
                          bool destacada);

    void mostrarMensagem(const QString& mensagem);

    Contexto& m_contexto;
    QComboBox* m_campoAtivo = nullptr;
    QSpinBox* m_campoHorizonte = nullptr;
    QPushButton* m_botaoExecutar = nullptr;
    QTableWidget* m_tabela = nullptr;
    QLabel* m_rotuloMensagem = nullptr;
    QLabel* m_rotuloRodape = nullptr;
};

}
