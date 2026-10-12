#include "ui/telausuarios.h"

#include <optional>

#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

#include "ui/contexto.h"
#include "ui/dialogos/dialogousuario.h"

namespace analisador
{

TelaUsuarios::TelaUsuarios(Contexto& contexto, QWidget* pai)
    : QWidget(pai)
    , m_contexto(contexto)
{
    auto* titulo = new QLabel(QString::fromUtf8("Usuários"), this);
    titulo->setObjectName(QStringLiteral("tituloTela"));

    auto* botaoNovo = new QPushButton(QStringLiteral("Novo usuário"), this);
    m_botaoEditar = new QPushButton(QStringLiteral("Editar"), this);
    m_botaoRemover = new QPushButton(QStringLiteral("Remover"), this);
    m_botaoEditar->setEnabled(false);
    m_botaoRemover->setEnabled(false);

    auto* linhaBotoes = new QHBoxLayout();
    linhaBotoes->addWidget(botaoNovo);
    linhaBotoes->addWidget(m_botaoEditar);
    linhaBotoes->addWidget(m_botaoRemover);
    linhaBotoes->addStretch();

    m_tabela = new QTableWidget(this);
    m_tabela->setColumnCount(4);
    m_tabela->setHorizontalHeaderLabels(QStringList()
                                        << QStringLiteral("ID")
                                        << QStringLiteral("Nome")
                                        << QStringLiteral("E-mail")
                                        << QStringLiteral("Papel"));
    m_tabela->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tabela->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tabela->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tabela->setAlternatingRowColors(true);
    m_tabela->verticalHeader()->setVisible(false);
    m_tabela->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_tabela->horizontalHeader()->setStretchLastSection(true);
    m_tabela->setColumnWidth(0, 50);

    m_rotuloMensagem = new QLabel(QString(), this);
    m_rotuloMensagem->setWordWrap(true);
    m_rotuloMensagem->setVisible(false);

    auto* disposicao = new QVBoxLayout(this);
    disposicao->setContentsMargins(20, 18, 20, 18);
    disposicao->setSpacing(10);
    disposicao->addWidget(titulo);
    disposicao->addLayout(linhaBotoes);
    disposicao->addWidget(m_tabela, 1);
    disposicao->addWidget(m_rotuloMensagem);

    connect(botaoNovo, &QPushButton::clicked, this, &TelaUsuarios::cadastrar);
    connect(m_botaoEditar, &QPushButton::clicked, this, &TelaUsuarios::editarSelecionado);
    connect(m_botaoRemover, &QPushButton::clicked, this, &TelaUsuarios::removerSelecionado);
    connect(m_tabela, &QTableWidget::itemSelectionChanged, this, [this]() {
        const bool temSelecao = idSelecionado() > 0;
        m_botaoEditar->setEnabled(temSelecao);
        m_botaoRemover->setEnabled(temSelecao);
    });

    atualizar();
}

void TelaUsuarios::mostrarMensagem(const QString& mensagem, bool erro)
{
    m_rotuloMensagem->setText(mensagem);
    m_rotuloMensagem->setVisible(!mensagem.isEmpty());
    m_rotuloMensagem->setStyleSheet(erro ? QStringLiteral("color: #e74c3c;")
                                         : QStringLiteral("color: #2ecc71;"));
}

qint64 TelaUsuarios::idSelecionado() const
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

void TelaUsuarios::atualizar()
{
    const QVector<Usuario> usuarios = m_contexto.servicoAutenticacao.listarUsuarios();

    m_tabela->setRowCount(usuarios.size());
    for (int linha = 0; linha < usuarios.size(); ++linha)
    {
        const Usuario& usuario = usuarios.at(linha);

        auto* celulaId = new QTableWidgetItem(QString::number(usuario.id()));
        celulaId->setData(Qt::UserRole, usuario.id());

        auto* celulaPapel = new QTableWidgetItem(paraTexto(usuario.papel()));
        if (usuario.ehAdministrador())
        {
            celulaPapel->setForeground(QColor(QStringLiteral("#f39c12")));
        }

        m_tabela->setItem(linha, 0, celulaId);
        m_tabela->setItem(linha, 1, new QTableWidgetItem(usuario.nome()));
        m_tabela->setItem(linha, 2, new QTableWidgetItem(usuario.email()));
        m_tabela->setItem(linha, 3, celulaPapel);
    }

    m_botaoEditar->setEnabled(false);
    m_botaoRemover->setEnabled(false);
    mostrarMensagem(QString(), false);
}

void TelaUsuarios::cadastrar()
{
    DialogoUsuario dialogo(this);
    if (dialogo.exec() != QDialog::Accepted)
    {
        return;
    }

    if (!m_contexto.servicoAutenticacao.cadastrarUsuario(dialogo.nome(),
                                                         dialogo.email(),
                                                         dialogo.senha(),
                                                         dialogo.papel()))
    {
        QMessageBox::warning(this, QString::fromUtf8("Cadastrar usuário"),
                             m_contexto.servicoAutenticacao.ultimoErro());
        return;
    }

    atualizar();
    mostrarMensagem(QString::fromUtf8("Usuário %1 cadastrado.").arg(dialogo.email()), false);
}

void TelaUsuarios::editarSelecionado()
{
    const qint64 id = idSelecionado();
    if (id <= 0)
    {
        return;
    }

    const std::optional<Usuario> existente = m_contexto.repositorioUsuario.buscarPorId(id);
    if (!existente.has_value())
    {
        atualizar();
        return;
    }

    DialogoUsuario dialogo(this, &existente.value());
    if (dialogo.exec() != QDialog::Accepted)
    {
        return;
    }

    if (!m_contexto.servicoAutenticacao.atualizarUsuario(dialogo.usuarioInformado()))
    {
        QMessageBox::warning(this, QString::fromUtf8("Editar usuário"),
                             m_contexto.servicoAutenticacao.ultimoErro());
        return;
    }

    // Senha em branco na edicao significa "manter a senha atual".
    if (!dialogo.senha().isEmpty()
        && !m_contexto.servicoAutenticacao.alterarSenha(id, dialogo.senha()))
    {
        QMessageBox::warning(this, QStringLiteral("Alterar senha"),
                             m_contexto.servicoAutenticacao.ultimoErro());
        atualizar();
        return;
    }

    atualizar();
    mostrarMensagem(QString::fromUtf8("Usuário atualizado."), false);
}

void TelaUsuarios::removerSelecionado()
{
    const qint64 id = idSelecionado();
    if (id <= 0)
    {
        return;
    }
    if (id == m_contexto.usuarioLogado.id())
    {
        mostrarMensagem(QString::fromUtf8("Você não pode remover o usuário com o qual está logado."), true);
        return;
    }

    const QMessageBox::StandardButton resposta = QMessageBox::question(
        this,
        QString::fromUtf8("Remover usuário"),
        QString::fromUtf8("Remover o usuário selecionado?\n\nAs carteiras e alertas dele também "
                          "serão apagados."),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);
    if (resposta != QMessageBox::Yes)
    {
        return;
    }

    if (!m_contexto.servicoAutenticacao.removerUsuario(id))
    {
        QMessageBox::warning(this, QString::fromUtf8("Remover usuário"),
                             m_contexto.servicoAutenticacao.ultimoErro());
        return;
    }

    atualizar();
    mostrarMensagem(QString::fromUtf8("Usuário removido."), false);
}

}
