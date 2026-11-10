#pragma once

// Dialogo de compra e venda de ativos na carteira.
//
// O mesmo dialogo atende as duas operacoes: no modo Compra pede preco e data;
// no modo Venda limita a quantidade a posicao existente e dispensa o preco
// (a venda apenas reduz a posicao registrada).

#include <QDate>
#include <QDialog>
#include <QString>
#include <QVector>

class QComboBox;
class QDateEdit;
class QDoubleSpinBox;
class QLabel;
class QSpinBox;

namespace analisador
{

// Opcao de ativo oferecida no dialogo, com os limites de cada operacao.
struct OpcaoOperacao
{
    qint64 ativoId = 0;
    QString rotulo;
    double precoSugerido = 0.0;
    int quantidadeMaxima = 0;
};

class DialogoOperacao : public QDialog
{
    Q_OBJECT

public:
    enum class Modo
    {
        Compra,
        Venda
    };

    DialogoOperacao(Modo modo, const QVector<OpcaoOperacao>& opcoes, QWidget* pai = nullptr);

    qint64 ativoId() const;
    int quantidade() const;
    double preco() const;
    QDate data() const;

private slots:
    void ajustarLimitesPorAtivo();

private:
    Modo m_modo;
    QVector<OpcaoOperacao> m_opcoes;
    QComboBox* m_campoAtivo = nullptr;
    QSpinBox* m_campoQuantidade = nullptr;
    QDoubleSpinBox* m_campoPreco = nullptr;
    QDateEdit* m_campoData = nullptr;
    QLabel* m_rotuloPreco = nullptr;
    QLabel* m_rotuloData = nullptr;
    QLabel* m_rotuloLimite = nullptr;
};

}
