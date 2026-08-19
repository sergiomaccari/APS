#include "ui/widgets/graficolinha.h"

#include <QtCharts/QChart>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

#include <QDateTime>
#include <QPen>

namespace analisador
{

GraficoLinha::GraficoLinha(QWidget* pai)
    : QChartView(pai)
{
    setRenderHint(QPainter::Antialiasing, true);
    setMinimumHeight(260);

    auto* grafico = new QChart();
    grafico->setTitle(QString::fromUtf8("Sem dados"));
    grafico->legend()->setVisible(false);
    setChart(grafico);
}

void GraficoLinha::mostrarMensagem(const QString& mensagem)
{
    auto* grafico = new QChart();
    grafico->setTitle(mensagem);
    grafico->legend()->setVisible(false);
    setChart(grafico);
}

void GraficoLinha::definirDados(const QString& titulo, const QVector<Cotacao>& cotacoes)
{
    if (cotacoes.size() < 2)
    {
        mostrarMensagem(QString::fromUtf8("São necessárias ao menos duas cotações para o gráfico."));
        return;
    }

    auto* grafico = new QChart();
    grafico->setTitle(titulo);

    auto* serie = new QLineSeries();
    serie->setName(QString::fromUtf8("Fechamento"));
    QPen caneta(QColor(QStringLiteral("#3498db")));
    caneta.setWidth(2);
    serie->setPen(caneta);

    double minimo = cotacoes.first().fechamento();
    double maximo = minimo;
    for (const Cotacao& cotacao : cotacoes)
    {
        const QDateTime momento(cotacao.data(), QTime(12, 0));
        serie->append(static_cast<qreal>(momento.toMSecsSinceEpoch()), cotacao.fechamento());
        minimo = qMin(minimo, cotacao.fechamento());
        maximo = qMax(maximo, cotacao.fechamento());
    }
    grafico->addSeries(serie);

    auto* eixoDatas = new QDateTimeAxis();
    eixoDatas->setFormat(QStringLiteral("dd/MM/yy"));
    eixoDatas->setTickCount(8);
    eixoDatas->setTitleText(QStringLiteral("Data"));
    grafico->addAxis(eixoDatas, Qt::AlignBottom);
    serie->attachAxis(eixoDatas);

    const double folga = qMax(0.5, (maximo - minimo) * 0.08);
    auto* eixoValores = new QValueAxis();
    eixoValores->setRange(minimo - folga, maximo + folga);
    eixoValores->setLabelFormat(QStringLiteral("R$ %.2f"));
    grafico->addAxis(eixoValores, Qt::AlignLeft);
    serie->attachAxis(eixoValores);

    grafico->legend()->setVisible(false);
    setChart(grafico);
}

}
