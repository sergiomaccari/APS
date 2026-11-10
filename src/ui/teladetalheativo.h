#pragma once

// Tela de detalhe do ativo: fundamentos cadastrados, grafico de linha do
// fechamento, tabela das cotacoes e o parecer de cada regra de analise
// (detalhamento do padrao Strategy, regra por regra).

#include <QWidget>

class QComboBox;
class QLabel;
class QTableWidget;
class QTextBrowser;

namespace analisador
{

class Contexto;
class GraficoLinha;

class TelaDetalheAtivo : public QWidget
{
    Q_OBJECT

public:
    explicit TelaDetalheAtivo(Contexto& contexto, QWidget* pai = nullptr);

public slots:
    void atualizar();
    void selecionarAtivo(qint64 ativoId);

private slots:
    void redesenhar();

private:
    // Cotacoes exibidas na tabela (as mais recentes primeiro).
    static const int LINHAS_TABELA;

    qint64 ativoSelecionado() const;

    Contexto& m_contexto;
    QComboBox* m_campoAtivo = nullptr;
    QLabel* m_rotuloFundamentos = nullptr;
    GraficoLinha* m_grafico = nullptr;
    QTableWidget* m_tabelaCotacoes = nullptr;
    QTextBrowser* m_pareceres = nullptr;
};

}
