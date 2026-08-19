#include "ui/telarecomendacoes.h"

#include <memory>

#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLocale>
#include <QPushButton>
#include <QTableWidget>
#include <QTextBrowser>
#include <QVBoxLayout>

#include "dominio/ativo.h"
#include "dominio/recomendacao.h"
#include "ui/contexto.h"

namespace analisador
{

TelaRecomendacoes::TelaRecomendacoes(Contexto& contexto, QWidget* pai)
    : QWidget(pai)
    , m_contexto(contexto)
{
    auto* titulo = new QLabel(QString::fromUtf8("Recomendações"), this);
    titulo->setObjectName(QStringLiteral("tituloTela"));

    auto* explicacao = new QLabel(
        QString::fromUtf8("Cada recomendação é o parecer consolidado das regras ativas. "
                          "A pontuação varia de -1 (venda) a +1 (compra); entre -0,25 e +0,25 "
                          "o sistema considera que não há sinal claro. Isto não é recomendação "
                          "de investimento: é um apoio à decisão baseado em regras explícitas."),
        this);
    explicacao->setWordWrap(true);
    explicacao->setObjectName(QStringLiteral("textoAuxiliar"));

    auto* botaoGerar = new QPushButton(QString::fromUtf8("Gerar para todos os ativos"), this);
    m_botaoAbrir = new QPushButton(QStringLiteral("Abrir no painel"), this);
    m_botaoAbrir->setEnabled(false);

    auto* linhaBotoes = new QHBoxLayout();
    linhaBotoes->addWidget(botaoGerar);
    linhaBotoes->addWidget(m_botaoAbrir);
    linhaBotoes->addStretch();

    m_tabela = new QTableWidget(this);
    m_tabela->setColumnCount(5);
    m_tabela->setHorizontalHeaderLabels(QStringList()
                                        << QStringLiteral("Ativo")
                                        << QString::fromUtf8("Recomendação")
                                        << QString::fromUtf8("Pontuação")
                                        << QStringLiteral("Gerada em")
                                        << QStringLiteral("Regras aplicadas"));
    m_tabela->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tabela->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tabela->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tabela->setAlternatingRowColors(true);
    m_tabela->verticalHeader()->setVisible(false);
    m_tabela->horizontalHeader()->setStretchLastSection(true);

    m_detalhes = new QTextBrowser(this);
    m_detalhes->setPlaceholderText(
        QString::fromUtf8("Selecione uma recomendação para ver a justificativa completa."));

    auto* grupoDetalhes = new QGroupBox(QString::fromUtf8("Justificativa e pareceres por regra"), this);
    auto* disposicaoDetalhes = new QVBoxLayout(grupoDetalhes);
    disposicaoDetalhes->addWidget(m_detalhes);

    m_rotuloMensagem = new QLabel(QString(), this);
    m_rotuloMensagem->setWordWrap(true);
    m_rotuloMensagem->setVisible(false);

    auto* corpo = new QHBoxLayout();
    corpo->addWidget(m_tabela, 3);
    corpo->addWidget(grupoDetalhes, 2);

    auto* disposicao = new QVBoxLayout(this);
    disposicao->setContentsMargins(20, 18, 20, 18);
    disposicao->setSpacing(10);
    disposicao->addWidget(titulo);
    disposicao->addWidget(explicacao);
    disposicao->addLayout(linhaBotoes);
    disposicao->addLayout(corpo, 1);
    disposicao->addWidget(m_rotuloMensagem);

    connect(botaoGerar, &QPushButton::clicked, this, &TelaRecomendacoes::gerarParaTodos);
    connect(m_tabela, &QTableWidget::itemSelectionChanged,
            this, &TelaRecomendacoes::mostrarDetalhesDaSelecao);
    connect(m_botaoAbrir, &QPushButton::clicked, this, [this]() {
        const qint64 id = ativoSelecionado();
        if (id > 0)
        {
            emit abrirAtivo(id);
        }
    });
    connect(m_tabela, &QTableWidget::itemDoubleClicked, this, [this]() {
        const qint64 id = ativoSelecionado();
        if (id > 0)
        {
            emit abrirAtivo(id);
        }
    });

    atualizar();
}

qint64 TelaRecomendacoes::ativoSelecionado() const
{
    const int linha = m_tabela->currentRow();
    if (linha < 0)
    {
        return 0;
    }
    QTableWidgetItem* celula = m_tabela->item(linha, 0);
    if (celula == nullptr)
    {
        return 0;
    }
    return celula->data(Qt::UserRole).toLongLong();
}

void TelaRecomendacoes::atualizar()
{
    const QLocale brasil(QLocale::Portuguese, QLocale::Brazil);
    const QVector<Recomendacao> vigentes = m_contexto.servicoRecomendacao.listarVigentes();

    m_tabela->setRowCount(vigentes.size());
    for (int linha = 0; linha < vigentes.size(); ++linha)
    {
        const Recomendacao& recomendacao = vigentes.at(linha);
        const std::unique_ptr<Ativo> ativo = m_contexto.servicoAtivo.buscarPorId(recomendacao.ativoId());

        auto* celulaAtivo = new QTableWidgetItem(ativo ? ativo->ticker()
                                                       : QString::fromUtf8("(removido)"));
        celulaAtivo->setData(Qt::UserRole, recomendacao.ativoId());
        celulaAtivo->setData(Qt::UserRole + 1, recomendacao.justificativa());

        auto* celulaTipo = new QTableWidgetItem(paraTexto(recomendacao.tipo()));
        if (recomendacao.tipo() == TipoRecomendacao::Compra)
        {
            celulaTipo->setForeground(QColor(QStringLiteral("#2ecc71")));
        }
        else if (recomendacao.tipo() == TipoRecomendacao::Venda)
        {
            celulaTipo->setForeground(QColor(QStringLiteral("#e74c3c")));
        }
        else
        {
            celulaTipo->setForeground(QColor(QStringLiteral("#95a5a6")));
        }

        m_tabela->setItem(linha, 0, celulaAtivo);
        m_tabela->setItem(linha, 1, celulaTipo);
        m_tabela->setItem(linha, 2,
            new QTableWidgetItem(brasil.toString(recomendacao.pontuacao(), 'f', 2)));
        m_tabela->setItem(linha, 3,
            new QTableWidgetItem(recomendacao.geradaEm().toString(QStringLiteral("dd/MM/yyyy"))));
        m_tabela->setItem(linha, 4, new QTableWidgetItem(recomendacao.regraAplicada()));
    }

    m_botaoAbrir->setEnabled(false);
    m_detalhes->clear();

    m_rotuloMensagem->setVisible(vigentes.isEmpty());
    if (vigentes.isEmpty())
    {
        m_rotuloMensagem->setText(QString::fromUtf8(
            "Nenhuma recomendação vigente. Use \"Gerar para todos os ativos\"."));
    }
}

void TelaRecomendacoes::mostrarDetalhesDaSelecao()
{
    const qint64 ativoId = ativoSelecionado();
    m_botaoAbrir->setEnabled(ativoId > 0);
    if (ativoId <= 0)
    {
        m_detalhes->clear();
        return;
    }

    const int linha = m_tabela->currentRow();
    const QString justificativa = m_tabela->item(linha, 0)->data(Qt::UserRole + 1).toString();

    const QLocale brasil(QLocale::Portuguese, QLocale::Brazil);
    QStringList texto;
    texto.append(justificativa);

    const QVector<Recomendacao> pareceres = m_contexto.servicoRecomendacao.detalharPorRegra(ativoId);
    if (!pareceres.isEmpty())
    {
        texto.append(QString());
        texto.append(QString::fromUtf8("--- Reavaliação regra por regra (dados atuais) ---"));
        for (const Recomendacao& parecer : pareceres)
        {
            texto.append(QStringLiteral("- %1: %2 (%3)")
                             .arg(parecer.regraAplicada(),
                                  paraTexto(parecer.tipo()),
                                  brasil.toString(parecer.pontuacao(), 'f', 2)));
        }
    }
    m_detalhes->setPlainText(texto.join(QLatin1Char('\n')));
}

void TelaRecomendacoes::gerarParaTodos()
{
    const QVector<Recomendacao> geradas = m_contexto.servicoRecomendacao.gerarParaTodos();
    atualizar();

    m_rotuloMensagem->setVisible(true);
    if (geradas.isEmpty())
    {
        m_rotuloMensagem->setText(QString::fromUtf8(
            "Nenhuma recomendação foi gerada. Verifique se há ativos cadastrados: %1")
            .arg(m_contexto.servicoRecomendacao.ultimoErro()));
        return;
    }
    m_rotuloMensagem->setText(QString::fromUtf8("%1 recomendação(ões) publicada(s).").arg(geradas.size()));
}

}
