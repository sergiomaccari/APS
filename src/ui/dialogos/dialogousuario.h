#pragma once

// Dialogo de cadastro e edicao de usuarios (exclusivo do administrador).
// No cadastro a senha e obrigatoria; na edicao, deixar o campo em branco
// mantem a senha atual.

#include <QDialog>
#include <QString>

#include "dominio/papelusuario.h"
#include "dominio/usuario.h"

class QComboBox;
class QLabel;
class QLineEdit;

namespace analisador
{

class DialogoUsuario : public QDialog
{
    Q_OBJECT

public:
    explicit DialogoUsuario(QWidget* pai = nullptr, const Usuario* usuarioExistente = nullptr);

    QString nome() const;
    QString email() const;
    QString senha() const;
    PapelUsuario papel() const;
    bool ehEdicao() const;

    // Usuario com os campos do formulario, preservando id e hash quando em edicao.
    Usuario usuarioInformado() const;

private:
    bool m_ehEdicao = false;
    Usuario m_original;
    QLineEdit* m_campoNome = nullptr;
    QLineEdit* m_campoEmail = nullptr;
    QLineEdit* m_campoSenha = nullptr;
    QComboBox* m_campoPapel = nullptr;
    QLabel* m_rotuloAjuda = nullptr;
};

}
