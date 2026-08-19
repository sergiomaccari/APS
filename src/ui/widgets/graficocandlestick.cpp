#include "ui/widgets/graficocandlestick.h"

#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QCandlestickSeries>
#include <QtCharts/QCandlestickSet>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

#include <QBrush>
#include <QPen>

namespace analisador
{

const int GraficoCandlestick::MAXIMO_ROTULOS_EIXO = 12;

GraficoCandlestick::GraficoCandlestick(QWidget* pai)
    : QChartView(pai)
{
    setRenderHint(QPainter::Antialiasing, true);
    setMinimumHeight(320);

    auto* grafico = new QChart();
    grafico->setTitle(QString::fromUtf8("Selecione um ativo"));
    grafico->legend()->setVisible(false);
    setChart(grafico);
}

void GraficoCandlestick::mostrarMensagem(const QString& mensagem)
{
    auto* grafico = new QChart();
    grafico->setTitle(mensagem);
    grafico->legend()->setVisible(false);
    // setChart devolve a propriedade do grafico anterior ao QChartView, que o apaga.
    setChart(grafico);
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

    QStringList categorias;
    categorias.reserve(cotacoes.size());
    double minimoEixo = cotacoes.first().minima();
    double maximoEixo = cotacoes.first().maxima();

    for (const Cotacao& cotacao : cotacoes)
    {
        const QString rotulo = cotacao.data().toString(QStringLiteral("dd/MM"));
        categorias.append(rotulo);

        auto* candle = new QCandlestickSet(cotacao.abertura(),
                                           cotacao.maxima(),
                                           cotacao.minima(),
                                           cotacao.fechamento());
        candles->append(candle);

        minimoEixo = qMin(minimoEixo, cotacao.minima());
        maximoEixo = qMax(maximoEixo, cotacao.maxima());
    }
    grafico->addSeries(candles);

    // Series de medias moveis: alinhadas pelo fim, ja que terminam na mesma data
    // da ultima cotacao. Os primeiros pontos da serie de candles ficam sem media.
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

        const int deslocamento = cotacoes.size() - media.size();
        for (int i = 0; i < media.size(); ++i)
        {
            const int posicao = deslocamento + i;
            if (posicao < 0 || posicao >= cotacoes.size())
            {
                continue;
            }
            linha->append(static_cast<qreal>(posicao), media.at(i).valor);
            minimoEixo = qMin(minimoEixo, media.at(i).valor);
            maximoEixo = qMax(maximoEixo, media.at(i).valor);
        }
        grafico->addSeries(linha);
    };

    adicionarMedia(mediaCurta, QColor(QStringLiteral("#f1c40f")));
    adicionarMedia(mediaLonga, QColor(QStringLiteral("#3498db")));

    auto* eixoDatas = new QBarCategoryAxis();
    eixoDatas->append(categorias);
    // Com muitas datas, mostrar todos os rotulos deixa o eixo ilegivel.
    if (categorias.size() > MAXIMO_ROTULOS_EIXO)
    {
        eixoDatas->setLabelsAngle(-60);
    }
    grafico->addAxis(eixoDatas, Qt::AlignBottom);

    const double folga = qMax(0.5, (maximoEixo - minimoEixo) * 0.08);
    auto* eixoValores = new QValueAxis();
    eixoValores->setRange(minimoEixo - folga, maximoEixo + folga);
    eixoValores->setLabelFormat(QStringLiteral("R$ %.2f"));
    eixoValores->setTitleText(QString::fromUtf8("Preço"));
    grafico->addAxis(eixoValores, Qt::AlignLeft);

    candles->attachAxis(eixoDatas);
    candles->attachAxis(eixoValores);

    // As linhas de media usam eixo numerico no lugar das categorias, por isso
    // recebem um eixo horizontal proprio, invisivel e alinhado com os candles.
    for (QAbstractSeries* serie : grafico->series())
    {
        auto* linha = qobject_cast<QLineSeries*>(serie);
        if (linha == nullptr)
        {
            continue;
        }
        auto* eixoIndice = new QValueAxis();
        eixoIndice->setRange(0.0, static_cast<double>(categorias.size() - 1));
        eixoIndice->setVisible(false);
        grafico->addAxis(eixoIndice, Qt::AlignTop);
        linha->attachAxis(eixoIndice);
        linha->attachAxis(eixoValores);
    }

    grafico->legend()->setVisible(true);
    grafico->legend()->setAlignment(Qt::AlignBottom);
    setChart(grafico);
}

}
