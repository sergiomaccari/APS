#include "servicos/servicosincronizacao.h"

#include <memory>
#include <vector>

#include "dominio/ativo.h"
#include "dominio/importacao.h"
#include "persistencia/bancodedados.h"

namespace analisador
{

const int ServicoSincronizacao::JANELA_INICIAL_DIAS = 365;

QString ResultadoSincronizacao::resumo() const
{
    if (ativosProcessados == 0)
    {
        return QString::fromUtf8("Nenhum ativo cadastrado para sincronizar.");
    }

    QString texto = QString::fromUtf8(
                        "Sincronização online: %1 ativo(s) verificado(s), %2 cotação(ões) nova(s), "
                        "%3 ignorada(s) por já existirem.")
                        .arg(ativosProcessados)
                        .arg(cotacoesInseridas)
                        .arg(cotacoesIgnoradas);

    if (ativosComFalha > 0)
    {
        texto += QString::fromUtf8(" %1 ativo(s) não puderam ser atualizados.").arg(ativosComFalha);
    }
    return texto;
}

ServicoSincronizacao::ServicoSincronizacao(RepositorioAtivo& repositorioAtivo,
                                           RepositorioCotacao& repositorioCotacao,
                                           RepositorioImportacao& repositorioImportacao,
                                           FonteCotacoesRemota& fonte)
    : m_repositorioAtivo(repositorioAtivo)
    , m_repositorioCotacao(repositorioCotacao)
    , m_repositorioImportacao(repositorioImportacao)
    , m_fonte(fonte)
{
}

QDate ServicoSincronizacao::inicioDaJanela(const std::optional<Cotacao>& ultima, const QDate& hoje)
{
    if (ultima.has_value() && ultima.value().data().isValid())
    {
        // Janela incremental: so o que falta depois do ultimo candle local.
        return ultima.value().data().addDays(1);
    }
    // Ativo sem historico: carga inicial de um ano, suficiente para as medias
    // moveis longas e para o grafico do detalhe do ativo.
    return hoje.addDays(-JANELA_INICIAL_DIAS);
}

bool ServicoSincronizacao::registrarAuditoria(qint64 ativoId,
                                              const QString& ticker,
                                              int lidas,
                                              int inseridas,
                                              const QString& motivoDaRejeicao)
{
    Importacao registro;
    registro.definirAtivoId(ativoId);
    // O campo "arquivo" identifica a origem: a auditoria e unica para CSV e online.
    registro.definirArquivo(QString::fromUtf8("%1 (%2)").arg(m_fonte.nome(), ticker));
    registro.iniciarValidacao();

    if (motivoDaRejeicao.isEmpty())
    {
        registro.concluir(lidas, inseridas);
        // A coluna mensagem_erro e NOT NULL: string vazia, porem nao nula.
        registro.definirMensagemErro(QString::fromUtf8(""));
    }
    else
    {
        registro.rejeitar(motivoDaRejeicao);
        registro.definirLinhasLidas(lidas);
    }

    return m_repositorioImportacao.salvar(registro);
}

bool ServicoSincronizacao::gravar(qint64 ativoId,
                                  const QString& ticker,
                                  const QVector<Cotacao>& cotacoes,
                                  int* inseridas,
                                  QString* erro)
{
    *inseridas = 0;
    const int lidas = static_cast<int>(cotacoes.size());

    BancoDeDados& banco = BancoDeDados::instancia();
    if (!banco.iniciarTransacao())
    {
        *erro = banco.ultimoErro();
        registrarAuditoria(ativoId, ticker, lidas, 0, *erro);
        return false;
    }

    const int gravadas = m_repositorioCotacao.inserirEmLote(cotacoes);

    // Dupla verificacao proposital, igual a da importacao de CSV: o repositorio
    // devolve -1 quando alguma insercao falha e tambem preenche ultimoErro().
    const QString erroDoLote = m_repositorioCotacao.ultimoErro();
    if (gravadas < 0 || !erroDoLote.isEmpty())
    {
        banco.desfazer();
        *erro = erroDoLote.isEmpty()
            ? QString::fromUtf8("Falha ao gravar as cotações baixadas.")
            : erroDoLote;
        registrarAuditoria(ativoId, ticker, lidas, 0, *erro);
        return false;
    }

    if (!registrarAuditoria(ativoId, ticker, lidas, gravadas, QString()))
    {
        banco.desfazer();
        *erro = m_repositorioImportacao.ultimoErro();
        return false;
    }

    if (!banco.confirmar())
    {
        // Commit recusado deixa a transacao aberta: desfazer antes de sair.
        *erro = banco.ultimoErro();
        banco.desfazer();
        return false;
    }

    *inseridas = gravadas;
    return true;
}

ResultadoSincronizacao ServicoSincronizacao::sincronizarTodos()
{
    ResultadoSincronizacao resultado;
    const QDate hoje = QDate::currentDate();

    const std::vector<std::unique_ptr<Ativo>> ativos = m_repositorioAtivo.listarTodos();
    if (ativos.empty())
    {
        resultado.mensagens.append(
            QString::fromUtf8("Nenhum ativo cadastrado: cadastre os ativos antes de sincronizar."));
        return resultado;
    }

    for (const std::unique_ptr<Ativo>& ativo : ativos)
    {
        if (!ativo)
        {
            continue;
        }

        const qint64 ativoId = ativo->id();
        const QString ticker = ativo->ticker();
        ++resultado.ativosProcessados;

        const std::optional<Cotacao> ultima = m_repositorioCotacao.buscarUltima(ativoId);
        const QDate de = inicioDaJanela(ultima, hoje);
        if (de > hoje)
        {
            // Nada faltando: nem gastamos uma requisicao com esse ativo.
            const QString dataLocal = ultima.has_value()
                ? ultima.value().data().toString(QStringLiteral("dd/MM/yyyy"))
                : QStringLiteral("--");
            resultado.mensagens.append(
                QString::fromUtf8("%1: já atualizado (última cotação em %2).").arg(ticker, dataLocal));
            continue;
        }

        QVector<Cotacao> baixadas;
        QString erro;
        if (!m_fonte.buscar(ticker, ativoId, de, hoje, &baixadas, &erro))
        {
            // Falha isolada: registra, avisa e segue para o proximo ativo.
            ++resultado.ativosComFalha;
            resultado.mensagens.append(QString::fromUtf8("%1: falha — %2").arg(ticker, erro));
            registrarAuditoria(ativoId, ticker, 0, 0, erro);
            continue;
        }

        if (baixadas.isEmpty())
        {
            // Sem pregao no intervalo (fim de semana, feriado): nada a auditar.
            resultado.mensagens.append(
                QString::fromUtf8("%1: nenhuma cotação nova entre %2 e %3.")
                    .arg(ticker,
                         de.toString(QStringLiteral("dd/MM/yyyy")),
                         hoje.toString(QStringLiteral("dd/MM/yyyy"))));
            continue;
        }

        int inseridas = 0;
        QString erroGravacao;
        if (!gravar(ativoId, ticker, baixadas, &inseridas, &erroGravacao))
        {
            ++resultado.ativosComFalha;
            resultado.mensagens.append(
                QString::fromUtf8("%1: falha ao gravar — %2").arg(ticker, erroGravacao));
            continue;
        }

        const int ignoradas = static_cast<int>(baixadas.size()) - inseridas;
        resultado.cotacoesInseridas += inseridas;
        resultado.cotacoesIgnoradas += ignoradas;
        resultado.mensagens.append(QString::fromUtf8("%1: %2 novas, %3 ignoradas")
                                       .arg(ticker)
                                       .arg(inseridas)
                                       .arg(ignoradas));
    }

    return resultado;
}

}
