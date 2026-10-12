#pragma once

// Tela de alertas do investidor: cadastro, arquivamento e avaliacao manual.
// A avaliacao percorre os alertas monitorados e dispara os que atingiram a
// condicao - as notificacoes chegam a janela principal pelo padrao Observer.

#include <QWidget>

class QLabel;
class QPushButton;
class QTableWidget;

namespace analisador
{

class Contexto;

class TelaAlertas : public QWidget
{
    Q_OBJECT

public:
    explicit TelaAlertas(Contexto& contexto, QWidget* pai = nullptr);

public slots:
    void atualizar();

private slots:
    void criarAlerta();
    void arquivarSelecionado();
    void removerSelecionado();
    void avaliarAgora();

private:
    qint64 idSelecionado() const;
    void mostrarMensagem(const QString& mensagem, bool erro);

    Contexto& m_contexto;
    QTableWidget* m_tabela = nullptr;
    QPushButton* m_botaoArquivar = nullptr;
    QPushButton* m_botaoRemover = nullptr;
    QLabel* m_rotuloMensagem = nullptr;
};

}
