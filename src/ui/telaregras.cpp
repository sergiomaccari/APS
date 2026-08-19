#include "ui/telaregras.h"

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

#include "analise/motoranalise.h"
#include "analise/regraanalise.h"
#include "ui/contexto.h"
#include "ui/dialogos/dialogoregra.h"

namespace analisador
{

TelaRegras::TelaRegras(Contexto& contexto, QWidget* pai)
    : QWidget(pai)
    , m_contexto(contexto)
{
    auto* titulo = new QLabel(QString::fromUtf8("Regras de Análise"), this);
    titulo->setObjectName(QStringLiteral("tituloTela"));

    auto* explicacao = new QLabel(
        QString::fromUtf8("Cada regra é uma estratégia independente (padrão Strategy). O parecer "
                          "final de um ativo é a média das pontuações das regras ativas que "
                          "conseguiram opinar sobre ele."),
        this);
    explicacao->setWordWrap(true);
    explicacao->setObjectName(QStringLiteral("textoAuxiliar"));

    m_botaoEditar = new QPushButton(QStringLiteral("Editar parâmetros"), this);
    m_botaoAlternar = new QPushButton(QStringLiteral("Ativar / desativar"), this);
    m_botaoEditar->setEnabled(false);
    m_botaoAlternar->setEnabled(false);

    auto* linhaBotoes = new QHBoxLayout();
    linhaBotoes->addWidget(m_botaoEditar);
    linhaBotoes->addWidget(m_botaoAlternar);
    linhaBotoes->addStretch();

    m_tabela = new QTableWidget(this);
    m_tabela->setColumnCount(5);
    m_tabela->setHorizontalHeaderLabels(QStringList()
                                        << QStringLiteral("Regra")
                                        << QStringLiteral("Estado")
                                        << QStringLiteral("1º parâmetro")
                                        << QStringLiteral("2º parâmetro")
                                        << QStringLiteral("Critério"));
    m_tabela->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tabela->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tabela->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tabela->setAlternatingRowColors(true);
    m_tabela->verticalHeader()->setVisible(false);
    m_tabela->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_tabela->horizontalHeader()->setStretchLastSection(true);
    m_tabela->setWordWrap(true);

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

    connect(m_botaoEditar, &QPushButton::clicked, this, &TelaRegras::editarSelecionada);
    connect(m_botaoAlternar, &QPushButton::clicked, this, &TelaRegras::alternarAtivacao);
    connect(m_tabela, &QTableWidget::itemSelectionChanged, this, [this]() {
        const bool temSelecao = idSelecionado() > 0;
        m_botaoEditar->setEnabled(temSelecao);
        m_botaoAlternar->setEnabled(temSelecao);
    });
    connect(m_tabela, &QTableWidget::itemDoubleClicked, this, [this]() {
        if (idSelecionado() > 0)
        {
            editarSelecionada();
        }
    });

    atualizar();
}

void TelaRegras::mostrarMensagem(const QString& mensagem, bool erro)
{
    m_rotuloMensagem->setText(mensagem);
    m_rotuloMensagem->setVisible(!mensagem.isEmpty());
    m_rotuloMensagem->setStyleSheet(erro ? QStringLiteral("color: #e74c3c;")
                                         : QStringLiteral("color: #2ecc71;"));
}

qint64 TelaRegras::idSelecionado() const
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

void TelaRegras::atualizar()
{
    const QLocale brasil(QLocale::Portuguese, QLocale::Brazil);
    const QVector<RegraConfigurada> regras = m_contexto.repositorioRegra.listarTodas();

    m_tabela->setRowCount(regras.size());
    for (int linha = 0; linha < regras.size(); ++linha)
    {
        const RegraConfigurada& regra = regras.at(linha);

        auto* celulaNome = new QTableWidgetItem(regra.nomeRegra());
        celulaNome->setData(Qt::UserRole, regra.id());

        auto* celulaEstado = new QTableWidgetItem(regra.ativa() ? QStringLiteral("Ativa")
                                                                : QStringLiteral("Inativa"));
        celulaEstado->setForeground(regra.ativa() ? QColor(QStringLiteral("#2ecc71"))
                                                  : QColor(QStringLiteral("#95a5a6")));

        QString criterio = QString::fromUtf8("Estratégia não reconhecida pelo motor.");
        const std::unique_ptr<RegraAnalise> estrategia = MotorAnalise::criarRegra(regra.nomeRegra());
        if (estrategia)
        {
            estrategia->configurar(regra.parametroPrincipal(), regra.parametroSecundario());
            criterio = estrategia->descricao();
        }

        m_tabela->setItem(linha, 0, celulaNome);
        m_tabela->setItem(linha, 1, celulaEstado);
        m_tabela->setItem(linha, 2, new QTableWidgetItem(brasil.toString(regra.parametroPrincipal(), 'f', 2)));
        m_tabela->setItem(linha, 3, new QTableWidgetItem(brasil.toString(regra.parametroSecundario(), 'f', 2)));
        m_tabela->setItem(linha, 4, new QTableWidgetItem(criterio));
    }
    m_tabela->resizeRowsToContents();

    m_botaoEditar->setEnabled(false);
    m_botaoAlternar->setEnabled(false);

    if (regras.isEmpty())
    {
        mostrarMensagem(QString::fromUtf8(
            "Nenhuma regra cadastrada: o motor usará a configuração padrão embutida."), false);
    }
    else
    {
        mostrarMensagem(QString(), false);
    }
}

void TelaRegras::editarSelecionada()
{
    const qint64 id = idSelecionado();
    if (id <= 0)
    {
        return;
    }

    const std::optional<RegraConfigurada> existente = m_contexto.repositorioRegra.buscarPorId(id);
    if (!existente.has_value())
    {
        atualizar();
        return;
    }

    DialogoRegra dialogo(existente.value(), this);
    if (dialogo.exec() != QDialog::Accepted)
    {
        return;
    }

    const RegraConfigurada editada = dialogo.regraInformada();
    if (!editada.valida())
    {
        mostrarMensagem(QString::fromUtf8("Parâmetros inválidos: use valores não negativos."), true);
        return;
    }
    if (!m_contexto.repositorioRegra.atualizar(editada))
    {
        QMessageBox::warning(this, QStringLiteral("Configurar regra"),
                             m_contexto.repositorioRegra.ultimoErro());
        return;
    }

    atualizar();
    mostrarMensagem(QString::fromUtf8("Regra %1 atualizada.").arg(editada.nomeRegra()), false);
    emit regrasAlteradas();
}

void TelaRegras::alternarAtivacao()
{
    const qint64 id = idSelecionado();
    if (id <= 0)
    {
        return;
    }

    std::optional<RegraConfigurada> existente = m_contexto.repositorioRegra.buscarPorId(id);
    if (!existente.has_value())
    {
        atualizar();
        return;
    }

    RegraConfigurada regra = existente.value();
    regra.definirAtiva(!regra.ativa());
    if (!m_contexto.repositorioRegra.atualizar(regra))
    {
        QMessageBox::warning(this, QStringLiteral("Configurar regra"),
                             m_contexto.repositorioRegra.ultimoErro());
        return;
    }

    atualizar();
    mostrarMensagem(QStringLiteral("Regra %1 agora está %2.")
                        .arg(regra.nomeRegra(),
                             regra.ativa() ? QStringLiteral("ativa") : QStringLiteral("inativa")),
                    false);
    emit regrasAlteradas();
}

}
