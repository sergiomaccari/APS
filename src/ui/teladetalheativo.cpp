#include "ui/teladetalheativo.h"

#include <memory>

#include <QComboBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLocale>
#include <QTableWidget>
#include <QTextBrowser>
#include <QVBoxLayout>

#include "dominio/ativo.h"
#include "dominio/cotacao.h"
#include "dominio/recomendacao.h"
#include "ui/contexto.h"
#include "ui/widgets/graficolinha.h"

namespace analisador
{

const int TelaDetalheAtivo::LINHAS_TABELA = 60;

TelaDetalheAtivo::TelaDetalheAtivo(Contexto& contexto, QWidget* pai)
    : QWidget(pai)
    , m_contexto(contexto)
{
    auto* titulo = new QLabel(QString::fromUtf8("Detalhes do Ativo"), this);
    titulo->setObjectName(QStringLiteral("tituloTela"));

    m_campoAtivo = new QComboBox(this);
    m_campoAtivo->setMinimumWidth(280);

    auto* linhaSeletor = new QHBoxLayout();
    linhaSeletor->addWidget(new QLabel(QStringLiteral("Ativo:"), this));
    linhaSeletor->addWidget(m_campoAtivo);
    linhaSeletor->addStretch();

    m_rotuloFundamentos = new QLabel(QString(), this);
    m_rotuloFundamentos->setWordWrap(true);
    m_rotuloFundamentos->setObjectName(QStringLiteral("textoAuxiliar"));

    auto* grupoFundamentos = new QGroupBox(QStringLiteral("Fundamentos"), this);
    auto* disposicaoFundamentos = new QVBoxLayout(grupoFundamentos);
    disposicaoFundamentos->addWidget(m_rotuloFundamentos);

    m_grafico = new GraficoLinha(this);

    m_tabelaCotacoes = new QTableWidget(this);
    m_tabelaCotacoes->setColumnCount(7);
    m_tabelaCotacoes->setHorizontalHeaderLabels(QStringList()
                                                << QStringLiteral("Data")
                                                << QStringLiteral("Abertura")
                                                << QString::fromUtf8("Máxima")
                                                << QString::fromUtf8("Mínima")
                                                << QStringLiteral("Fechamento")
                                                << QString::fromUtf8("Variação")
                                                << QStringLiteral("Volume"));
    m_tabelaCotacoes->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tabelaCotacoes->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tabelaCotacoes->setAlternatingRowColors(true);
    m_tabelaCotacoes->verticalHeader()->setVisible(false);
    m_tabelaCotacoes->horizontalHeader()->setStretchLastSection(true);

    m_pareceres = new QTextBrowser(this);
    m_pareceres->setPlaceholderText(
        QString::fromUtf8("O parecer de cada regra de análise aparece aqui."));

    auto* grupoPareceres = new QGroupBox(QString::fromUtf8("Pareceres por regra (Strategy)"), this);
    auto* disposicaoPareceres = new QVBoxLayout(grupoPareceres);
    disposicaoPareceres->addWidget(m_pareceres);

    auto* colunaDireita = new QVBoxLayout();
    colunaDireita->addWidget(m_tabelaCotacoes, 3);
    colunaDireita->addWidget(grupoPareceres, 2);

    auto* corpo = new QHBoxLayout();
    corpo->addWidget(m_grafico, 3);
    corpo->addLayout(colunaDireita, 2);

    auto* disposicao = new QVBoxLayout(this);
    disposicao->setContentsMargins(20, 18, 20, 18);
    disposicao->setSpacing(12);
    disposicao->addWidget(titulo);
    disposicao->addLayout(linhaSeletor);
    disposicao->addWidget(grupoFundamentos);
    disposicao->addLayout(corpo, 1);

    connect(m_campoAtivo, &QComboBox::currentIndexChanged, this, &TelaDetalheAtivo::redesenhar);

    atualizar();
}

qint64 TelaDetalheAtivo::ativoSelecionado() const
{
    if (m_campoAtivo->currentIndex() < 0)
    {
        return 0;
    }
    return m_campoAtivo->currentData().toLongLong();
}

void TelaDetalheAtivo::atualizar()
{
    const qint64 selecionadoAntes = ativoSelecionado();

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

void TelaDetalheAtivo::selecionarAtivo(qint64 ativoId)
{
    const int indice = m_campoAtivo->findData(ativoId);
    if (indice >= 0)
    {
        m_campoAtivo->setCurrentIndex(indice);
    }
}

void TelaDetalheAtivo::redesenhar()
{
    const QLocale brasil(QLocale::Portuguese, QLocale::Brazil);
    const qint64 ativoId = ativoSelecionado();

    if (ativoId <= 0)
    {
        m_rotuloFundamentos->setText(QString::fromUtf8("Nenhum ativo cadastrado."));
        m_grafico->mostrarMensagem(QString::fromUtf8("Sem ativo selecionado."));
        m_tabelaCotacoes->setRowCount(0);
        m_pareceres->clear();
        return;
    }

    const std::unique_ptr<Ativo> ativo = m_contexto.servicoAtivo.buscarPorId(ativoId);
    if (!ativo)
    {
        m_rotuloFundamentos->setText(QString::fromUtf8("Ativo não encontrado."));
        return;
    }

    m_rotuloFundamentos->setText(ativo->descricaoFundamentos());

    const QVector<Cotacao> historico = m_contexto.repositorioCotacao.listarPorAtivo(ativoId);
    m_grafico->definirDados(QStringLiteral("%1 - fechamento").arg(ativo->ticker()), historico);

    // A tabela mostra as mais recentes primeiro, ao contrario da serie do grafico.
    const int total = historico.size();
    const int quantidade = qMin(LINHAS_TABELA, total);
    m_tabelaCotacoes->setRowCount(quantidade);

    for (int i = 0; i < quantidade; ++i)
    {
        const Cotacao& cotacao = historico.at(total - 1 - i);
        m_tabelaCotacoes->setItem(i, 0,
            new QTableWidgetItem(cotacao.data().toString(QStringLiteral("dd/MM/yyyy"))));
        m_tabelaCotacoes->setItem(i, 1, new QTableWidgetItem(brasil.toString(cotacao.abertura(), 'f', 2)));
        m_tabelaCotacoes->setItem(i, 2, new QTableWidgetItem(brasil.toString(cotacao.maxima(), 'f', 2)));
        m_tabelaCotacoes->setItem(i, 3, new QTableWidgetItem(brasil.toString(cotacao.minima(), 'f', 2)));
        m_tabelaCotacoes->setItem(i, 4, new QTableWidgetItem(brasil.toString(cotacao.fechamento(), 'f', 2)));

        auto* celulaVariacao = new QTableWidgetItem(
            QStringLiteral("%1%").arg(brasil.toString(cotacao.variacaoPercentual(), 'f', 2)));
        celulaVariacao->setForeground(cotacao.ehAlta() ? QColor(QStringLiteral("#2ecc71"))
                                                       : QColor(QStringLiteral("#e74c3c")));
        m_tabelaCotacoes->setItem(i, 5, celulaVariacao);
        m_tabelaCotacoes->setItem(i, 6, new QTableWidgetItem(brasil.toString(cotacao.volume())));
    }

    const QVector<Recomendacao> pareceres = m_contexto.servicoRecomendacao.detalharPorRegra(ativoId);
    if (pareceres.isEmpty())
    {
        m_pareceres->setPlainText(QString::fromUtf8(
            "Nenhuma regra ativa conseguiu opinar sobre %1. Verifique se há cotações "
            "importadas e fundamentos cadastrados.").arg(ativo->ticker()));
        return;
    }

    QStringList texto;
    for (int i = 0; i < pareceres.size(); ++i)
    {
        const Recomendacao& parecer = pareceres.at(i);
        texto.append(QStringLiteral("%1) %2 - %3 (pontuação %4)\n%5")
                         .arg(i + 1)
                         .arg(parecer.regraAplicada(),
                              paraTexto(parecer.tipo()),
                              brasil.toString(parecer.pontuacao(), 'f', 2),
                              parecer.justificativa()));
    }
    m_pareceres->setPlainText(texto.join(QStringLiteral("\n\n")));
}

}
