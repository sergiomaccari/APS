#include "ui/telacarteira.h"

#include <memory>
#include <optional>

#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLocale>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

#include "dominio/ativo.h"
#include "dominio/cotacao.h"
#include "ui/contexto.h"
#include "ui/dialogos/dialogooperacao.h"

namespace analisador
{

TelaCarteira::TelaCarteira(Contexto& contexto, QWidget* pai)
    : QWidget(pai)
    , m_contexto(contexto)
{
    auto* titulo = new QLabel(QStringLiteral("Minha Carteira"), this);
    titulo->setObjectName(QStringLiteral("tituloTela"));

    auto* botaoComprar = new QPushButton(QStringLiteral("Comprar"), this);
    m_botaoVender = new QPushButton(QStringLiteral("Vender"), this);
    auto* botaoRecarregar = new QPushButton(QStringLiteral("Recarregar"), this);

    auto* linhaBotoes = new QHBoxLayout();
    linhaBotoes->addStretch();
    linhaBotoes->addWidget(botaoComprar);
    linhaBotoes->addWidget(m_botaoVender);
    linhaBotoes->addWidget(botaoRecarregar);

    m_tabela = new QTableWidget(this);
    m_tabela->setColumnCount(8);
    m_tabela->setHorizontalHeaderLabels(QStringList()
                                        << QStringLiteral("Ticker")
                                        << QStringLiteral("Ativo")
                                        << QStringLiteral("Qtd.")
                                        << QString::fromUtf8("Preço médio")
                                        << QStringLiteral("Preço atual")
                                        << QStringLiteral("Investido")
                                        << QStringLiteral("Valor atual")
                                        << QString::fromUtf8("Lucro / prejuízo"));
    m_tabela->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tabela->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tabela->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tabela->setAlternatingRowColors(true);
    m_tabela->verticalHeader()->setVisible(false);
    m_tabela->horizontalHeader()->setStretchLastSection(true);

    m_rotuloResumo = new QLabel(QString(), this);
    m_rotuloResumo->setObjectName(QStringLiteral("resumoCarteira"));

    m_rotuloMensagem = new QLabel(QString(), this);
    m_rotuloMensagem->setWordWrap(true);
    m_rotuloMensagem->setVisible(false);

    auto* disposicao = new QVBoxLayout(this);
    disposicao->setContentsMargins(20, 18, 20, 18);
    disposicao->setSpacing(10);
    disposicao->addWidget(titulo);
    disposicao->addLayout(linhaBotoes);
    disposicao->addWidget(m_tabela, 1);
    disposicao->addWidget(m_rotuloResumo);
    disposicao->addWidget(m_rotuloMensagem);

    connect(botaoComprar, &QPushButton::clicked, this, &TelaCarteira::comprar);
    connect(m_botaoVender, &QPushButton::clicked, this, &TelaCarteira::vender);
    connect(botaoRecarregar, &QPushButton::clicked, this, &TelaCarteira::atualizar);

    atualizar();
}

bool TelaCarteira::garantirCarteira()
{
    if (m_carteiraId > 0)
    {
        return true;
    }
    const std::optional<Carteira> carteira =
        m_contexto.servicoCarteira.carteiraDoUsuario(m_contexto.usuarioLogado.id());
    if (!carteira.has_value())
    {
        m_rotuloMensagem->setText(m_contexto.servicoCarteira.ultimoErro());
        m_rotuloMensagem->setVisible(true);
        return false;
    }
    m_carteiraId = carteira.value().id();
    return true;
}

void TelaCarteira::atualizar()
{
    m_rotuloMensagem->setVisible(false);
    if (!garantirCarteira())
    {
        return;
    }

    const QLocale brasil(QLocale::Portuguese, QLocale::Brazil);
    const ResumoCarteira resumo = m_contexto.servicoCarteira.calcularResumo(m_carteiraId);

    m_tabela->setRowCount(resumo.linhas.size());
    for (int linha = 0; linha < resumo.linhas.size(); ++linha)
    {
        const LinhaResumoCarteira& item = resumo.linhas.at(linha);

        auto* celulaTicker = new QTableWidgetItem(item.ticker);
        celulaTicker->setData(Qt::UserRole, item.ativoId);
        celulaTicker->setData(Qt::UserRole + 1, item.quantidade);

        auto* celulaResultado = new QTableWidgetItem(
            QStringLiteral("R$ %1  (%2%)")
                .arg(brasil.toString(item.lucroPrejuizo, 'f', 2),
                     brasil.toString(item.rentabilidadePercentual, 'f', 2)));
        celulaResultado->setForeground(item.lucroPrejuizo >= 0.0 ? QColor(QStringLiteral("#2ecc71"))
                                                                 : QColor(QStringLiteral("#e74c3c")));

        m_tabela->setItem(linha, 0, celulaTicker);
        m_tabela->setItem(linha, 1, new QTableWidgetItem(item.nomeAtivo));
        m_tabela->setItem(linha, 2, new QTableWidgetItem(QString::number(item.quantidade)));
        m_tabela->setItem(linha, 3, new QTableWidgetItem(brasil.toString(item.precoMedio, 'f', 2)));
        m_tabela->setItem(linha, 4,
                          new QTableWidgetItem(item.temCotacao
                                                   ? brasil.toString(item.precoAtual, 'f', 2)
                                                   : QString::fromUtf8("sem cotação")));
        m_tabela->setItem(linha, 5, new QTableWidgetItem(brasil.toString(item.valorInvestido, 'f', 2)));
        m_tabela->setItem(linha, 6, new QTableWidgetItem(brasil.toString(item.valorAtual, 'f', 2)));
        m_tabela->setItem(linha, 7, celulaResultado);
    }

    m_botaoVender->setEnabled(!resumo.vazia());

    if (resumo.vazia())
    {
        m_rotuloResumo->setText(QString::fromUtf8("Carteira vazia. Use \"Comprar\" para registrar a primeira posição."));
        m_rotuloResumo->setStyleSheet(QString());
        return;
    }

    m_rotuloResumo->setText(QString::fromUtf8(
        "Investido: R$ %1     Valor atual: R$ %2     Resultado: R$ %3 (%4%)")
        .arg(brasil.toString(resumo.valorInvestido, 'f', 2),
             brasil.toString(resumo.valorAtual, 'f', 2),
             brasil.toString(resumo.lucroPrejuizo, 'f', 2),
             brasil.toString(resumo.rentabilidadePercentual, 'f', 2)));
    m_rotuloResumo->setStyleSheet(resumo.lucroPrejuizo >= 0.0
                                      ? QStringLiteral("color: #2ecc71; font-weight: bold;")
                                      : QStringLiteral("color: #e74c3c; font-weight: bold;"));
}

void TelaCarteira::comprar()
{
    if (!garantirCarteira())
    {
        return;
    }

    QVector<OpcaoOperacao> opcoes;
    const std::vector<std::unique_ptr<Ativo>> ativos = m_contexto.servicoAtivo.listarTodos();
    for (const std::unique_ptr<Ativo>& ativo : ativos)
    {
        if (!ativo)
        {
            continue;
        }
        OpcaoOperacao opcao;
        opcao.ativoId = ativo->id();
        opcao.rotulo = ativo->rotuloCompleto();
        const std::optional<Cotacao> ultima = m_contexto.repositorioCotacao.buscarUltima(ativo->id());
        opcao.precoSugerido = ultima.has_value() ? ultima.value().fechamento() : 0.0;
        opcoes.append(opcao);
    }

    if (opcoes.isEmpty())
    {
        QMessageBox::information(this, QStringLiteral("Comprar"),
                                 QString::fromUtf8("Nenhum ativo cadastrado para comprar."));
        return;
    }

    DialogoOperacao dialogo(DialogoOperacao::Modo::Compra, opcoes, this);
    if (dialogo.exec() != QDialog::Accepted)
    {
        return;
    }

    if (!m_contexto.servicoCarteira.comprar(m_carteiraId,
                                            dialogo.ativoId(),
                                            dialogo.quantidade(),
                                            dialogo.preco(),
                                            dialogo.data()))
    {
        QMessageBox::warning(this, QStringLiteral("Comprar"), m_contexto.servicoCarteira.ultimoErro());
        return;
    }
    atualizar();
}

void TelaCarteira::vender()
{
    if (!garantirCarteira())
    {
        return;
    }

    QVector<OpcaoOperacao> opcoes;
    const ResumoCarteira resumo = m_contexto.servicoCarteira.calcularResumo(m_carteiraId);
    for (const LinhaResumoCarteira& item : resumo.linhas)
    {
        OpcaoOperacao opcao;
        opcao.ativoId = item.ativoId;
        opcao.rotulo = QStringLiteral("%1 - %2").arg(item.ticker, item.nomeAtivo);
        opcao.precoSugerido = item.precoAtual;
        opcao.quantidadeMaxima = item.quantidade;
        opcoes.append(opcao);
    }

    if (opcoes.isEmpty())
    {
        QMessageBox::information(this, QStringLiteral("Vender"),
                                 QString::fromUtf8("Não há posições na carteira."));
        return;
    }

    DialogoOperacao dialogo(DialogoOperacao::Modo::Venda, opcoes, this);
    if (dialogo.exec() != QDialog::Accepted)
    {
        return;
    }

    if (!m_contexto.servicoCarteira.vender(m_carteiraId, dialogo.ativoId(), dialogo.quantidade()))
    {
        QMessageBox::warning(this, QStringLiteral("Vender"), m_contexto.servicoCarteira.ultimoErro());
        return;
    }
    atualizar();
}

}
