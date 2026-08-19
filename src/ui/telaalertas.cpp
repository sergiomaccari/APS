#include "ui/telaalertas.h"

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
#include "ui/dialogos/dialogoalerta.h"

namespace analisador
{

TelaAlertas::TelaAlertas(Contexto& contexto, QWidget* pai)
    : QWidget(pai)
    , m_contexto(contexto)
{
    auto* titulo = new QLabel(QStringLiteral("Alertas de Preço"), this);
    titulo->setObjectName(QStringLiteral("tituloTela"));

    auto* explicacao = new QLabel(
        QString::fromUtf8("Cada alerta monitora uma condição sobre a última cotação importada. "
                          "Ao ser satisfeita, o alerta é disparado e notificado na barra de status."),
        this);
    explicacao->setWordWrap(true);
    explicacao->setObjectName(QStringLiteral("textoAuxiliar"));

    auto* botaoNovo = new QPushButton(QStringLiteral("Novo alerta"), this);
    m_botaoArquivar = new QPushButton(QStringLiteral("Arquivar"), this);
    m_botaoRemover = new QPushButton(QStringLiteral("Remover"), this);
    auto* botaoAvaliar = new QPushButton(QStringLiteral("Avaliar agora"), this);

    m_botaoArquivar->setEnabled(false);
    m_botaoRemover->setEnabled(false);

    auto* linhaBotoes = new QHBoxLayout();
    linhaBotoes->addWidget(botaoNovo);
    linhaBotoes->addWidget(m_botaoArquivar);
    linhaBotoes->addWidget(m_botaoRemover);
    linhaBotoes->addStretch();
    linhaBotoes->addWidget(botaoAvaliar);

    m_tabela = new QTableWidget(this);
    m_tabela->setColumnCount(6);
    m_tabela->setHorizontalHeaderLabels(QStringList()
                                        << QStringLiteral("Ativo")
                                        << QString::fromUtf8("Condição")
                                        << QStringLiteral("Estado")
                                        << QStringLiteral("Criado em")
                                        << QStringLiteral("Disparado em")
                                        << QString::fromUtf8("Última cotação"));
    m_tabela->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tabela->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tabela->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tabela->setAlternatingRowColors(true);
    m_tabela->verticalHeader()->setVisible(false);
    m_tabela->horizontalHeader()->setStretchLastSection(true);

    m_rotuloMensagem = new QLabel(QString(), this);
    m_rotuloMensagem->setWordWrap(true);
    m_rotuloMensagem->setVisible(false);

    auto* disposicao = new QVBoxLayout(this);
    disposicao->setContentsMargins(20, 18, 20, 18);
    disposicao->setSpacing(10);
    disposicao->addWidget(titulo);
    disposicao->addWidget(explicacao);
    disposicao->addLayout(linhaBotoes);
    disposicao->addWidget(m_tabela, 1);
    disposicao->addWidget(m_rotuloMensagem);

    connect(botaoNovo, &QPushButton::clicked, this, &TelaAlertas::criarAlerta);
    connect(m_botaoArquivar, &QPushButton::clicked, this, &TelaAlertas::arquivarSelecionado);
    connect(m_botaoRemover, &QPushButton::clicked, this, &TelaAlertas::removerSelecionado);
    connect(botaoAvaliar, &QPushButton::clicked, this, &TelaAlertas::avaliarAgora);
    connect(m_tabela, &QTableWidget::itemSelectionChanged, this, [this]() {
        const bool temSelecao = idSelecionado() > 0;
        m_botaoArquivar->setEnabled(temSelecao);
        m_botaoRemover->setEnabled(temSelecao);
    });

    atualizar();
}

void TelaAlertas::mostrarMensagem(const QString& mensagem, bool erro)
{
    m_rotuloMensagem->setText(mensagem);
    m_rotuloMensagem->setVisible(!mensagem.isEmpty());
    m_rotuloMensagem->setStyleSheet(erro ? QStringLiteral("color: #e74c3c;")
                                         : QStringLiteral("color: #2ecc71;"));
}

qint64 TelaAlertas::idSelecionado() const
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

void TelaAlertas::atualizar()
{
    const QLocale brasil(QLocale::Portuguese, QLocale::Brazil);
    const QVector<Alerta> alertas =
        m_contexto.servicoAlerta.listarPorUsuario(m_contexto.usuarioLogado.id());

    m_tabela->setRowCount(alertas.size());
    for (int linha = 0; linha < alertas.size(); ++linha)
    {
        const Alerta& alerta = alertas.at(linha);
        const std::unique_ptr<Ativo> ativo = m_contexto.servicoAtivo.buscarPorId(alerta.ativoId());
        const QString rotuloAtivo = ativo ? ativo->ticker() : QString::fromUtf8("(removido)");

        auto* celulaAtivo = new QTableWidgetItem(rotuloAtivo);
        celulaAtivo->setData(Qt::UserRole, alerta.id());

        auto* celulaEstado = new QTableWidgetItem(paraTexto(alerta.estado()));
        if (alerta.estado() == EstadoAlerta::Disparado)
        {
            celulaEstado->setForeground(QColor(QStringLiteral("#f39c12")));
        }
        else if (alerta.estado() == EstadoAlerta::Ativo)
        {
            celulaEstado->setForeground(QColor(QStringLiteral("#2ecc71")));
        }

        QString ultimaCotacao = QString::fromUtf8("sem cotação");
        if (ativo)
        {
            const std::optional<Cotacao> ultima = m_contexto.repositorioCotacao.buscarUltima(ativo->id());
            if (ultima.has_value())
            {
                ultimaCotacao = QStringLiteral("R$ %1 (%2)")
                                    .arg(brasil.toString(ultima.value().fechamento(), 'f', 2),
                                         ultima.value().data().toString(QStringLiteral("dd/MM/yyyy")));
            }
        }

        m_tabela->setItem(linha, 0, celulaAtivo);
        m_tabela->setItem(linha, 1, new QTableWidgetItem(alerta.descricao()));
        m_tabela->setItem(linha, 2, celulaEstado);
        m_tabela->setItem(linha, 3,
            new QTableWidgetItem(alerta.criadoEm().toString(QStringLiteral("dd/MM/yyyy"))));
        m_tabela->setItem(linha, 4,
            new QTableWidgetItem(alerta.disparadoEm().isValid()
                                     ? alerta.disparadoEm().toString(QStringLiteral("dd/MM/yyyy HH:mm"))
                                     : QStringLiteral("--")));
        m_tabela->setItem(linha, 5, new QTableWidgetItem(ultimaCotacao));
    }

    m_botaoArquivar->setEnabled(false);
    m_botaoRemover->setEnabled(false);

    if (alertas.isEmpty())
    {
        mostrarMensagem(QString::fromUtf8("Nenhum alerta cadastrado."), false);
    }
    else
    {
        mostrarMensagem(QString(), false);
    }
}

void TelaAlertas::criarAlerta()
{
    QVector<OpcaoAlerta> opcoes;
    const std::vector<std::unique_ptr<Ativo>> ativos = m_contexto.servicoAtivo.listarTodos();
    for (const std::unique_ptr<Ativo>& ativo : ativos)
    {
        if (!ativo)
        {
            continue;
        }
        OpcaoAlerta opcao;
        opcao.ativoId = ativo->id();
        opcao.rotulo = ativo->rotuloCompleto();
        const std::optional<Cotacao> ultima = m_contexto.repositorioCotacao.buscarUltima(ativo->id());
        opcao.ultimoFechamento = ultima.has_value() ? ultima.value().fechamento() : 0.0;
        opcoes.append(opcao);
    }

    if (opcoes.isEmpty())
    {
        QMessageBox::information(this, QStringLiteral("Novo alerta"),
                                 QString::fromUtf8("Cadastre um ativo antes de criar alertas."));
        return;
    }

    DialogoAlerta dialogo(opcoes, this);
    if (dialogo.exec() != QDialog::Accepted)
    {
        return;
    }

    if (!m_contexto.servicoAlerta.criarAlerta(m_contexto.usuarioLogado.id(),
                                              dialogo.ativoId(),
                                              dialogo.condicao(),
                                              dialogo.valorReferencia()))
    {
        QMessageBox::warning(this, QStringLiteral("Novo alerta"), m_contexto.servicoAlerta.ultimoErro());
        return;
    }

    atualizar();
    mostrarMensagem(QString::fromUtf8("Alerta criado e em monitoramento."), false);
}

void TelaAlertas::arquivarSelecionado()
{
    const qint64 id = idSelecionado();
    if (id <= 0)
    {
        return;
    }
    if (!m_contexto.servicoAlerta.arquivarAlerta(id))
    {
        mostrarMensagem(m_contexto.servicoAlerta.ultimoErro(), true);
        return;
    }
    atualizar();
    mostrarMensagem(QStringLiteral("Alerta arquivado."), false);
}

void TelaAlertas::removerSelecionado()
{
    const qint64 id = idSelecionado();
    if (id <= 0)
    {
        return;
    }

    const QMessageBox::StandardButton resposta = QMessageBox::question(
        this,
        QStringLiteral("Remover alerta"),
        QString::fromUtf8("Remover definitivamente o alerta selecionado?"),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);
    if (resposta != QMessageBox::Yes)
    {
        return;
    }

    if (!m_contexto.servicoAlerta.removerAlerta(id))
    {
        mostrarMensagem(m_contexto.servicoAlerta.ultimoErro(), true);
        return;
    }
    atualizar();
}

void TelaAlertas::avaliarAgora()
{
    const int disparados = m_contexto.servicoAlerta.avaliarAlertas(m_contexto.usuarioLogado.id());
    atualizar();

    if (disparados == 0)
    {
        mostrarMensagem(QString::fromUtf8("Nenhum alerta atingiu a condição nesta avaliação."), false);
        return;
    }
    mostrarMensagem(QString::fromUtf8("%1 alerta(s) disparado(s).").arg(disparados), false);
}

}
