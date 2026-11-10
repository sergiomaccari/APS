#pragma once

// Tela da carteira do investidor: posicoes com preco medio, preco atual,
// lucro/prejuizo colorido e o resumo consolidado no rodape.

#include <QWidget>

class QLabel;
class QPushButton;
class QTableWidget;

namespace analisador
{

class Contexto;

class TelaCarteira : public QWidget
{
    Q_OBJECT

public:
    explicit TelaCarteira(Contexto& contexto, QWidget* pai = nullptr);

public slots:
    void atualizar();

private slots:
    void comprar();
    void vender();

private:
    // Garante que a carteira do usuario logado existe e guarda seu id.
    bool garantirCarteira();

    Contexto& m_contexto;
    qint64 m_carteiraId = 0;
    QTableWidget* m_tabela = nullptr;
    QLabel* m_rotuloResumo = nullptr;
    QLabel* m_rotuloMensagem = nullptr;
    QPushButton* m_botaoVender = nullptr;
};

}
