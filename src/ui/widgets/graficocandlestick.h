#pragma once

// Grafico de candles (OHLC) com medias moveis sobrepostas, construido com o
// modulo Qt Charts. Recebe a serie de cotacoes ja ordenada por data.

#include <QVector>
#include <QtCharts/QChartView>

#include "dominio/cotacao.h"
#include "dominio/indicador.h"

// Declaracao explicita: QChartView usa QChart apenas por ponteiro.
QT_FORWARD_DECLARE_CLASS(QChart)

namespace analisador
{

class GraficoCandlestick : public QChartView
{
    Q_OBJECT

public:
    explicit GraficoCandlestick(QWidget* pai = nullptr);

    // Redesenha o grafico. As series de medias sao opcionais; quando vazias,
    // apenas os candles sao desenhados.
    void definirDados(const QString& titulo,
                      const QVector<Cotacao>& cotacoes,
                      const QVector<Indicador>& mediaCurta = QVector<Indicador>(),
                      const QVector<Indicador>& mediaLonga = QVector<Indicador>());

    // Mostra uma mensagem no lugar do grafico (sem dados suficientes, por exemplo).
    void mostrarMensagem(const QString& mensagem);

private:
    // Instala o grafico novo e APAGA o anterior: QChartView::setChart apenas
    // libera a posse do grafico antigo, sem destrui-lo - sem isso cada
    // redesenho do painel vazaria um QChart com todas as suas series.
    void trocarGrafico(QChart* novo);

    // Quantidade maxima de rotulos no eixo de datas, para nao poluir a leitura.
    static const int MAXIMO_ROTULOS_EIXO;
};

}
