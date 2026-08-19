#include "ui/widgets/graficocandlestick.h"

#include <QtCharts/QCandlestickSeries>
#include <QtCharts/QCandlestickSet>
#include <QtCharts/QChart>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

#include <QtCharts/QAbstractAxis>
#include <QtCharts/QAbstractSeries>
#include <QtCharts/QLegend>

#include <QDateTime>

#include <QBrush>
#include <QPainter>
#include <QPen>

namespace analisador
{

const int GraficoCandlestick::MAXIMO_ROTULOS_EIXO = 12;

void aplicarTemaEscuro(QChart* grafico, QAbstractAxis* eixoHorizontal, QAbstractAxis* eixoVertical)
{
    // O grafico e um QGraphicsView: a folha de estilo da aplicacao nao alcanca o
    // seu interior, por isso as cores sao definidas aqui, na mao. O tema pronto
    // do Qt (ChartThemeDark) nao serve porque sobrescreveria o verde e o vermelho
    // dos candles.
    grafico->setBackgroundBrush(QBrush(QColor(QStringLiteral("#232a36"))));
    grafico->setBackgroundPen(Qt::NoPen);
    grafico->setPlotAreaBackgroundBrush(QBrush(QColor(QStringLiteral("#1f2530"))));
    grafico->setPlotAreaBackgroundVisible(true);
    grafico->setTitleBrush(QBrush(QColor(QStringLiteral("#ecf0f1"))));
    grafico->legend()->setLabelColor(QColor(QStringLiteral("#cfd8e3")));

    const QColor corTexto(QStringLiteral("#9fb0c0"));
    const QColor corGrade(QStringLiteral("#2f3743"));
    for (QAbstractAxis* eixo : {eixoHorizontal, eixoVertical})
    {
        if (eixo == nullptr)
        {
            continue;
        }
        eixo->setLabelsColor(corTexto);
        eixo->setTitleBrush(QBrush(corTexto));
        eixo->setGridLineColor(corGrade);
        eixo->setLinePenColor(corGrade);
    }
}

void GraficoCandlestick::trocarGrafico(QChart* novo)
{
    QChart* anterior = chart();
    setChart(novo);
    if (anterior != nullptr && anterior != novo)
    {
        delete anterior;
    }
}

GraficoCandlestick::GraficoCandlestick(QWidget* pai)
    : QChartView(pai)
{
    setRenderHint(QPainter::Antialiasing, true);
    setMinimumHeight(320);

    auto* grafico = new QChart();
    grafico->setTitle(QString::fromUtf8("Selecione um ativo"));
    grafico->legend()->setVisible(false);
    trocarGrafico(grafico);
}

void GraficoCandlestick::mostrarMensagem(const QString& mensagem)
{
    auto* grafico = new QChart();
    grafico->setTitle(mensagem);
    grafico->legend()->setVisible(false);
    trocarGrafico(grafico);
}

void GraficoCandlestick::definirDados(const QString& titulo,
                                      const QVector<Cotacao>& cotacoes,
                                      const QVector<Indicador>& mediaCurta,
                                      const QVector<Indicador>& mediaLonga)
{
    if (cotacoes.isEmpty())
    {
        mostrarMensagem(QString::fromUtf8("Sem cotações importadas para este ativo."));
        return;
    }

    auto* grafico = new QChart();
    grafico->setTitle(titulo);
    grafico->setAnimationOptions(QChart::SeriesAnimations);

    auto* candles = new QCandlestickSeries();
    candles->setName(QString::fromUtf8("Cotação diária"));
    candles->setIncreasingColor(QColor(QStringLiteral("#2ecc71")));
    candles->setDecreasingColor(QColor(QStringLiteral("#e74c3c")));
    candles->setBodyOutlineVisible(false);

    double minimoEixo = cotacoes.first().minima();
    double maximoEixo = cotacoes.first().maxima();

    for (const Cotacao& cotacao : cotacoes)
    {
        // O timestamp posiciona o candle no eixo de tempo; sem ele todos os
        // candles ficariam no instante zero e o grafico sairia vazio.
        const qint64 momento = cotacao.data().startOfDay().toMSecsSinceEpoch();
        candles->append(new QCandlestickSet(cotacao.abertura(),
                                           cotacao.maxima(),
                                           cotacao.minima(),
                                           cotacao.fechamento(),
                                           momento));

        minimoEixo = qMin(minimoEixo, cotacao.minima());
        maximoEixo = qMax(maximoEixo, cotacao.maxima());
    }
    grafico->addSeries(candles);

    // Eixo de tempo (e nao de categorias): o Qt escolhe sozinho quantas datas
    // cabem, o que mantem o eixo legivel em qualquer quantidade de pregoes, e as
    // medias moveis podem compartilhar o mesmo eixo usando as datas reais.
    auto* eixoDatas = new QDateTimeAxis();
    eixoDatas->setFormat(QStringLiteral("dd/MM/yy"));
    eixoDatas->setTickCount(qBound(3, static_cast<int>(cotacoes.size()) / 12 + 2, MAXIMO_ROTULOS_EIXO));
    eixoDatas->setLabelsAngle(-30);
    // Meio dia de folga em cada ponta para o primeiro e o ultimo candle nao
    // ficarem cortados pela borda da area de plotagem.
    const QDateTime primeiro = cotacoes.first().data().startOfDay().addSecs(-12 * 3600);
    const QDateTime ultimo = cotacoes.last().data().startOfDay().addSecs(12 * 3600);
    eixoDatas->setRange(primeiro, ultimo);
    grafico->addAxis(eixoDatas, Qt::AlignBottom);
    candles->attachAxis(eixoDatas);

    const auto adicionarMedia = [&](const QVector<Indicador>& media, const QColor& cor) {
        if (media.isEmpty())
        {
            return;
        }
        auto* linha = new QLineSeries();
        linha->setName(media.first().nome);
        QPen caneta(cor);
        caneta.setWidth(2);
        linha->setPen(caneta);

        for (const Indicador& ponto : media)
        {
            if (!ponto.valido())
            {
                continue;
            }
            linha->append(static_cast<qreal>(ponto.data.startOfDay().toMSecsSinceEpoch()), ponto.valor);
            minimoEixo = qMin(minimoEixo, ponto.valor);
            maximoEixo = qMax(maximoEixo, ponto.valor);
        }
        grafico->addSeries(linha);
        linha->attachAxis(eixoDatas);
    };

    adicionarMedia(mediaCurta, QColor(QStringLiteral("#f1c40f")));
    adicionarMedia(mediaLonga, QColor(QStringLiteral("#3498db")));

    const double folga = qMax(0.5, (maximoEixo - minimoEixo) * 0.08);
    auto* eixoValores = new QValueAxis();
    eixoValores->setRange(minimoEixo - folga, maximoEixo + folga);
    eixoValores->setLabelFormat(QStringLiteral("R$ %.2f"));
    eixoValores->setTitleText(QString::fromUtf8("Preço"));
    grafico->addAxis(eixoValores, Qt::AlignLeft);

    // Todas as series compartilham o eixo de precos.
    for (QAbstractSeries* serie : grafico->series())
    {
        serie->attachAxis(eixoValores);
    }

    grafico->legend()->setVisible(true);
    grafico->legend()->setAlignment(Qt::AlignBottom);
    aplicarTemaEscuro(grafico, eixoDatas, eixoValores);
    trocarGrafico(grafico);
}

}
