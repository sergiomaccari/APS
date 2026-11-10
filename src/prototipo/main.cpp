// Ponto de entrada do PROTÓTIPO DE TELAS do Analisador B3 (entrega de 13/10).
//
// Mesma sequência de src/main.cpp, sem banco de dados:
//   1. aplica o estilo visual da aplicação;
//   2. monta o Contexto (repositórios e serviços do protótipo, que trabalham
//      sobre os dados fixos em memória de src/prototipo/dadosfixos.h);
//   3. autentica pela TelaLogin e abre a JanelaPrincipal.
//
// Nada é gravado em disco: ao fechar o programa, os cadastros e operações
// feitos durante a sessão se perdem e a semente volta ao estado inicial.
//
// A ação "Trocar usuário" fecha a janela principal e devolve o controle ao laço
// abaixo, que reapresenta a tela de login - útil na apresentação para alternar
// entre o perfil administrador e o investidor.

#include <QApplication>
#include <QDialog>
#include <QString>

#include "ui/contexto.h"
#include "ui/estilo.h"
#include "ui/janelaprincipal.h"
#include "ui/telalogin.h"

int main(int argc, char* argv[])
{
    QApplication aplicacao(argc, argv);
    QApplication::setApplicationName(QStringLiteral("Analisador B3"));
    QApplication::setApplicationVersion(QStringLiteral("1.0.0-prototipo"));
    QApplication::setOrganizationName(QStringLiteral("UTFPR - APS"));

    analisador::aplicarEstilo(aplicacao);

    analisador::Contexto contexto;

    // Laço de sessão: permite trocar de usuário sem reiniciar o programa.
    bool continuar = true;
    while (continuar)
    {
        analisador::TelaLogin login(contexto.servicoAutenticacao);
        if (login.exec() != QDialog::Accepted)
        {
            break;
        }
        contexto.usuarioLogado = login.usuarioAutenticado();

        analisador::JanelaPrincipal janela(contexto);
        janela.show();
        aplicacao.exec();

        continuar = janela.desejaTrocarUsuario();
    }

    return 0;
}
