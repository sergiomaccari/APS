#include "ui/janelaprincipal.h"

#include <QAction>
#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QLocale>
#include <QKeySequence>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QStackedWidget>
#include <QStatusBar>
#include <QTimer>
#include <QWidget>

#include "dominio/ativo.h"
#include "ui/contexto.h"
#include "ui/telaalertas.h"
#include "ui/telaativos.h"
#include "ui/telacarteira.h"
#include "ui/teladashboard.h"
#include "ui/teladetalheativo.h"
#include "ui/telaimportacao.h"
#include "ui/telarecomendacoes.h"
#include "ui/telaregras.h"
#include "ui/telausuarios.h"

namespace analisador
{

JanelaPrincipal::JanelaPrincipal(Contexto& contexto, QWidget* pai)
    : QMainWindow(pai)
    , m_contexto(contexto)
{
    setWindowTitle(QString::fromUtf8("Analisador B3 - análise e apoio à decisão de investimentos"));
    resize(1280, 800);

    m_navegacao = new QListWidget(this);
    m_navegacao->setObjectName(QStringLiteral("navegacaoLateral"));
    m_navegacao->setFixedWidth(220);
    m_navegacao->setSpacing(2);

    m_paginas = new QStackedWidget(this);

    auto* central = new QWidget(this);
    auto* disposicao = new QHBoxLayout(central);
    disposicao->setContentsMargins(0, 0, 0, 0);
    disposicao->setSpacing(0);
    disposicao->addWidget(m_navegacao);
    disposicao->addWidget(m_paginas, 1);
    setCentralWidget(central);

    // Telas do investidor (o administrador tambem as acessa).
    m_telaDashboard = new TelaDashboard(m_contexto, this);
    m_telaDetalhe = new TelaDetalheAtivo(m_contexto, this);
    m_telaRecomendacoes = new TelaRecomendacoes(m_contexto, this);
    m_telaCarteira = new TelaCarteira(m_contexto, this);
    m_telaAlertas = new TelaAlertas(m_contexto, this);

    adicionarPagina(QString::fromUtf8("Painel de análise"), m_telaDashboard);
    adicionarPagina(QString::fromUtf8("Detalhes do ativo"), m_telaDetalhe);
    adicionarPagina(QString::fromUtf8("Recomendações"), m_telaRecomendacoes);
    adicionarPagina(QStringLiteral("Minha carteira"), m_telaCarteira);
    adicionarPagina(QStringLiteral("Alertas"), m_telaAlertas);

    if (m_contexto.ehAdministrador())
    {
        m_telaAtivos = new TelaAtivos(m_contexto, this);
        m_telaImportacao = new TelaImportacao(m_contexto, this);
        m_telaRegras = new TelaRegras(m_contexto, this);
        m_telaUsuarios = new TelaUsuarios(m_contexto, this);

        // Separador visual entre as areas do investidor e da administracao.
        auto* separador = new QListWidgetItem(QString::fromUtf8("— Administração —"));
        separador->setFlags(Qt::NoItemFlags);
        separador->setTextAlignment(Qt::AlignCenter);
        m_navegacao->addItem(separador);

        adicionarPagina(QStringLiteral("Ativos"), m_telaAtivos);
        adicionarPagina(QString::fromUtf8("Importar cotações"), m_telaImportacao);
        adicionarPagina(QString::fromUtf8("Regras de análise"), m_telaRegras);
        adicionarPagina(QString::fromUtf8("Usuários"), m_telaUsuarios);
    }

    montarMenus();
    conectarTelas();

    m_rotuloUsuario = new QLabel(this);
    m_rotuloUsuario->setText(QStringLiteral("%1 (%2)")
                                 .arg(m_contexto.usuarioLogado.nome(),
                                      paraTexto(m_contexto.usuarioLogado.papel())));
    statusBar()->addPermanentWidget(m_rotuloUsuario);
    statusBar()->showMessage(QString::fromUtf8("Bem-vindo ao Analisador B3."), 6000);

    // A janela e o observador dos alertas deste usuario.
    m_contexto.servicoAlerta.registrarObservador(this);

    m_navegacao->setCurrentRow(0);
    m_paginas->setCurrentIndex(0);

    // Avalia os alertas logo apos a janela aparecer, para o investidor ver de
    // imediato o que ja atingiu a condicao.
    QTimer::singleShot(400, this, [this]() {
        const int disparados = m_contexto.servicoAlerta.avaliarAlertas(m_contexto.usuarioLogado.id());
        if (disparados == 0)
        {
            statusBar()->showMessage(QString::fromUtf8("Nenhum alerta pendente."), 4000);
        }
        if (m_telaAlertas != nullptr)
        {
            m_telaAlertas->atualizar();
        }
    });
}

JanelaPrincipal::~JanelaPrincipal()
{
    // Evita notificacao para uma janela em destruicao.
    m_contexto.servicoAlerta.removerObservador(this);
}

void JanelaPrincipal::adicionarPagina(const QString& rotulo, QWidget* tela)
{
    m_navegacao->addItem(rotulo);
    m_paginas->addWidget(tela);
}

void JanelaPrincipal::montarMenus()
{
    auto* menuSistema = menuBar()->addMenu(QStringLiteral("&Sistema"));

    auto* acaoTrocar = menuSistema->addAction(QString::fromUtf8("&Trocar usuário"));
    connect(acaoTrocar, &QAction::triggered, this, &JanelaPrincipal::trocarUsuario);

    menuSistema->addSeparator();
    auto* acaoSair = menuSistema->addAction(QStringLiteral("&Sair"));
    acaoSair->setShortcut(QKeySequence::Quit);
    connect(acaoSair, &QAction::triggered, this, &QWidget::close);

    auto* menuAjuda = menuBar()->addMenu(QStringLiteral("A&juda"));
    auto* acaoSobre = menuAjuda->addAction(QStringLiteral("&Sobre"));
    connect(acaoSobre, &QAction::triggered, this, &JanelaPrincipal::mostrarSobre);
}

void JanelaPrincipal::conectarTelas()
{
    connect(m_navegacao, &QListWidget::currentRowChanged, this, &JanelaPrincipal::irParaPagina);

    // Navegacao cruzada entre telas.
    connect(m_telaDashboard, &TelaDashboard::detalharAtivo, this, [this](qint64 ativoId) {
        m_telaDetalhe->selecionarAtivo(ativoId);
        m_navegacao->setCurrentRow(1);
    });
    connect(m_telaRecomendacoes, &TelaRecomendacoes::abrirAtivo, this, [this](qint64 ativoId) {
        m_telaDashboard->selecionarAtivo(ativoId);
        m_navegacao->setCurrentRow(0);
    });

    if (m_telaAtivos != nullptr)
    {
        // Mudanca no cadastro repercute em todas as telas que listam ativos.
        connect(m_telaAtivos, &TelaAtivos::cadastroAlterado, this, [this]() {
            m_telaDashboard->atualizar();
            m_telaDetalhe->atualizar();
            m_telaRecomendacoes->atualizar();
            m_telaCarteira->atualizar();
            m_telaAlertas->atualizar();
            if (m_telaImportacao != nullptr)
            {
                m_telaImportacao->atualizar();
            }
        });
    }

    if (m_telaImportacao != nullptr)
    {
        connect(m_telaImportacao, &TelaImportacao::cotacoesImportadas, this, [this]() {
            m_telaDashboard->atualizar();
            m_telaDetalhe->atualizar();
            m_telaCarteira->atualizar();
            m_telaAlertas->atualizar();
            statusBar()->showMessage(
                QString::fromUtf8("Cotações importadas: gráficos e carteira atualizados."), 5000);
        });
    }

    if (m_telaRegras != nullptr)
    {
        connect(m_telaRegras, &TelaRegras::regrasAlteradas, this, [this]() {
            statusBar()->showMessage(
                QString::fromUtf8("Regras alteradas: gere as recomendações novamente para aplicá-las."),
                6000);
        });
    }
}

void JanelaPrincipal::irParaPagina(int indice)
{
    if (indice < 0)
    {
        return;
    }

    QListWidgetItem* item = m_navegacao->item(indice);
    if (item != nullptr && item->flags() == Qt::NoItemFlags)
    {
        // Separador nao e uma pagina: pula para o item seguinte.
        m_navegacao->setCurrentRow(indice + 1);
        return;
    }

    // O separador ocupa uma linha na navegacao, mas nao tem pagina na pilha.
    int pagina = 0;
    for (int i = 0; i < indice; ++i)
    {
        QListWidgetItem* atual = m_navegacao->item(i);
        if (atual != nullptr && atual->flags() != Qt::NoItemFlags)
        {
            ++pagina;
        }
    }
    if (pagina >= m_paginas->count())
    {
        return;
    }

    m_paginas->setCurrentIndex(pagina);

    // Recarrega a tela ao entrar, para refletir mudancas feitas em outras telas.
    QWidget* tela = m_paginas->widget(pagina);
    if (tela == m_telaDashboard)
    {
        m_telaDashboard->atualizar();
    }
    else if (tela == m_telaDetalhe)
    {
        m_telaDetalhe->atualizar();
    }
    else if (tela == m_telaRecomendacoes)
    {
        m_telaRecomendacoes->atualizar();
    }
    else if (tela == m_telaCarteira)
    {
        m_telaCarteira->atualizar();
    }
    else if (tela == m_telaAlertas)
    {
        m_telaAlertas->atualizar();
    }
    else if (m_telaAtivos != nullptr && tela == m_telaAtivos)
    {
        m_telaAtivos->atualizar();
    }
    else if (m_telaImportacao != nullptr && tela == m_telaImportacao)
    {
        m_telaImportacao->atualizar();
    }
    else if (m_telaRegras != nullptr && tela == m_telaRegras)
    {
        m_telaRegras->atualizar();
    }
    else if (m_telaUsuarios != nullptr && tela == m_telaUsuarios)
    {
        m_telaUsuarios->atualizar();
    }
}

void JanelaPrincipal::alertaDisparado(const Alerta& alerta, const Ativo& ativo, double valorObservado)
{
    const QLocale brasil(QLocale::Portuguese, QLocale::Brazil);
    const QString mensagem = QString::fromUtf8("Alerta disparado: %1 - %2 (valor observado: %3).")
                                 .arg(ativo.ticker(),
                                      alerta.descricao(),
                                      brasil.toString(valorObservado, 'f', 2));
    statusBar()->showMessage(mensagem, 15000);
}

void JanelaPrincipal::trocarUsuario()
{
    m_desejaTrocarUsuario = true;
    close();
}

bool JanelaPrincipal::desejaTrocarUsuario() const
{
    return m_desejaTrocarUsuario;
}

void JanelaPrincipal::mostrarSobre()
{
    QMessageBox::about(
        this,
        QStringLiteral("Sobre o Analisador B3"),
        QString::fromUtf8(
            "<h3>Analisador B3</h3>"
            "<p>Sistema de análise de ativos da B3 e apoio à decisão de investimentos, "
            "com recomendações justificadas por regras transparentes.</p>"
            "<p>Trabalho da disciplina Análise e Projeto de Sistemas (UTFPR).<br>"
            "Equipe: Sergio Roncato Maccari, Arthur Dal Bem Nunes, Leonardo Pereira Shibata "
            "e Thales Romagna Fabrowski.</p>"
            "<p>Implementado em C++20 com Qt 6 (Widgets, Charts e SQL) e SQLite. "
            "Padrões de projeto aplicados: Strategy nas regras de análise, Observer nos "
            "alertas, Repository na persistência e Singleton na conexão com o banco.</p>"
            "<p><i>As recomendações são apoio à decisão baseado em regras explícitas e não "
            "constituem recomendação de investimento.</i></p>"));
}

}
