#include "analise/regrarsi.h"

#include <QDate>
#include <QLocale>
#include <QtGlobal>

#include "analise/calculadoraindicadores.h"

namespace analisador
{

RegraRsi::RegraRsi()
{
}

RegraRsi::~RegraRsi()
{
}

QString RegraRsi::nome() const
{
    return QStringLiteral("Indice de Forca Relativa");
}

QString RegraRsi::descricao() const
{
    const QLocale brasil(QLocale::Portuguese, QLocale::Brazil);
    return QString::fromUtf8(
        "Calcula o RSI de %1 períodos. Abaixo de %2 o ativo é considerado sobrevendido "
        "(compra); acima de %3, sobrecomprado (venda).")
        .arg(m_periodo)
        .arg(brasil.toString(m_limiteSobrevenda, 'f', 0), brasil.toString(m_limiteSobrecompra, 'f', 0));
}

void RegraRsi::configurar(double parametroPrincipal, double parametroSecundario)
{
    m_limiteSobrevenda = (parametroPrincipal > 0.0 && parametroPrincipal < 50.0) ? parametroPrincipal : 30.0;
    m_limiteSobrecompra = (parametroSecundario > 50.0 && parametroSecundario < 100.0) ? parametroSecundario : 70.0;
}

double RegraRsi::limiteSobrevenda() const
{
    return m_limiteSobrevenda;
}

double RegraRsi::limiteSobrecompra() const
{
    return m_limiteSobrecompra;
}

int RegraRsi::periodo() const
{
    return m_periodo;
}

std::optional<Recomendacao> RegraRsi::avaliar(const Ativo& ativo,
                                              const QVector<Cotacao>& historico) const
{
    const QVector<Indicador> serie = CalculadoraIndicadores::indiceForcaRelativa(historico, m_periodo);
    const std::optional<double> ultimo = CalculadoraIndicadores::ultimoValor(serie);
    if (!ultimo.has_value())
    {
        return std::nullopt;
    }

    const double rsi = ultimo.value();
    const QLocale brasil(QLocale::Portuguese, QLocale::Brazil);
    const QString rsiTexto = brasil.toString(rsi, 'f', 1);
    const QDate dataAtual = serie.last().data;
    const QString dataTexto = dataAtual.toString(QStringLiteral("dd/MM/yyyy"));

    double pontuacao = 0.0;
    QString justificativa;

    if (rsi < m_limiteSobrevenda)
    {
        // Quanto mais fundo na zona de sobrevenda, maior a conviccao de compra.
        const double intensidade = (m_limiteSobrevenda - rsi) / m_limiteSobrevenda;
        pontuacao = qBound(0.30, 0.30 + intensidade, 0.90);
        justificativa = QString::fromUtf8(
            "RSI de %1 períodos em %2 em %3, abaixo do limite de sobrevenda (%4): "
            "ativo descontado no curto prazo.")
            .arg(m_periodo)
            .arg(rsiTexto, dataTexto, brasil.toString(m_limiteSobrevenda, 'f', 0));
    }
    else if (rsi > m_limiteSobrecompra)
    {
        const double faixa = 100.0 - m_limiteSobrecompra;
        const double intensidade = faixa > 0.0 ? (rsi - m_limiteSobrecompra) / faixa : 0.0;
        pontuacao = -qBound(0.30, 0.30 + intensidade, 0.90);
        justificativa = QString::fromUtf8(
            "RSI de %1 períodos em %2 em %3, acima do limite de sobrecompra (%4): "
            "ativo esticado, risco de correção.")
            .arg(m_periodo)
            .arg(rsiTexto, dataTexto, brasil.toString(m_limiteSobrecompra, 'f', 0));
    }
    else
    {
        // Faixa neutra: a regra se abstem.
        return std::nullopt;
    }

    Recomendacao recomendacao;
    recomendacao.definirAtivoId(ativo.id());
    recomendacao.definirPontuacao(pontuacao);
    recomendacao.definirTipo(Recomendacao::tipoPorPontuacao(pontuacao));
    recomendacao.definirJustificativa(justificativa);
    recomendacao.definirRegraAplicada(nome());
    recomendacao.definirGeradaEm(dataAtual.isValid() ? dataAtual : QDate::currentDate());
    return recomendacao;
}

}
