#pragma once

// Tela de configuracao das regras de analise (exclusiva do administrador).
// Permite ativar, desativar e reparametrizar as estrategias sem recompilar o
// sistema - o MotorAnalise le essas configuracoes a cada analise.

#include <QWidget>

class QLabel;
class QPushButton;
class QTableWidget;

namespace analisador
{

class Contexto;

class TelaRegras : public QWidget
{
    Q_OBJECT

public:
    explicit TelaRegras(Contexto& contexto, QWidget* pai = nullptr);

public slots:
    void atualizar();

signals:
    // Mudar regras muda as recomendacoes futuras.
    void regrasAlteradas();

private slots:
    void editarSelecionada();
    void alternarAtivacao();
    void abrirBacktest();

private:
    qint64 idSelecionado() const;
    void mostrarMensagem(const QString& mensagem, bool erro);

    Contexto& m_contexto;
    QTableWidget* m_tabela = nullptr;
    QPushButton* m_botaoEditar = nullptr;
    QPushButton* m_botaoAlternar = nullptr;
    QPushButton* m_botaoBacktest = nullptr;
    QLabel* m_rotuloMensagem = nullptr;
};

}
