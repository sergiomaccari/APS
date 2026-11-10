#pragma once

// Tela de gestao de usuarios (exclusiva do administrador).
// As regras de negocio (e-mail unico, senha minima, protecao do ultimo
// administrador) ficam no ServicoAutenticacao; aqui apenas se apresentam.

#include <QWidget>

class QLabel;
class QPushButton;
class QTableWidget;

namespace analisador
{

class Contexto;

class TelaUsuarios : public QWidget
{
    Q_OBJECT

public:
    explicit TelaUsuarios(Contexto& contexto, QWidget* pai = nullptr);

public slots:
    void atualizar();

private slots:
    void cadastrar();
    void editarSelecionado();
    void removerSelecionado();

private:
    qint64 idSelecionado() const;
    void mostrarMensagem(const QString& mensagem, bool erro);

    Contexto& m_contexto;
    QTableWidget* m_tabela = nullptr;
    QPushButton* m_botaoEditar = nullptr;
    QPushButton* m_botaoRemover = nullptr;
    QLabel* m_rotuloMensagem = nullptr;
};

}
