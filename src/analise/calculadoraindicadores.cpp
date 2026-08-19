#include "analise/calculadoraindicadores.h"

#include <cmath>

#include <QString>
#include <QtGlobal>

namespace analisador
{

const int CalculadoraIndicadores::PREGOES_POR_ANO = 252;

QVector<Indicador> CalculadoraIndicadores::mediaMovelSimples(const QVector<Cotacao>& historico,
                                                             int periodo)
{
    QVector<Indicador> serie;
    if (periodo <= 0 || historico.size() < periodo)
    {
        return serie;
    }

    const QString nome = QStringLiteral("MMS %1").arg(periodo);
    serie.reserve(historico.size() - periodo + 1);

    double soma = 0.0;
    for (int i = 0; i < periodo; ++i)
    {
        soma += historico.at(i).fechamento();
    }
    serie.append(Indicador(nome, historico.at(periodo - 1).data(), soma / periodo));

    for (int i = periodo; i < historico.size(); ++i)
    {
        // Janela deslizante: entra o fechamento novo, sai o mais antigo.
        soma += historico.at(i).fechamento() - historico.at(i - periodo).fechamento();
        serie.append(Indicador(nome, historico.at(i).data(), soma / periodo));
    }
    return serie;
}

QVector<Indicador> CalculadoraIndicadores::mediaMovelExponencial(const QVector<Cotacao>& historico,
                                                                  int periodo)
{
    QVector<Indicador> serie;
    if (periodo <= 0 || historico.size() < periodo)
    {
        return serie;
    }

    const QString nome = QStringLiteral("MME %1").arg(periodo);
    const double k = 2.0 / (periodo + 1.0);
    serie.reserve(historico.size() - periodo + 1);

    double soma = 0.0;
    for (int i = 0; i < periodo; ++i)
    {
        soma += historico.at(i).fechamento();
    }
    double media = soma / periodo;
    serie.append(Indicador(nome, historico.at(periodo - 1).data(), media));

    for (int i = periodo; i < historico.size(); ++i)
    {
        media = historico.at(i).fechamento() * k + media * (1.0 - k);
        serie.append(Indicador(nome, historico.at(i).data(), media));
    }
    return serie;
}

QVector<Indicador> CalculadoraIndicadores::indiceForcaRelativa(const QVector<Cotacao>& historico,
                                                                int periodo)
{
    QVector<Indicador> serie;
    // Sao necessarios periodo + 1 fechamentos para obter "periodo" variacoes.
    if (periodo <= 0 || historico.size() <= periodo)
    {
        return serie;
    }

    const QString nome = QStringLiteral("RSI %1").arg(periodo);
    serie.reserve(historico.size() - periodo);

    double somaGanhos = 0.0;
    double somaPerdas = 0.0;
    for (int i = 1; i <= periodo; ++i)
    {
        const double diferenca = historico.at(i).fechamento() - historico.at(i - 1).fechamento();
        if (diferenca >= 0.0)
        {
            somaGanhos += diferenca;
        }
        else
        {
            somaPerdas += -diferenca;
        }
    }

    double mediaGanhos = somaGanhos / periodo;
    double mediaPerdas = somaPerdas / periodo;

    const auto calcularRsi = [](double ganhos, double perdas) {
        if (perdas <= 0.0)
        {
            // Sem perdas na janela: forca maxima (ou neutra quando tambem nao houve ganho).
            return ganhos > 0.0 ? 100.0 : 50.0;
        }
        const double forcaRelativa = ganhos / perdas;
        return 100.0 - (100.0 / (1.0 + forcaRelativa));
    };

    serie.append(Indicador(nome, historico.at(periodo).data(), calcularRsi(mediaGanhos, mediaPerdas)));

    for (int i = periodo + 1; i < historico.size(); ++i)
    {
        const double diferenca = historico.at(i).fechamento() - historico.at(i - 1).fechamento();
        const double ganho = diferenca > 0.0 ? diferenca : 0.0;
        const double perda = diferenca < 0.0 ? -diferenca : 0.0;

        // Suavizacao de Wilder.
        mediaGanhos = (mediaGanhos * (periodo - 1) + ganho) / periodo;
        mediaPerdas = (mediaPerdas * (periodo - 1) + perda) / periodo;

        serie.append(Indicador(nome, historico.at(i).data(), calcularRsi(mediaGanhos, mediaPerdas)));
    }
    return serie;
}

std::optional<double> CalculadoraIndicadores::variacaoPercentual(const QVector<Cotacao>& historico,
                                                                 int dias)
{
    if (historico.size() < 2 || dias <= 0)
    {
        return std::nullopt;
    }

    // size() devolve qsizetype (nao int): o recuo e calculado nesse tipo e
    // limitado ao inicio da serie quando "dias" excede o historico disponivel.
    qsizetype indiceInicial = historico.size() - 1 - dias;
    if (indiceInicial < 0)
    {
        indiceInicial = 0;
    }

    const double precoInicial = historico.at(indiceInicial).fechamento();
    const double precoFinal = historico.last().fechamento();
    if (qFuzzyIsNull(precoInicial))
    {
        return std::nullopt;
    }
    return ((precoFinal - precoInicial) / precoInicial) * 100.0;
}

std::optional<double> CalculadoraIndicadores::volatilidadeAnualizada(const QVector<Cotacao>& historico,
                                                                     int janela)
{
    if (janela < 2 || historico.size() < 3)
    {
        return std::nullopt;
    }

    const QVector<Cotacao> recorte = ultimasCotacoes(historico, janela + 1);
    if (recorte.size() < 3)
    {
        return std::nullopt;
    }

    QVector<double> retornos;
    retornos.reserve(recorte.size() - 1);
    for (int i = 1; i < recorte.size(); ++i)
    {
        const double anterior = recorte.at(i - 1).fechamento();
        if (qFuzzyIsNull(anterior))
        {
            continue;
        }
        retornos.append((recorte.at(i).fechamento() - anterior) / anterior);
    }
    if (retornos.size() < 2)
    {
        return std::nullopt;
    }

    double soma = 0.0;
    for (const double retorno : retornos)
    {
        soma += retorno;
    }
    const double media = soma / retornos.size();

    double somaQuadrados = 0.0;
    for (const double retorno : retornos)
    {
        somaQuadrados += (retorno - media) * (retorno - media);
    }
    // Desvio padrao amostral (n - 1).
    const double desvio = std::sqrt(somaQuadrados / (retornos.size() - 1));
    return desvio * std::sqrt(static_cast<double>(PREGOES_POR_ANO)) * 100.0;
}

std::optional<double> CalculadoraIndicadores::retornoAcumulado(const QVector<Cotacao>& historico)
{
    if (historico.size() < 2)
    {
        return std::nullopt;
    }
    const double precoInicial = historico.first().fechamento();
    if (qFuzzyIsNull(precoInicial))
    {
        return std::nullopt;
    }
    return ((historico.last().fechamento() - precoInicial) / precoInicial) * 100.0;
}

std::optional<double> CalculadoraIndicadores::ultimoFechamento(const QVector<Cotacao>& historico)
{
    if (historico.isEmpty())
    {
        return std::nullopt;
    }
    return historico.last().fechamento();
}

std::optional<double> CalculadoraIndicadores::ultimoValor(const QVector<Indicador>& serie)
{
    if (serie.isEmpty())
    {
        return std::nullopt;
    }
    return serie.last().valor;
}

QVector<Cotacao> CalculadoraIndicadores::ultimasCotacoes(const QVector<Cotacao>& historico,
                                                         int quantidade)
{
    if (quantidade <= 0 || historico.isEmpty())
    {
        return QVector<Cotacao>();
    }
    if (historico.size() <= quantidade)
    {
        return historico;
    }
    return historico.mid(historico.size() - quantidade, quantidade);
}

}
