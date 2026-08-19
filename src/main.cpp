// Ponto de entrada do Analisador B3.
//
// Sequencia de inicializacao:
//   1. aplica o estilo visual da aplicacao;
//   2. abre (ou cria) o banco SQLite no diretorio de dados do usuario;
//   3. executa as migracoes versionadas (esquema + dados de semente);
//   4. monta o Contexto (repositorios e servicos);
//   5. autentica pela TelaLogin e abre a JanelaPrincipal.
//
// A acao "Trocar usuario" fecha a janela principal e devolve o controle ao laco
// abaixo, que reapresenta a tela de login - util na apresentacao para alternar
// entre o perfil administrador e o investidor.

#include <QApplication>
#include <QDir>
#include <QMessageBox>
#include <QStandardPaths>
#include <QString>

#include "persistencia/bancodedados.h"
#include "ui/contexto.h"
#include "ui/estilo.h"
#include "ui/janelaprincipal.h"
#include "ui/telalogin.h"

namespace
{

// Caminho do arquivo de banco: pasta de dados da aplicacao do usuario.
QString caminhoDoBanco()
{
    const QString diretorio = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(diretorio);
    return QDir(diretorio).filePath(QStringLiteral("analisador-b3.sqlite"));
}

}

int main(int argc, char* argv[])
{
    QApplication aplicacao(argc, argv);
    QApplication::setApplicationName(QStringLiteral("Analisador B3"));
    QApplication::setApplicationVersion(QStringLiteral("1.0.0"));
    QApplication::setOrganizationName(QStringLiteral("UTFPR - APS"));

    analisador::aplicarEstilo(aplicacao);

    analisador::BancoDeDados& banco = analisador::BancoDeDados::instancia();
    const QString caminho = caminhoDoBanco();

    if (!banco.abrir(caminho))
    {
        QMessageBox::critical(nullptr,
                              QString::fromUtf8("Analisador B3 - erro de inicialização"),
                              banco.ultimoErro());
        return 1;
    }

    if (!banco.executarMigracoes())
    {
        QMessageBox::critical(nullptr,
                              QString::fromUtf8("Analisador B3 - erro de banco de dados"),
                              QString::fromUtf8("Falha ao preparar o banco em %1:\n\n%2")
                                  .arg(caminho, banco.ultimoErro()));
        return 1;
    }

    analisador::Contexto contexto;

    // Laco de sessao: permite trocar de usuario sem reiniciar o programa.
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

    banco.fechar();
    return 0;
}
