#pragma once

// Painel principal do investidor: grafico de candles com medias moveis,
// cartoes com os indicadores do periodo e a recomendacao vigente do ativo com
// a justificativa completa.

#include <QWidget>

class QComboBox;
class QLabel;
class QPushButton;
class QTextBrowser;

namespace analisador
{

class Contexto;
class CartaoIndicador;
class GraficoCandlestick;

class TelaDashboard : public QWidget
{
    Q_OBJECT

public:
    explicit TelaDashboard(Contexto& contexto, QWidget* pai = nullptr);

public slots:
    // Recarrega a lista de ativos preservando a selecao e redesenha tudo.
    void atualizar();

    // Seleciona um ativo especifico (usado pela navegacao de outras telas).
    void selecionarAtivo(qint64 ativoId);

signals:
    // Pedido de abertura da tela de detalhe do ativo em exibicao.
    void detalharAtivo(qint64 ativoId);

private slots:
    void redesenhar();
    void analisarAtivoAtual();

private:
    // Dias de historico de cada opcao do seletor de periodo (0 = serie inteira).
    static const int PERIODOS[];
    static const int QUANTIDADE_PERIODOS;

    qint64 ativoSelecionado() const;
    int diasDoPeriodoSelecionado() const;

    Contexto& m_contexto;
    QComboBox* m_campoAtivo = nullptr;
    QComboBox* m_campoPeriodo = nullptr;
    QPushButton* m_botaoAnalisar = nullptr;
    QPushButton* m_botaoDetalhar = nullptr;
    GraficoCandlestick* m_grafico = nullptr;
    CartaoIndicador* m_cartaoFechamento = nullptr;
    CartaoIndicador* m_cartaoVariacao = nullptr;
    CartaoIndicador* m_cartaoRsi = nullptr;
    CartaoIndicador* m_cartaoVolatilidade = nullptr;
    QLabel* m_selo = nullptr;
    QTextBrowser* m_justificativa = nullptr;
};

}
