#pragma once

// Tela das recomendacoes vigentes: lista consolidada por ativo, com o
// detalhamento do parecer de cada regra do padrao Strategy no painel lateral.

#include <QWidget>

class QLabel;
class QPushButton;
class QTableWidget;
class QTextBrowser;

namespace analisador
{

class Contexto;

class TelaRecomendacoes : public QWidget
{
    Q_OBJECT

public:
    explicit TelaRecomendacoes(Contexto& contexto, QWidget* pai = nullptr);

public slots:
    void atualizar();

signals:
    // Pedido de abertura do painel de analise no ativo escolhido.
    void abrirAtivo(qint64 ativoId);

private slots:
    void gerarParaTodos();
    void mostrarDetalhesDaSelecao();

private:
    qint64 ativoSelecionado() const;

    Contexto& m_contexto;
    QTableWidget* m_tabela = nullptr;
    QTextBrowser* m_detalhes = nullptr;
    QLabel* m_rotuloMensagem = nullptr;
    QPushButton* m_botaoAbrir = nullptr;
};

}
