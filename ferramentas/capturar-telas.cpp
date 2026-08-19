// Utilitario de captura das telas do sistema.
//
// Abre um banco temporario, importa os CSVs de demonstracao, gera as
// recomendacoes e salva um PNG de cada tela em capturas/. Serve para dois fins:
//   1. verificar que todas as telas renderizam sem erro (roda com a plataforma
//      "offscreen", portanto funciona em servidor e no WSL sem interface);
//   2. gerar as figuras do capitulo "Protótipo" do documento da disciplina.
//
// Uso:  ./build/capturar-telas [diretorio-de-saida]

#include <QApplication>
#include <QDir>
#include <QEventLoop>
#include <QTimer>
#include <QPixmap>
#include <QTemporaryDir>
#include <QTextStream>
#include <QWidget>

#include "persistencia/bancodedados.h"
#include "servicos/servicoimportacao.h"
#include "ui/contexto.h"
#include "ui/estilo.h"
#include "dominio/alerta.h"
#include "ui/janelaprincipal.h"
#include "ui/telaalertas.h"
#include "ui/telaativos.h"
#include "ui/telacarteira.h"
#include "ui/teladashboard.h"
#include "ui/teladetalheativo.h"
#include "ui/telaimportacao.h"
#include "ui/telalogin.h"
#include "ui/telarecomendacoes.h"
#include "ui/telaregras.h"
#include "ui/telausuarios.h"

namespace
{

QTextStream& saida()
{
    static QTextStream fluxo(stdout);
    return fluxo;
}

// Desenha o widget e grava o PNG. Devolve false quando a captura sai vazia.
bool capturar(QWidget* tela, const QString& arquivo, int largura, int altura)
{
    tela->resize(largura, altura);
    tela->show();
    QApplication::processEvents();

    // Os graficos do Qt Charts entram animados (as series comecam invisiveis):
    // sem esperar o fim da animacao, a captura sai com o grafico em branco.
    QEventLoop espera;
    QTimer::singleShot(1500, &espera, &QEventLoop::quit);
    espera.exec();
    QApplication::processEvents();

    const QPixmap imagem = tela->grab();
    tela->hide();

    if (imagem.isNull() || !imagem.save(arquivo, "PNG"))
    {
        saida() << "FALHA ao capturar " << arquivo << Qt::endl;
        return false;
    }
    saida() << "ok  " << arquivo << "  (" << imagem.width() << "x" << imagem.height() << ")" << Qt::endl;
    return true;
}

}

