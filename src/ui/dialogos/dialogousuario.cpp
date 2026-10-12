#include "ui/dialogos/dialogousuario.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

namespace analisador
{

DialogoUsuario::DialogoUsuario(QWidget* pai, const Usuario* usuarioExistente)
    : QDialog(pai)
    , m_ehEdicao(usuarioExistente != nullptr)
{
    setWindowTitle(m_ehEdicao ? QString::fromUtf8("Editar usuário")
                              : QString::fromUtf8("Cadastrar usuário"));
    setMinimumWidth(440);

    m_campoNome = new QLineEdit(this);
    m_campoEmail = new QLineEdit(this);
    m_campoEmail->setPlaceholderText(QStringLiteral("nome@dominio.com"));

    m_campoSenha = new QLineEdit(this);
    m_campoSenha->setEchoMode(QLineEdit::Password);

    m_campoPapel = new QComboBox(this);
    m_campoPapel->addItem(paraTexto(PapelUsuario::Investidor),
                          static_cast<int>(PapelUsuario::Investidor));
    m_campoPapel->addItem(paraTexto(PapelUsuario::Administrador),
                          static_cast<int>(PapelUsuario::Administrador));

    m_rotuloAjuda = new QLabel(this);
    m_rotuloAjuda->setWordWrap(true);
    m_rotuloAjuda->setObjectName(QStringLiteral("textoAuxiliar"));

    if (m_ehEdicao)
    {
        m_original = *usuarioExistente;
        m_campoNome->setText(m_original.nome());
        m_campoEmail->setText(m_original.email());
        const int indice = m_campoPapel->findData(static_cast<int>(m_original.papel()));
        if (indice >= 0)
        {
            m_campoPapel->setCurrentIndex(indice);
        }
        m_campoSenha->setPlaceholderText(QString::fromUtf8("deixe em branco para manter a senha atual"));
        m_rotuloAjuda->setText(QString::fromUtf8(
            "A senha só é alterada se o campo for preenchido (mínimo de 6 caracteres)."));
    }
    else
    {
        m_rotuloAjuda->setText(QString::fromUtf8("A senha deve ter ao menos 6 caracteres."));
    }

    auto* formulario = new QFormLayout();
    formulario->addRow(QStringLiteral("Nome:"), m_campoNome);
    formulario->addRow(QStringLiteral("E-mail:"), m_campoEmail);
    formulario->addRow(QStringLiteral("Senha:"), m_campoSenha);
    formulario->addRow(QStringLiteral("Papel:"), m_campoPapel);

    auto* botoes = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, this);
    botoes->button(QDialogButtonBox::Save)->setText(QStringLiteral("Salvar"));
    botoes->button(QDialogButtonBox::Cancel)->setText(QStringLiteral("Cancelar"));

    auto* disposicao = new QVBoxLayout(this);
    disposicao->addLayout(formulario);
    disposicao->addWidget(m_rotuloAjuda);
    disposicao->addWidget(botoes);

    connect(botoes, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(botoes, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

QString DialogoUsuario::nome() const
{
    return m_campoNome->text().trimmed();
}

QString DialogoUsuario::email() const
{
    return m_campoEmail->text().trimmed().toLower();
}

QString DialogoUsuario::senha() const
{
    return m_campoSenha->text();
}

PapelUsuario DialogoUsuario::papel() const
{
    return static_cast<PapelUsuario>(m_campoPapel->currentData().toInt());
}

bool DialogoUsuario::ehEdicao() const
{
    return m_ehEdicao;
}

Usuario DialogoUsuario::usuarioInformado() const
{
    Usuario resultado = m_original;
    resultado.definirNome(nome());
    resultado.definirEmail(email());
    resultado.definirPapel(papel());
    return resultado;
}

}
