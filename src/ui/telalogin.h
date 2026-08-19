#pragma once

// Tela de autenticacao. Aberta como dialogo modal antes da janela principal:
// se o usuario cancelar, a aplicacao encerra sem abrir o sistema.

#include <QDialog>

#include "dominio/usuario.h"

class QLabel;
class QLineEdit;
class QPushButton;

namespace analisador
{

class ServicoAutenticacao;

class TelaLogin : public QDialog
{
    Q_OBJECT

public:
    explicit TelaLogin(ServicoAutenticacao& servico, QWidget* pai = nullptr);

    // Valido somente depois de accept(): o usuario que entrou no sistema.
    Usuario usuarioAutenticado() const;

private slots:
    void tentarEntrar();

private:
    void mostrarErro(const QString& mensagem);

    ServicoAutenticacao& m_servico;
    QLineEdit* m_campoEmail = nullptr;
    QLineEdit* m_campoSenha = nullptr;
    QPushButton* m_botaoEntrar = nullptr;
    QLabel* m_rotuloErro = nullptr;
    Usuario m_usuarioAutenticado;
};

}
