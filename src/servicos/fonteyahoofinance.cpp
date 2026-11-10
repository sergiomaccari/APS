#include "servicos/fonteyahoofinance.h"

#include <algorithm>
#include <cmath>

#include <QDateTime>
#include <QEventLoop>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSslSocket>
#include <QTime>
#include <QUrl>

namespace analisador
{

const int FonteYahooFinance::TEMPO_LIMITE_MS = 15000;
const QString FonteYahooFinance::SUFIXO_B3 = QStringLiteral(".SA");

namespace
{

// Preco em reais: a API devolve varias casas decimais e a B3 negocia em centavos.
double arredondarCentavos(double valor)
{
    return std::round(valor * 100.0) / 100.0;
}

// Le o elemento "posicao" do vetor como numero. Devolve false quando o indice
// nao existe ou quando o valor e nulo - pregao sem negocio no papel.
bool numeroEm(const QJsonArray& vetor, int posicao, double* destino)
{
    if (posicao < 0 || posicao >= vetor.size())
    {
        return false;
    }
    const QJsonValue valor = vetor.at(posicao);
    if (!valor.isDouble())
    {
        return false;
    }
    *destino = valor.toDouble();
    return true;
}

// Extrai a mensagem de chart.error, que pode vir como objeto
// {"code": "...", "description": "..."} ou como texto simples.
QString descreverErroDaApi(const QJsonValue& erro)
{
    if (erro.isObject())
    {
        const QJsonObject objeto = erro.toObject();
        const QString descricao = objeto.value(QStringLiteral("description")).toString();
        if (!descricao.isEmpty())
        {
            return descricao;
        }
        const QString codigo = objeto.value(QStringLiteral("code")).toString();
        if (!codigo.isEmpty())
        {
            return codigo;
        }
    }
    else if (erro.isString() && !erro.toString().isEmpty())
    {
        return erro.toString();
    }
    return QString::fromUtf8("a fonte recusou a consulta sem detalhar o motivo.");
}

}

FonteYahooFinance::FonteYahooFinance()
    : m_rede(new QNetworkAccessManager())
{
}

FonteYahooFinance::~FonteYahooFinance()
{
    delete m_rede;
    m_rede = nullptr;
}

QString FonteYahooFinance::nome() const
{
    return QStringLiteral("Yahoo Finance");
}

QString FonteYahooFinance::montarUrl(const QString& ticker, const QDate& de, const QDate& ate)
{
    QString simbolo = ticker.trimmed().toUpper();
    if (!simbolo.endsWith(SUFIXO_B3))
    {
        simbolo += SUFIXO_B3;
    }

    // Datas em UTC: o pregao da B3 abre as 13h UTC, portanto o dia UTC coincide
    // com o dia do pregao e o resultado nao depende do fuso da maquina.
    const qint64 inicio = QDateTime(de, QTime(0, 0, 0), Qt::UTC).toSecsSinceEpoch();
    const qint64 fim = QDateTime(ate, QTime(23, 59, 59), Qt::UTC).toSecsSinceEpoch();

    return QStringLiteral("https://query1.finance.yahoo.com/v8/finance/chart/%1"
                          "?period1=%2&period2=%3&interval=1d")
        .arg(QString::fromUtf8(QUrl::toPercentEncoding(simbolo)))
        .arg(inicio)
        .arg(fim);
}

bool FonteYahooFinance::converterJson(const QByteArray& corpo,
                                      qint64 ativoId,
                                      QVector<Cotacao>* destino,
                                      QString* erro)
{
    const auto relatar = [erro](const QString& mensagem) {
        if (erro != nullptr)
        {
            *erro = mensagem;
        }
        return false;
    };

    if (destino == nullptr)
    {
        return relatar(QString::fromUtf8("Destino das cotações não informado."));
    }
    destino->clear();

    QJsonParseError falhaJson;
    const QJsonDocument documento = QJsonDocument::fromJson(corpo, &falhaJson);
    if (falhaJson.error != QJsonParseError::NoError || !documento.isObject())
    {
        return relatar(QString::fromUtf8("Resposta da fonte não é um JSON válido (%1).")
                           .arg(falhaJson.errorString()));
    }

    const QJsonObject grafico = documento.object().value(QStringLiteral("chart")).toObject();
    if (grafico.isEmpty())
    {
        return relatar(QString::fromUtf8("Resposta da fonte fora do formato esperado (sem \"chart\")."));
    }

    const QJsonValue erroDaApi = grafico.value(QStringLiteral("error"));
    if (!erroDaApi.isNull() && !erroDaApi.isUndefined())
    {
        return relatar(descreverErroDaApi(erroDaApi));
    }

    const QJsonArray resultados = grafico.value(QStringLiteral("result")).toArray();
    if (resultados.isEmpty())
    {
        return relatar(QString::fromUtf8("A fonte não devolveu nenhum resultado para o ativo."));
    }

    const QJsonObject serie = resultados.at(0).toObject();
    const QJsonArray instantes = serie.value(QStringLiteral("timestamp")).toArray();
    if (instantes.isEmpty())
    {
        // Intervalo sem pregao (feriado prolongado, ativo recem-listado):
        // ausencia de dados nao e erro.
        return true;
    }

    const QJsonArray cotacoesDaApi =
        serie.value(QStringLiteral("indicators")).toObject().value(QStringLiteral("quote")).toArray();
    if (cotacoesDaApi.isEmpty())
    {
        return relatar(QString::fromUtf8("Resposta da fonte sem o bloco de preços (indicators.quote)."));
    }

    const QJsonObject precos = cotacoesDaApi.at(0).toObject();
    const QJsonArray aberturas = precos.value(QStringLiteral("open")).toArray();
    const QJsonArray maximas = precos.value(QStringLiteral("high")).toArray();
    const QJsonArray minimas = precos.value(QStringLiteral("low")).toArray();
    const QJsonArray fechamentos = precos.value(QStringLiteral("close")).toArray();
    const QJsonArray volumes = precos.value(QStringLiteral("volume")).toArray();

    destino->reserve(instantes.size());
    for (int i = 0; i < instantes.size(); ++i)
    {
        const QJsonValue instante = instantes.at(i);
        if (!instante.isDouble())
        {
            continue;
        }

        double abertura = 0.0;
        double maxima = 0.0;
        double minima = 0.0;
        double fechamento = 0.0;
        double volume = 0.0;

        // Qualquer campo nulo invalida o candle inteiro: preferimos pular o
        // pregao a gravar um OHLCV incompleto.
        if (!numeroEm(aberturas, i, &abertura) || !numeroEm(maximas, i, &maxima)
            || !numeroEm(minimas, i, &minima) || !numeroEm(fechamentos, i, &fechamento)
            || !numeroEm(volumes, i, &volume))
        {
            continue;
        }

        const QDate data =
            QDateTime::fromSecsSinceEpoch(static_cast<qint64>(instante.toDouble()), Qt::UTC).date();

        Cotacao cotacao;
        cotacao.definirAtivoId(ativoId);
        cotacao.definirData(data);
        cotacao.definirAbertura(arredondarCentavos(abertura));
        cotacao.definirMaxima(arredondarCentavos(maxima));
        cotacao.definirMinima(arredondarCentavos(minima));
        cotacao.definirFechamento(arredondarCentavos(fechamento));
        cotacao.definirVolume(static_cast<qint64>(volume));

        // O arredondamento pode empurrar abertura/fechamento para fora da faixa
        // minima-maxima por um centavo; valida() barra esses casos.
        if (!cotacao.valida())
        {
            continue;
        }
        destino->append(cotacao);
    }

    std::sort(destino->begin(), destino->end());
    return true;
}

bool FonteYahooFinance::buscar(const QString& ticker,
                               qint64 ativoId,
                               const QDate& de,
                               const QDate& ate,
                               QVector<Cotacao>* destino,
                               QString* erro)
{
    const auto relatar = [erro](const QString& mensagem) {
        if (erro != nullptr)
        {
            *erro = mensagem;
        }
        return false;
    };

    if (destino == nullptr)
    {
        return relatar(QString::fromUtf8("Destino das cotações não informado."));
    }
    destino->clear();

    if (ticker.trimmed().isEmpty())
    {
        return relatar(QString::fromUtf8("Ativo sem ticker: impossível consultar a fonte online."));
    }
    if (!de.isValid() || !ate.isValid() || de > ate)
    {
        return relatar(QString::fromUtf8("Intervalo de datas inválido para a consulta online."));
    }

    // Sem backend TLS nao ha como falar https: avisar com clareza em vez de
    // deixar o Qt devolver um "unknown error" opaco.
    if (!QSslSocket::supportsSsl())
    {
        return relatar(QString::fromUtf8(
            "Este computador não tem backend TLS (OpenSSL) disponível para o Qt; "
            "a sincronização online exige HTTPS. Use a importação por CSV."));
    }

    QNetworkRequest requisicao{QUrl(montarUrl(ticker, de, ate))};
    // A API recusa clientes sem User-Agent de navegador.
    requisicao.setHeader(QNetworkRequest::UserAgentHeader,
                         QStringLiteral("Mozilla/5.0 (X11; Linux x86_64)"));
    requisicao.setTransferTimeout(TEMPO_LIMITE_MS);
    requisicao.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                            QNetworkRequest::NoLessSafeRedirectPolicy);

