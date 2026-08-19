#include "ui/telaativos.h"

#include <memory>

#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QLocale>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

#include "dominio/acao.h"
#include "dominio/fundoimobiliario.h"
#include "ui/contexto.h"
#include "ui/dialogos/dialogoativo.h"

namespace analisador
{

TelaAtivos::TelaAtivos(Contexto& contexto, QWidget* pai)
    : QWidget(pai)
    , m_contexto(contexto)
{
    auto* titulo = new QLabel(QString::fromUtf8("Gestão de Ativos"), this);
    titulo->setObjectName(QStringLiteral("tituloTela"));

    m_campoBusca = new QLineEdit(this);
    m_campoBusca->setPlaceholderText(QString::fromUtf8("Buscar por ticker ou nome..."));
    m_campoBusca->setClearButtonEnabled(true);

    auto* botaoNovo = new QPushButton(QStringLiteral("Novo ativo"), this);
    m_botaoEditar = new QPushButton(QStringLiteral("Editar"), this);
    m_botaoRemover = new QPushButton(QStringLiteral("Remover"), this);
    auto* botaoRecarregar = new QPushButton(QStringLiteral("Recarregar"), this);

    m_botaoEditar->setEnabled(false);
    m_botaoRemover->setEnabled(false);

    auto* linhaFerramentas = new QHBoxLayout();
    linhaFerramentas->addWidget(m_campoBusca, 1);
    linhaFerramentas->addWidget(botaoNovo);
    linhaFerramentas->addWidget(m_botaoEditar);
    linhaFerramentas->addWidget(m_botaoRemover);
    linhaFerramentas->addWidget(botaoRecarregar);

    m_tabela = new QTableWidget(this);
    m_tabela->setColumnCount(8);
    m_tabela->setHorizontalHeaderLabels(QStringList()
                                        << QStringLiteral("ID")
                                        << QStringLiteral("Ticker")
                                        << QStringLiteral("Nome")
                                        << QStringLiteral("Setor")
                                        << QStringLiteral("Tipo")
                                        << QString::fromUtf8("Dividend Yield")
                                        << QString::fromUtf8("Índice de valor")
                                        << QString::fromUtf8("Cotações"));
    m_tabela->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tabela->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tabela->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tabela->setAlternatingRowColors(true);
    m_tabela->verticalHeader()->setVisible(false);
    m_tabela->horizontalHeader()->setStretchLastSection(true);
    m_tabela->setColumnWidth(0, 50);
    m_tabela->setColumnWidth(2, 260);

    m_rotuloMensagem = new QLabel(QString(), this);
    m_rotuloMensagem->setWordWrap(true);
    m_rotuloMensagem->setVisible(false);

    auto* disposicao = new QVBoxLayout(this);
    disposicao->setContentsMargins(20, 18, 20, 18);
    disposicao->setSpacing(10);
    disposicao->addWidget(titulo);
    disposicao->addLayout(linhaFerramentas);
    disposicao->addWidget(m_tabela, 1);
    disposicao->addWidget(m_rotuloMensagem);

    connect(botaoNovo, &QPushButton::clicked, this, &TelaAtivos::cadastrarAtivo);
    connect(m_botaoEditar, &QPushButton::clicked, this, &TelaAtivos::editarAtivoSelecionado);
    connect(m_botaoRemover, &QPushButton::clicked, this, &TelaAtivos::removerAtivoSelecionado);
    connect(botaoRecarregar, &QPushButton::clicked, this, &TelaAtivos::atualizar);
    connect(m_campoBusca, &QLineEdit::textChanged, this, &TelaAtivos::aplicarFiltro);
    connect(m_tabela, &QTableWidget::itemSelectionChanged, this, [this]() {
        const bool temSelecao = idSelecionado() > 0;
        m_botaoEditar->setEnabled(temSelecao);
        m_botaoRemover->setEnabled(temSelecao);
    });
    connect(m_tabela, &QTableWidget::itemDoubleClicked, this, [this]() {
        if (idSelecionado() > 0)
        {
            editarAtivoSelecionado();
        }
    });

    atualizar();
}

void TelaAtivos::mostrarMensagem(const QString& mensagem, bool erro)
{
    m_rotuloMensagem->setText(mensagem);
    m_rotuloMensagem->setVisible(!mensagem.isEmpty());
    m_rotuloMensagem->setStyleSheet(erro ? QStringLiteral("color: #e74c3c;")
                                         : QStringLiteral("color: #2ecc71;"));
}

qint64 TelaAtivos::idSelecionado() const
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

void TelaAtivos::atualizar()
{
    const QLocale brasil(QLocale::Portuguese, QLocale::Brazil);
    const QString termo = m_campoBusca->text().trimmed();
    const std::vector<std::unique_ptr<Ativo>> ativos = m_contexto.servicoAtivo.buscar(termo);

    m_tabela->setRowCount(0);
    m_tabela->setRowCount(static_cast<int>(ativos.size()));

    int linha = 0;
    for (const std::unique_ptr<Ativo>& ativo : ativos)
    {
        if (!ativo)
        {
            continue;
        }

        auto* celulaId = new QTableWidgetItem(QString::number(ativo->id()));
        celulaId->setData(Qt::UserRole, ativo->id());

        double dividendYield = 0.0;
        if (const auto* acao = dynamic_cast<const Acao*>(ativo.get()))
        {
            dividendYield = acao->dividendYield();
        }
        else if (const auto* fundo = dynamic_cast<const FundoImobiliario*>(ativo.get()))
        {
            dividendYield = fundo->dividendYield();
        }

        const int quantidadeCotacoes = m_contexto.repositorioCotacao.contarPorAtivo(ativo->id());

        m_tabela->setItem(linha, 0, celulaId);
        m_tabela->setItem(linha, 1, new QTableWidgetItem(ativo->ticker()));
        m_tabela->setItem(linha, 2, new QTableWidgetItem(ativo->nomeEmpresa()));
        m_tabela->setItem(linha, 3, new QTableWidgetItem(ativo->setor()));
        m_tabela->setItem(linha, 4, new QTableWidgetItem(ativo->tipo()));
        m_tabela->setItem(linha, 5,
                          new QTableWidgetItem(QStringLiteral("%1%").arg(brasil.toString(dividendYield, 'f', 2))));
        m_tabela->setItem(linha, 6,
                          new QTableWidgetItem(brasil.toString(ativo->indiceValor(), 'f', 1)));
        m_tabela->setItem(linha, 7,
                          new QTableWidgetItem(quantidadeCotacoes >= 0 ? QString::number(quantidadeCotacoes)
                                                                       : QStringLiteral("--")));
        ++linha;
    }
    m_tabela->setRowCount(linha);

    m_botaoEditar->setEnabled(false);
    m_botaoRemover->setEnabled(false);
    if (linha == 0)
    {
        mostrarMensagem(termo.isEmpty()
                            ? QString::fromUtf8("Nenhum ativo cadastrado. Use \"Novo ativo\" para começar.")
                            : QString::fromUtf8("Nenhum ativo encontrado para \"%1\".").arg(termo),
                        false);
    }
    else
    {
        mostrarMensagem(QString(), false);
    }
}

void TelaAtivos::aplicarFiltro()
{
    atualizar();
}

void TelaAtivos::cadastrarAtivo()
{
    DialogoAtivo dialogo(this);
    if (dialogo.exec() != QDialog::Accepted)
    {
        return;
    }

    std::unique_ptr<Ativo> novo = dialogo.ativoInformado();
    if (!novo)
    {
        mostrarMensagem(QString::fromUtf8("Preencha ao menos o ticker e o nome do ativo."), true);
        return;
    }

    if (!m_contexto.servicoAtivo.salvar(*novo))
    {
        QMessageBox::warning(this, QStringLiteral("Cadastro de ativo"),
                             m_contexto.servicoAtivo.ultimoErro());
        return;
    }

    atualizar();
    mostrarMensagem(QString::fromUtf8("Ativo %1 cadastrado com sucesso.").arg(novo->ticker()), false);
    emit cadastroAlterado();
}

void TelaAtivos::editarAtivoSelecionado()
{
    const qint64 id = idSelecionado();
    if (id <= 0)
    {
        return;
    }

    const std::unique_ptr<Ativo> existente = m_contexto.servicoAtivo.buscarPorId(id);
    if (!existente)
    {
        mostrarMensagem(QString::fromUtf8("O ativo selecionado não foi encontrado."), true);
        atualizar();
        return;
    }

    DialogoAtivo dialogo(this, existente.get());
    if (dialogo.exec() != QDialog::Accepted)
    {
        return;
    }

    std::unique_ptr<Ativo> editado = dialogo.ativoInformado();
    if (!editado)
    {
        mostrarMensagem(QString::fromUtf8("Preencha ao menos o ticker e o nome do ativo."), true);
        return;
    }

    if (!m_contexto.servicoAtivo.salvar(*editado))
    {
        QMessageBox::warning(this, QString::fromUtf8("Edição de ativo"),
                             m_contexto.servicoAtivo.ultimoErro());
        return;
    }

    atualizar();
    mostrarMensagem(QString::fromUtf8("Ativo %1 atualizado.").arg(editado->ticker()), false);
    emit cadastroAlterado();
}

void TelaAtivos::removerAtivoSelecionado()
{
    const qint64 id = idSelecionado();
    if (id <= 0)
    {
        return;
    }

    const std::unique_ptr<Ativo> existente = m_contexto.servicoAtivo.buscarPorId(id);
    if (!existente)
    {
        atualizar();
        return;
    }

    const QMessageBox::StandardButton resposta = QMessageBox::question(
        this,
        QString::fromUtf8("Remover ativo"),
        QString::fromUtf8("Remover %1 (%2)?\n\nEsta ação também apaga as cotações, posições, "
                          "alertas e recomendações vinculadas e não pode ser desfeita.")
            .arg(existente->ticker(), existente->nomeEmpresa()),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);

    if (resposta != QMessageBox::Yes)
    {
        return;
    }

    if (!m_contexto.servicoAtivo.remover(id))
    {
        QMessageBox::warning(this, QString::fromUtf8("Remover ativo"),
                             m_contexto.servicoAtivo.ultimoErro());
        return;
    }

    atualizar();
    mostrarMensagem(QString::fromUtf8("Ativo %1 removido.").arg(existente->ticker()), false);
    emit cadastroAlterado();
}

}
