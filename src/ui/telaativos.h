#pragma once

// Tela de manutencao do cadastro de ativos (exclusiva do administrador).
// Lista os ativos com seus fundamentos e a quantidade de cotacoes importadas,
// permitindo cadastrar, editar e remover.

#include <QWidget>

class QLabel;
class QLineEdit;
class QPushButton;
class QTableWidget;

namespace analisador
{

class Contexto;

class TelaAtivos : public QWidget
{
    Q_OBJECT

public:
    explicit TelaAtivos(Contexto& contexto, QWidget* pai = nullptr);

public slots:
    // Recarrega a tabela a partir do banco. Chamada ao entrar na tela.
    void atualizar();

signals:
    // Emitido quando o cadastro muda, para as outras telas recarregarem.
    void cadastroAlterado();

private slots:
    void cadastrarAtivo();
    void editarAtivoSelecionado();
    void removerAtivoSelecionado();
    void aplicarFiltro();

private:
    // Id do ativo da linha selecionada; 0 quando nada esta selecionado.
    qint64 idSelecionado() const;

    void mostrarMensagem(const QString& mensagem, bool erro);

    Contexto& m_contexto;
    QLineEdit* m_campoBusca = nullptr;
    QTableWidget* m_tabela = nullptr;
    QPushButton* m_botaoEditar = nullptr;
    QPushButton* m_botaoRemover = nullptr;
    QLabel* m_rotuloMensagem = nullptr;
};

}
