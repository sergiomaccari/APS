#pragma once

// Grafico de linha simples do preco de fechamento, usado na tela de detalhe do
// ativo e em visualizacoes secundarias.

#include <QVector>
#include <QtCharts/QChartView>

#include "dominio/cotacao.h"

// Declaracao explicita: QChartView usa QChart apenas por ponteiro.
QT_FORWARD_DECLARE_CLASS(QChart)

namespace analisador
{

class GraficoLinha : public QChartView
{
    Q_OBJECT

public:
    explicit GraficoLinha(QWidget* pai = nullptr);

    void definirDados(const QString& titulo, const QVector<Cotacao>& cotacoes);
    void mostrarMensagem(const QString& mensagem);

private:
    // Ver GraficoCandlestick::trocarGrafico: setChart nao destroi o grafico
    // anterior, apenas libera a posse dele.
    void trocarGrafico(QChart* novo);
};

}
