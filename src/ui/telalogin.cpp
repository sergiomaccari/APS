#include "ui/telalogin.h"

#include <QFormLayout>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

#include "servicos/servicoautenticacao.h"

namespace analisador
{

TelaLogin::TelaLogin(ServicoAutenticacao& servico, QWidget* pai)
    : QDialog(pai)
    , m_servico(servico)
{
    setWindowTitle(QString::fromUtf8("Analisador B3 - Entrar"));
    setModal(true);
    setMinimumWidth(420);

    auto* titulo = new QLabel(QStringLiteral("Analisador B3"), this);
    titulo->setObjectName(QStringLiteral("tituloLogin"));
    titulo->setAlignment(Qt::AlignCenter);

    auto* subtitulo = new QLabel(
        QString::fromUtf8("Análise de ativos e apoio à decisão de investimentos"), this);
    subtitulo->setObjectName(QStringLiteral("subtituloLogin"));
    subtitulo->setAlignment(Qt::AlignCenter);

    m_campoEmail = new QLineEdit(this);
    m_campoEmail->setPlaceholderText(QStringLiteral("usuario@analisadorb3.com"));

    m_campoSenha = new QLineEdit(this);
    m_campoSenha->setEchoMode(QLineEdit::Password);
    m_campoSenha->setPlaceholderText(QString::fromUtf8("Sua senha"));

    auto* formulario = new QFormLayout();
    formulario->addRow(QStringLiteral("E-mail:"), m_campoEmail);
    formulario->addRow(QStringLiteral("Senha:"), m_campoSenha);

    m_rotuloErro = new QLabel(QString(), this);
    m_rotuloErro->setObjectName(QStringLiteral("erroLogin"));
    m_rotuloErro->setWordWrap(true);
    m_rotuloErro->setVisible(false);

    m_botaoEntrar = new QPushButton(QStringLiteral("Entrar"), this);
    m_botaoEntrar->setDefault(true);
    auto* botaoCancelar = new QPushButton(QStringLiteral("Cancelar"), this);

    auto* linhaBotoes = new QHBoxLayout();
    linhaBotoes->addStretch();
    linhaBotoes->addWidget(botaoCancelar);
    linhaBotoes->addWidget(m_botaoEntrar);

    auto* credenciais = new QLabel(
        QString::fromUtf8("Demonstração — administrador: admin@analisadorb3.com / admin123"
                          "\nInvestidor: investidor@analisadorb3.com / investidor123"),
        this);
    credenciais->setObjectName(QStringLiteral("dicaLogin"));
    credenciais->setAlignment(Qt::AlignCenter);

    auto* disposicao = new QVBoxLayout(this);
    disposicao->setContentsMargins(28, 24, 28, 20);
    disposicao->setSpacing(10);
    disposicao->addWidget(titulo);
    disposicao->addWidget(subtitulo);
    disposicao->addSpacing(12);
    disposicao->addLayout(formulario);
    disposicao->addWidget(m_rotuloErro);
    disposicao->addLayout(linhaBotoes);
    disposicao->addSpacing(8);
    disposicao->addWidget(credenciais);

    connect(m_botaoEntrar, &QPushButton::clicked, this, &TelaLogin::tentarEntrar);
    connect(botaoCancelar, &QPushButton::clicked, this, &QDialog::reject);
    // Enter em qualquer um dos campos confirma o login.
    connect(m_campoEmail, &QLineEdit::returnPressed, this, &TelaLogin::tentarEntrar);
    connect(m_campoSenha, &QLineEdit::returnPressed, this, &TelaLogin::tentarEntrar);
}

Usuario TelaLogin::usuarioAutenticado() const
{
    return m_usuarioAutenticado;
}

void TelaLogin::mostrarErro(const QString& mensagem)
{
    m_rotuloErro->setText(mensagem);
    m_rotuloErro->setVisible(!mensagem.isEmpty());
}

void TelaLogin::tentarEntrar()
{
    mostrarErro(QString());
    const std::optional<Usuario> autenticado =
        m_servico.autenticar(m_campoEmail->text(), m_campoSenha->text());

    if (!autenticado.has_value())
    {
        mostrarErro(m_servico.ultimoErro());
        m_campoSenha->clear();
        m_campoSenha->setFocus();
        return;
    }

    m_usuarioAutenticado = autenticado.value();
    accept();
}

}
