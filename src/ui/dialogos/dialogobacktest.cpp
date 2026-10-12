#include "ui/dialogos/dialogobacktest.h"

#include <memory>
#include <vector>

#include <QColor>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFont>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLocale>
#include <QPushButton>
#include <QSpinBox>
#include <QTableWidget>
#include <QVBoxLayout>

#include "analise/backtester.h"
#include "dominio/ativo.h"
#include "dominio/cotacao.h"
#include "dominio/regraconfigurada.h"
#include "ui/contexto.h"

namespace analisador
{

// Cores dos retornos, as mesmas usadas nas demais telas de resultado (REU002).
static const QString COR_POSITIVA = QStringLiteral("#2e7d32");
static const QString COR_NEGATIVA = QStringLiteral("#c62828");

// Formata uma fracao como percentual inteiro, ex.: 0,72 -> "72%".
static QString formatarTaxa(double fracao)
{
    const QLocale brasil(QLocale::Portuguese, QLocale::Brazil);
    return QStringLiteral("%1%").arg(brasil.toString(fracao * 100.0, 'f', 0));
}

// Formata uma fracao como percentual com sinal, ex.: 0,0341 -> "+3,41%".
static QString formatarRetorno(double fracao)
{
    const QLocale brasil(QLocale::Portuguese, QLocale::Brazil);
    const double percentual = fracao * 100.0;
    const QString sinal = percentual >= 0.0 ? QStringLiteral("+") : QString();
    return QStringLiteral("%1%2%").arg(sinal, brasil.toString(percentual, 'f', 2));
}

DialogoBacktest::DialogoBacktest(Contexto& contexto, QWidget* pai)
    : QDialog(pai)
    , m_contexto(contexto)
{
    setWindowTitle(QStringLiteral("Backtesting das regras"));
    setMinimumSize(760, 480);

    auto* titulo = new QLabel(QString::fromUtf8("Desempenho histórico das regras"), this);
    titulo->setObjectName(QStringLiteral("tituloDialogo"));

    auto* explicacao = new QLabel(
        QString::fromUtf8("As regras ativas são reaplicadas em cada pregão do histórico importado, "
                          "usando apenas os dados disponíveis naquela data. Cada sinal é conferido "
                          "contra o que o preço fez no horizonte escolhido."),
        this);
    explicacao->setWordWrap(true);
    explicacao->setObjectName(QStringLiteral("textoAuxiliar"));

    m_campoAtivo = new QComboBox(this);
    m_campoAtivo->addItem(QStringLiteral("Todos os ativos"), QVariant(static_cast<qint64>(0)));
    const std::vector<std::unique_ptr<Ativo>> ativos = m_contexto.repositorioAtivo.listarTodos();
    for (const std::unique_ptr<Ativo>& ativo : ativos)
    {
        if (!ativo)
        {
            continue;
        }
        m_campoAtivo->addItem(ativo->rotuloCompleto(), QVariant(ativo->id()));
    }

    m_campoHorizonte = new QSpinBox(this);
    m_campoHorizonte->setRange(1, 60);
    m_campoHorizonte->setValue(Backtester::HORIZONTE_PADRAO);
    m_campoHorizonte->setSuffix(QString::fromUtf8(" pregões"));
    m_campoHorizonte->setToolTip(QString::fromUtf8(
        "Quantos pregões à frente o resultado de cada sinal é medido."));

    m_botaoExecutar = new QPushButton(QStringLiteral("Executar backtesting"), this);
    m_botaoExecutar->setEnabled(!ativos.empty());

    auto* linhaFiltros = new QHBoxLayout();
    linhaFiltros->addWidget(new QLabel(QStringLiteral("Ativo:"), this));
    linhaFiltros->addWidget(m_campoAtivo, 1);
    linhaFiltros->addWidget(new QLabel(QStringLiteral("Horizonte:"), this));
    linhaFiltros->addWidget(m_campoHorizonte);
    linhaFiltros->addWidget(m_botaoExecutar);

    m_tabela = new QTableWidget(this);
    m_tabela->setColumnCount(6);
    m_tabela->setHorizontalHeaderLabels(QStringList()
                                        << QStringLiteral("Ativo")
                                        << QStringLiteral("Regra")
                                        << QStringLiteral("Sinais")
                                        << QStringLiteral("Acertos")
                                        << QStringLiteral("Taxa de acerto")
                                        << QString::fromUtf8("Retorno médio"));
    m_tabela->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tabela->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tabela->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tabela->setAlternatingRowColors(true);
    m_tabela->verticalHeader()->setVisible(false);
    m_tabela->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_tabela->horizontalHeader()->setStretchLastSection(true);

    m_rotuloMensagem = new QLabel(QString(), this);
    m_rotuloMensagem->setWordWrap(true);
    m_rotuloMensagem->setVisible(false);

    m_rotuloRodape = new QLabel(
        QString::fromUtf8("Backtesting reexecuta as regras em cada ponto do histórico e mede o "
                          "resultado no horizonte escolhido. Apoio ao estudo, não é garantia de "
                          "resultado futuro."),
        this);
    m_rotuloRodape->setWordWrap(true);
    m_rotuloRodape->setObjectName(QStringLiteral("textoAuxiliar"));
    QFont fonteRodape = m_rotuloRodape->font();
    fonteRodape.setPointSizeF(fonteRodape.pointSizeF() * 0.9);
    m_rotuloRodape->setFont(fonteRodape);

    auto* botoes = new QDialogButtonBox(QDialogButtonBox::Close, this);
    botoes->button(QDialogButtonBox::Close)->setText(QStringLiteral("Fechar"));

    auto* disposicao = new QVBoxLayout(this);
    disposicao->setContentsMargins(18, 16, 18, 16);
    disposicao->setSpacing(10);
    disposicao->addWidget(titulo);
    disposicao->addWidget(explicacao);
    disposicao->addLayout(linhaFiltros);
    disposicao->addWidget(m_tabela, 1);
    disposicao->addWidget(m_rotuloMensagem);
    disposicao->addWidget(m_rotuloRodape);
    disposicao->addWidget(botoes);

    connect(m_botaoExecutar, &QPushButton::clicked, this, &DialogoBacktest::executar);
    connect(botoes, &QDialogButtonBox::rejected, this, &QDialog::reject);

    if (ativos.empty())
    {
        mostrarMensagem(QString::fromUtf8(
            "Nenhum ativo cadastrado: cadastre ativos e importe cotações para usar o backtesting."));
    }
}

void DialogoBacktest::mostrarMensagem(const QString& mensagem)
{
    m_rotuloMensagem->setText(mensagem);
    m_rotuloMensagem->setVisible(!mensagem.isEmpty());
}

void DialogoBacktest::acrescentarLinha(const QString& ticker,
                                       const ResultadoBacktestRegra& apuracao,
                                       bool destacada)
{
    const int linha = m_tabela->rowCount();
    m_tabela->insertRow(linha);

    auto* celulaAtivo = new QTableWidgetItem(ticker);
    auto* celulaRegra = new QTableWidgetItem(apuracao.regra);
    auto* celulaSinais = new QTableWidgetItem(QString::number(apuracao.sinais));
    auto* celulaAcertos = new QTableWidgetItem(QString::number(apuracao.acertos));
    auto* celulaTaxa = new QTableWidgetItem(formatarTaxa(apuracao.taxaDeAcerto()));
    auto* celulaRetorno = new QTableWidgetItem(formatarRetorno(apuracao.retornoMedio()));

    celulaSinais->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    celulaAcertos->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    celulaTaxa->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    celulaRetorno->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

    // Verde no ganho, vermelho na perda: leitura imediata do resultado (REU002).
    if (apuracao.sinais > 0 && apuracao.retornoMedio() > 0.0)
    {
        celulaRetorno->setForeground(QColor(COR_POSITIVA));
    }
    else if (apuracao.sinais > 0 && apuracao.retornoMedio() < 0.0)
    {
        celulaRetorno->setForeground(QColor(COR_NEGATIVA));
    }

    m_tabela->setItem(linha, 0, celulaAtivo);
    m_tabela->setItem(linha, 1, celulaRegra);
    m_tabela->setItem(linha, 2, celulaSinais);
    m_tabela->setItem(linha, 3, celulaAcertos);
    m_tabela->setItem(linha, 4, celulaTaxa);
    m_tabela->setItem(linha, 5, celulaRetorno);

    if (!destacada)
    {
        return;
    }
    for (int coluna = 0; coluna < m_tabela->columnCount(); ++coluna)
    {
        QTableWidgetItem* celula = m_tabela->item(linha, coluna);
        if (celula == nullptr)
        {
            continue;
        }
        QFont fonte = celula->font();
        fonte.setBold(true);
        celula->setFont(fonte);
    }
}

void DialogoBacktest::acrescentarResultado(const ResultadoBacktest& resultado)
{
    // Ativo sem avaliacao possivel: uma linha unica com o motivo.
    if (resultado.pregoesAvaliados == 0)
    {
        const int linha = m_tabela->rowCount();
        m_tabela->insertRow(linha);
        m_tabela->setItem(linha, 0, new QTableWidgetItem(resultado.ticker));
        m_tabela->setItem(linha, 1, new QTableWidgetItem(resultado.observacao));
        m_tabela->setSpan(linha, 1, 1, m_tabela->columnCount() - 1);
        return;
    }

    for (const ResultadoBacktestRegra& apuracao : resultado.porRegra)
    {
        acrescentarLinha(resultado.ticker, apuracao, false);
    }
    acrescentarLinha(resultado.ticker, resultado.consolidado, true);
}

void DialogoBacktest::executar()
{
    const qint64 ativoSelecionado = m_campoAtivo->currentData().toLongLong();
    const int horizonte = m_campoHorizonte->value();

    const QVector<RegraConfigurada> regras = m_contexto.repositorioRegra.listarAtivas();
    const Backtester backtester(regras);

    m_tabela->clearSpans();
    m_tabela->setRowCount(0);

    int avaliados = 0;
    const std::vector<std::unique_ptr<Ativo>> ativos = m_contexto.repositorioAtivo.listarTodos();
    for (const std::unique_ptr<Ativo>& ativo : ativos)
    {
        if (!ativo)
        {
            continue;
        }
        if (ativoSelecionado > 0 && ativo->id() != ativoSelecionado)
        {
            continue;
        }

        const QVector<Cotacao> historico = m_contexto.repositorioCotacao.listarPorAtivo(ativo->id());
        acrescentarResultado(backtester.executar(*ativo, historico, horizonte));
        ++avaliados;
    }
    m_tabela->resizeRowsToContents();

    if (avaliados == 0)
    {
        mostrarMensagem(QString::fromUtf8("Nenhum ativo selecionado para o backtesting."));
        return;
    }
    mostrarMensagem(QString::fromUtf8(
        "%1 ativo(s) avaliado(s) com %2 regra(s) ativa(s) e horizonte de %3 pregão(ões).")
        .arg(avaliados)
        .arg(regras.size())
        .arg(horizonte));
}

}
