#pragma once

// Janela principal do sistema.
//
// Estrutura: navegacao lateral + QStackedWidget com as telas. Os itens visiveis
// dependem do papel do usuario logado (o administrador ve tudo; o investidor ve
// apenas as telas de consulta e as suas proprias).
//
// A classe implementa ObservadorAlerta (padrao Observer): quando o
// ServicoAlerta dispara um alerta, a janela recebe a notificacao e a exibe na
// barra de status, sem que o servico conheca a interface grafica.

#include <QMainWindow>
#include <QVector>

#include "servicos/servicoalerta.h"

class QLabel;
class QListWidget;
class QStackedWidget;

namespace analisador
{

class Contexto;
class TelaAlertas;
class TelaAtivos;
class TelaCarteira;
class TelaDashboard;
class TelaDetalheAtivo;
class TelaImportacao;
class TelaRecomendacoes;
class TelaRegras;
class TelaUsuarios;

class JanelaPrincipal : public QMainWindow, public ObservadorAlerta
{
    Q_OBJECT

public:
    explicit JanelaPrincipal(Contexto& contexto, QWidget* pai = nullptr);
    ~JanelaPrincipal() override;

    // Verdadeiro quando a janela foi fechada pela acao "Trocar usuario",
    // para o programa principal reabrir a tela de login.
    bool desejaTrocarUsuario() const;

    // ObservadorAlerta - chamado pelo ServicoAlerta a cada disparo.
    void alertaDisparado(const Alerta& alerta, const Ativo& ativo, double valorObservado) override;

private slots:
    void trocarUsuario();
    void mostrarSobre();
    void irParaPagina(int indice);

private:
    // Registra uma tela na navegacao lateral e na pilha de paginas.
    void adicionarPagina(const QString& rotulo, QWidget* tela);

    void montarMenus();
    void conectarTelas();

    Contexto& m_contexto;
    QListWidget* m_navegacao = nullptr;
    QStackedWidget* m_paginas = nullptr;
    QLabel* m_rotuloUsuario = nullptr;

    TelaDashboard* m_telaDashboard = nullptr;
    TelaDetalheAtivo* m_telaDetalhe = nullptr;
    TelaRecomendacoes* m_telaRecomendacoes = nullptr;
    TelaCarteira* m_telaCarteira = nullptr;
    TelaAlertas* m_telaAlertas = nullptr;
    TelaAtivos* m_telaAtivos = nullptr;
    TelaImportacao* m_telaImportacao = nullptr;
    TelaRegras* m_telaRegras = nullptr;
    TelaUsuarios* m_telaUsuarios = nullptr;

    bool m_desejaTrocarUsuario = false;
};

}