int main(int argc, char* argv[])
{
    QApplication aplicacao(argc, argv);
    analisador::aplicarEstilo(aplicacao);

    const QString diretorioSaida = argc > 1 ? QString::fromLocal8Bit(argv[1])
                                            : QStringLiteral("capturas");
    QDir().mkpath(diretorioSaida);

    // Banco proprio da captura, para nao interferir no banco de uso normal.
    QTemporaryDir temporario;
    if (!temporario.isValid())
    {
        saida() << "Nao foi possivel criar o diretorio temporario." << Qt::endl;
        return 1;
    }

    analisador::BancoDeDados& banco = analisador::BancoDeDados::instancia();
    if (!banco.abrir(QDir(temporario.path()).filePath(QStringLiteral("capturas.sqlite")))
        || !banco.executarMigracoes())
    {
        saida() << "Erro de banco: " << banco.ultimoErro() << Qt::endl;
        return 1;
    }

    analisador::Contexto contexto;

    // Cada tela e capturada com o perfil que a usa de verdade: as telas de
    // consulta com o investidor (que tem carteira e alertas) e as de
    // administracao com o administrador.
    const auto administrador = contexto.repositorioUsuario.buscarPorEmail(
        QStringLiteral("admin@analisadorb3.com"));
    const auto investidor = contexto.repositorioUsuario.buscarPorEmail(
        QStringLiteral("investidor@analisadorb3.com"));
    if (!administrador.has_value() || !investidor.has_value())
    {
        saida() << "Usuarios da semente nao encontrados." << Qt::endl;
        return 1;
    }
    contexto.usuarioLogado = administrador.value();

    // Popula o banco como um usuario faria na apresentacao.
    const analisador::ResultadoImportacao importacao =
        contexto.servicoImportacao.importarDiretorio(QStringLiteral("dados"));
    saida() << "importacao: " << importacao.resumo() << Qt::endl;

    const auto recomendacoes = contexto.servicoRecomendacao.gerarParaTodos();
    saida() << "recomendacoes geradas: " << recomendacoes.size() << Qt::endl;
    for (const analisador::Recomendacao& recomendacao : recomendacoes)
    {
        const auto ativo = contexto.repositorioAtivo.buscarPorId(recomendacao.ativoId());
        saida() << "  " << (ativo ? ativo->ticker() : QStringLiteral("?")) << ": "
                << analisador::paraTexto(recomendacao.tipo()) << " ("
                << QString::number(recomendacao.pontuacao(), 'f', 3) << ")" << Qt::endl;
    }

    // Alertas de demonstracao para o investidor: um que dispara de imediato
    // (preco acima de um valor baixo) e um que segue monitorando.
    contexto.usuarioLogado = investidor.value();
    const auto petr = contexto.repositorioAtivo.buscarPorTicker(QStringLiteral("PETR4"));
    const auto vale = contexto.repositorioAtivo.buscarPorTicker(QStringLiteral("VALE3"));
    if (petr && vale)
    {
        const auto ultimaPetr = contexto.repositorioCotacao.buscarUltima(petr->id());
        const auto ultimaVale = contexto.repositorioCotacao.buscarUltima(vale->id());
        if (ultimaPetr.has_value())
        {
            contexto.servicoAlerta.criarAlerta(investidor->id(), petr->id(),
                                               analisador::TipoCondicaoAlerta::PrecoAcimaDe,
                                               ultimaPetr->fechamento() * 0.97);
        }
        if (ultimaVale.has_value())
        {
            contexto.servicoAlerta.criarAlerta(investidor->id(), vale->id(),
                                               analisador::TipoCondicaoAlerta::PrecoAbaixoDe,
                                               ultimaVale->fechamento() * 0.90);
        }
        const int disparados = contexto.servicoAlerta.avaliarAlertas(investidor->id());
        saida() << "alertas disparados na avaliacao: " << disparados << Qt::endl;
    }

    int falhas = 0;
    const auto arquivo = [&diretorioSaida](const QString& nome) {
        return QDir(diretorioSaida).filePath(nome);
    };

    analisador::TelaLogin login(contexto.servicoAutenticacao);
    falhas += capturar(&login, arquivo(QStringLiteral("01-login.png")), 460, 380) ? 0 : 1;

    analisador::TelaDashboard dashboard(contexto);
    falhas += capturar(&dashboard, arquivo(QStringLiteral("02-painel-analise.png")), 1280, 800) ? 0 : 1;

    analisador::TelaDetalheAtivo detalhe(contexto);
    falhas += capturar(&detalhe, arquivo(QStringLiteral("03-detalhe-ativo.png")), 1280, 800) ? 0 : 1;

    analisador::TelaRecomendacoes recomendacoesTela(contexto);
    falhas += capturar(&recomendacoesTela, arquivo(QStringLiteral("04-recomendacoes.png")), 1280, 700) ? 0 : 1;

    analisador::TelaCarteira carteira(contexto);
    falhas += capturar(&carteira, arquivo(QStringLiteral("05-carteira.png")), 1280, 700) ? 0 : 1;

    analisador::TelaAlertas alertas(contexto);
    falhas += capturar(&alertas, arquivo(QStringLiteral("06-alertas.png")), 1280, 700) ? 0 : 1;

    // A partir daqui, perfil administrador.
    contexto.usuarioLogado = administrador.value();

    analisador::TelaAtivos ativos(contexto);
    falhas += capturar(&ativos, arquivo(QStringLiteral("07-ativos.png")), 1280, 700) ? 0 : 1;

    analisador::TelaImportacao importacaoTela(contexto);
    falhas += capturar(&importacaoTela, arquivo(QStringLiteral("08-importacao.png")), 1280, 700) ? 0 : 1;

    analisador::TelaRegras regras(contexto);
    falhas += capturar(&regras, arquivo(QStringLiteral("09-regras.png")), 1280, 700) ? 0 : 1;

    analisador::TelaUsuarios usuarios(contexto);
    falhas += capturar(&usuarios, arquivo(QStringLiteral("10-usuarios.png")), 1280, 600) ? 0 : 1;

    analisador::JanelaPrincipal janelaAdmin(contexto);
    falhas += capturar(&janelaAdmin, arquivo(QStringLiteral("11-janela-principal-admin.png")), 1360, 860) ? 0 : 1;

    contexto.usuarioLogado = investidor.value();
    analisador::JanelaPrincipal janelaInvestidor(contexto);
    falhas += capturar(&janelaInvestidor,
                       arquivo(QStringLiteral("12-janela-principal-investidor.png")), 1360, 860) ? 0 : 1;

    saida() << (falhas == 0 ? "TODAS as telas capturadas."
                            : QStringLiteral("%1 captura(s) falharam.").arg(falhas)) << Qt::endl;
    banco.fechar();
    return falhas == 0 ? 0 : 1;
}
