#include "ui/teladashboard.h"

#include <memory>
#include <optional>

#include <QComboBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLocale>
#include <QMessageBox>
#include <QPushButton>
#include <QTextBrowser>
#include <QVBoxLayout>

#include "analise/calculadoraindicadores.h"
#include "dominio/ativo.h"
#include "dominio/cotacao.h"
#include "dominio/recomendacao.h"
#include "ui/contexto.h"
#include "ui/widgets/cartaoindicador.h"
#include "ui/widgets/graficocandlestick.h"

namespace analisador
{

const int TelaDashboard::PERIODOS[] = {30, 90, 180, 365, 0};
const int TelaDashboard::QUANTIDADE_PERIODOS = 5;

TelaDashboard::TelaDashboard(Contexto& contexto, QWidget* pai)
    : QWidget(pai)
    , m_contexto(contexto)
{
    auto* titulo = new QLabel(QString::fromUtf8("Painel de Análise"), this);
    titulo->setObjectName(QStringLiteral("tituloTela"));

    m_campoAtivo = new QComboBox(this);
    m_campoAtivo->setMinimumWidth(280);

    m_campoPeriodo = new QComboBox(this);
    m_campoPeriodo->addItem(QString::fromUtf8("Últimos 30 pregões"));
    m_campoPeriodo->addItem(QString::fromUtf8("Últimos 90 pregões"));
    m_campoPeriodo->addItem(QString::fromUtf8("Últimos 180 pregões"));
    m_campoPeriodo->addItem(QString::fromUtf8("Último ano"));
    m_campoPeriodo->addItem(QString::fromUtf8("Série completa"));
    m_campoPeriodo->setCurrentIndex(1);

    m_botaoAnalisar = new QPushButton(QString::fromUtf8("Analisar agora"), this);
    m_botaoDetalhar = new QPushButton(QString::fromUtf8("Ver detalhes"), this);

    auto* linhaSeletores = new QHBoxLayout();
    linhaSeletores->addWidget(new QLabel(QStringLiteral("Ativo:"), this));
    linhaSeletores->addWidget(m_campoAtivo);
    linhaSeletores->addSpacing(12);
    linhaSeletores->addWidget(new QLabel(QString::fromUtf8("Período:"), this));
    linhaSeletores->addWidget(m_campoPeriodo);
    linhaSeletores->addStretch();
    linhaSeletores->addWidget(m_botaoDetalhar);
    linhaSeletores->addWidget(m_botaoAnalisar);

    m_cartaoFechamento = new CartaoIndicador(QString::fromUtf8("Último fechamento"), this);
    m_cartaoVariacao = new CartaoIndicador(QString::fromUtf8("Variação do período"), this);
    m_cartaoRsi = new CartaoIndicador(QStringLiteral("RSI (14)"), this);
    m_cartaoVolatilidade = new CartaoIndicador(QString::fromUtf8("Volatilidade anual"), this);

    auto* linhaCartoes = new QHBoxLayout();
    linhaCartoes->setSpacing(12);
    linhaCartoes->addWidget(m_cartaoFechamento);
    linhaCartoes->addWidget(m_cartaoVariacao);
    linhaCartoes->addWidget(m_cartaoRsi);
    linhaCartoes->addWidget(m_cartaoVolatilidade);
    linhaCartoes->addStretch();

    m_grafico = new GraficoCandlestick(this);

    m_selo = new QLabel(QString::fromUtf8("Sem recomendação"), this);
    m_selo->setObjectName(QStringLiteral("seloRecomendacao"));
    m_selo->setAlignment(Qt::AlignCenter);
    m_selo->setMinimumWidth(160);

    m_justificativa = new QTextBrowser(this);
    m_justificativa->setMinimumHeight(130);
    m_justificativa->setPlaceholderText(
        QString::fromUtf8("A justificativa da recomendação aparece aqui."));

    auto* linhaRecomendacao = new QHBoxLayout();
    linhaRecomendacao->addWidget(m_selo);
    linhaRecomendacao->addWidget(m_justificativa, 1);

    auto* grupoRecomendacao = new QGroupBox(QString::fromUtf8("Recomendação vigente"), this);
    auto* disposicaoRecomendacao = new QVBoxLayout(grupoRecomendacao);
    disposicaoRecomendacao->addLayout(linhaRecomendacao);

    auto* disposicao = new QVBoxLayout(this);
    disposicao->setContentsMargins(20, 18, 20, 18);
    disposicao->setSpacing(12);
    disposicao->addWidget(titulo);
    disposicao->addLayout(linhaSeletores);
    disposicao->addLayout(linhaCartoes);
    disposicao->addWidget(m_grafico, 1);
    disposicao->addWidget(grupoRecomendacao);

    connect(m_campoAtivo, &QComboBox::currentIndexChanged, this, &TelaDashboard::redesenhar);
    connect(m_campoPeriodo, &QComboBox::currentIndexChanged, this, &TelaDashboard::redesenhar);
    connect(m_botaoAnalisar, &QPushButton::clicked, this, &TelaDashboard::analisarAtivoAtual);
    connect(m_botaoDetalhar, &QPushButton::clicked, this, [this]() {
        const qint64 id = ativoSelecionado();
        if (id > 0)
        {
            emit detalharAtivo(id);
        }
    });

    atualizar();
}

qint64 TelaDashboard::ativoSelecionado() const
{
    if (m_campoAtivo->currentIndex() < 0)
    {
        return 0;
    }
    return m_campoAtivo->currentData().toLongLong();
}

int TelaDashboard::diasDoPeriodoSelecionado() const
{
    const int indice = m_campoPeriodo->currentIndex();
    if (indice < 0 || indice >= QUANTIDADE_PERIODOS)
    {
        return 0;
    }
    return PERIODOS[indice];
}

void TelaDashboard::atualizar()
{
    const qint64 selecionadoAntes = ativoSelecionado();

    // Bloquear os sinais evita redesenhar uma vez por item inserido.
    m_campoAtivo->blockSignals(true);
    m_campoAtivo->clear();
    const std::vector<std::unique_ptr<Ativo>> ativos = m_contexto.servicoAtivo.listarTodos();
    for (const std::unique_ptr<Ativo>& ativo : ativos)
    {
        if (ativo)
        {
            m_campoAtivo->addItem(ativo->rotuloCompleto(), ativo->id());
        }
    }
    const int indice = m_campoAtivo->findData(selecionadoAntes);
    m_campoAtivo->setCurrentIndex(indice >= 0 ? indice : (m_campoAtivo->count() > 0 ? 0 : -1));
    m_campoAtivo->blockSignals(false);

    redesenhar();
}

void TelaDashboard::selecionarAtivo(qint64 ativoId)
{
    const int indice = m_campoAtivo->findData(ativoId);
    if (indice >= 0)
    {
        m_campoAtivo->setCurrentIndex(indice);
    }
}

void TelaDashboard::redesenhar()
{
    const QLocale brasil(QLocale::Portuguese, QLocale::Brazil);
    const qint64 ativoId = ativoSelecionado();

    const bool temAtivo = ativoId > 0;
    m_botaoAnalisar->setEnabled(temAtivo);
    m_botaoDetalhar->setEnabled(temAtivo);

    if (!temAtivo)
    {
        m_grafico->mostrarMensagem(QString::fromUtf8("Nenhum ativo cadastrado."));
        m_cartaoFechamento->limpar();
        m_cartaoVariacao->limpar();
        m_cartaoRsi->limpar();
        m_cartaoVolatilidade->limpar();
        m_selo->setText(QString::fromUtf8("Sem ativo"));
        m_selo->setStyleSheet(QString());
        m_justificativa->clear();
        return;
    }

    const std::unique_ptr<Ativo> ativo = m_contexto.servicoAtivo.buscarPorId(ativoId);
    if (!ativo)
    {
        m_grafico->mostrarMensagem(QString::fromUtf8("Ativo não encontrado."));
        return;
    }

    const int dias = diasDoPeriodoSelecionado();
    const QVector<Cotacao> historico = m_contexto.repositorioCotacao.listarPorAtivo(ativoId, dias);

    // As medias sao calculadas sobre a janela exibida, para o grafico ficar coerente.
    const QVector<Indicador> mediaCurta = CalculadoraIndicadores::mediaMovelSimples(historico, 9);
    const QVector<Indicador> mediaLonga = CalculadoraIndicadores::mediaMovelSimples(historico, 21);

    m_grafico->definirDados(QStringLiteral("%1 - %2").arg(ativo->ticker(), ativo->nomeEmpresa()),
                            historico,
                            mediaCurta,
                            mediaLonga);

    const std::optional<double> fechamento = CalculadoraIndicadores::ultimoFechamento(historico);
    if (fechamento.has_value())
    {
        const QDate dataUltima = historico.last().data();
        m_cartaoFechamento->definirValor(
            QStringLiteral("R$ %1").arg(brasil.toString(fechamento.value(), 'f', 2)),
            dataUltima.toString(QStringLiteral("dd/MM/yyyy")));
    }
    else
    {
        m_cartaoFechamento->limpar();
    }

    // size() e qsizetype: sem a conversao explicita, qMin/qMax nao deduzem o tipo.
    const int totalCotacoes = static_cast<int>(historico.size());
    const int diasVariacao = dias > 0 ? dias : qMax(1, totalCotacoes - 1);
    const std::optional<double> variacao = CalculadoraIndicadores::variacaoPercentual(historico, diasVariacao);
    if (variacao.has_value())
    {
        m_cartaoVariacao->definirValor(
            QStringLiteral("%1%").arg(brasil.toString(variacao.value(), 'f', 2)));
        m_cartaoVariacao->definirDetalheComSinal(
            variacao.value(),
            variacao.value() >= 0.0 ? QString::fromUtf8("alta no período")
                                    : QString::fromUtf8("queda no período"));
    }
    else
    {
        m_cartaoVariacao->limpar();
    }

    const QVector<Indicador> serieRsi = CalculadoraIndicadores::indiceForcaRelativa(historico, 14);
    const std::optional<double> rsi = CalculadoraIndicadores::ultimoValor(serieRsi);
    if (rsi.has_value())
    {
        QString leitura = QString::fromUtf8("faixa neutra");
        if (rsi.value() < 30.0)
        {
            leitura = QString::fromUtf8("sobrevendido");
        }
        else if (rsi.value() > 70.0)
        {
            leitura = QString::fromUtf8("sobrecomprado");
        }
        m_cartaoRsi->definirValor(brasil.toString(rsi.value(), 'f', 1), leitura);
    }
    else
    {
        m_cartaoRsi->limpar();
    }

    const std::optional<double> volatilidade =
        CalculadoraIndicadores::volatilidadeAnualizada(historico, qMin(60, totalCotacoes));
    if (volatilidade.has_value())
    {
        m_cartaoVolatilidade->definirValor(
            QStringLiteral("%1%").arg(brasil.toString(volatilidade.value(), 'f', 1)),
            QString::fromUtf8("desvio anualizado"));
    }
    else
    {
        m_cartaoVolatilidade->limpar();
    }

    const std::optional<Recomendacao> vigente = m_contexto.servicoRecomendacao.vigenteDoAtivo(ativoId);
    if (!vigente.has_value())
    {
        m_selo->setText(QString::fromUtf8("Sem recomendação"));
        m_selo->setStyleSheet(QStringLiteral("background-color: #34495e; color: #ecf0f1;"
                                            "border-radius: 6px; padding: 12px; font-weight: bold;"));
        m_justificativa->setPlainText(QString::fromUtf8(
            "Nenhuma recomendação vigente para %1. Use \"Analisar agora\" para gerar uma "
            "com base nas regras ativas.").arg(ativo->ticker()));
        return;
    }

    const Recomendacao& recomendacao = vigente.value();
    QString cor = QStringLiteral("#7f8c8d");
    if (recomendacao.tipo() == TipoRecomendacao::Compra)
    {
        cor = QStringLiteral("#27ae60");
    }
    else if (recomendacao.tipo() == TipoRecomendacao::Venda)
    {
        cor = QStringLiteral("#c0392b");
    }

    m_selo->setText(QStringLiteral("%1\n%2")
                        .arg(paraTexto(recomendacao.tipo()),
                             brasil.toString(recomendacao.pontuacao(), 'f', 2)));
    m_selo->setStyleSheet(QStringLiteral("background-color: %1; color: white; border-radius: 6px;"
                                        "padding: 12px; font-size: 15px; font-weight: bold;")
                              .arg(cor));
    m_justificativa->setPlainText(recomendacao.justificativa());
}

void TelaDashboard::analisarAtivoAtual()
{
    const qint64 ativoId = ativoSelecionado();
    if (ativoId <= 0)
    {
        return;
    }

    const std::optional<Recomendacao> gerada = m_contexto.servicoRecomendacao.gerarParaAtivo(ativoId);
    if (!gerada.has_value())
    {
        QMessageBox::warning(this, QString::fromUtf8("Análise"),
                             m_contexto.servicoRecomendacao.ultimoErro());
        return;
    }
    redesenhar();
}

}