    // Requisicao sincrona: um QEventLoop local espera o "finished". A chamada
    // parte de um clique e ja roda com cursor de espera, entao bloquear aqui
    // mantem o servico linear e transacional.
    QNetworkReply* resposta = m_rede->get(requisicao);
    QEventLoop espera;
    QObject::connect(resposta, &QNetworkReply::finished, &espera, &QEventLoop::quit);
    espera.exec();

    const QNetworkReply::NetworkError falhaDeRede = resposta->error();
    const QString descricaoDaRede = resposta->errorString();
    const QByteArray corpo = resposta->readAll();
    resposta->deleteLater();

    if (falhaDeRede != QNetworkReply::NoError)
    {
        // Mesmo em HTTP 404 a API costuma devolver um JSON explicando o motivo;
        // essa mensagem e mais util que o texto generico do Qt.
        QVector<Cotacao> descartadas;
        QString erroDaApi;
        if (!corpo.isEmpty() && !converterJson(corpo, ativoId, &descartadas, &erroDaApi)
            && !erroDaApi.isEmpty())
        {
            return relatar(erroDaApi);
        }
        if (falhaDeRede == QNetworkReply::OperationCanceledError
            || falhaDeRede == QNetworkReply::TimeoutError)
        {
            return relatar(QString::fromUtf8("A fonte online não respondeu em %1 segundos.")
                               .arg(TEMPO_LIMITE_MS / 1000));
        }
        return relatar(QString::fromUtf8("Falha de rede ao consultar a fonte online: %1.")
                           .arg(descricaoDaRede));
    }

    return converterJson(corpo, ativoId, destino, erro);
}

}
