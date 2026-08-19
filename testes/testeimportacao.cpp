// Testes da importacao de cotacoes a partir de CSV.
//
// Cada teste roda sobre um banco SQLite recem-criado em diretorio temporario,
// com as migracoes aplicadas - portanto exercita de verdade o esquema, os
// repositorios e o controle de transacao, nao apenas a leitura do arquivo.
//
// O ponto central verificado aqui e a atomicidade: um arquivo com erro
// estrutural nao deixa NENHUMA cotacao gravada, e reimportar o mesmo arquivo
// valido nao duplica dados (idempotencia).

#include <optional>

#include <QDate>
#include <QDir>
#include <QFile>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QTemporaryDir>
#include <QTest>
#include <QTextStream>
#include <QVector>

#include "dominio/acao.h"
#include "dominio/importacao.h"
#include "persistencia/bancodedados.h"
#include "persistencia/repositorioativo.h"
#include "persistencia/repositoriocotacao.h"
#include "persistencia/repositorioimportacao.h"
#include "servicos/servicoimportacao.h"

namespace analisador
{

class TesteImportacao : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void init();

    void arquivoValidoImportaTodasAsLinhas();
    void reimportarMesmoArquivoNaoDuplica();
    void arquivoInvalidoNaoGravaNada();
    void cabecalhoInvalidoRejeitaArquivo();
    void arquivoInexistenteRelataErro();
    void numeroNoPadraoBrasileiroEConvertido();
    void historicoRegistraEstadoDaImportacao();
    void importarDiretorioCasaTickerComAtivo();

private:
    // Cria um CSV no diretorio temporario e devolve o caminho completo.
    QString escreverArquivo(const QString& nome, const QStringList& linhas) const;

    // CSV valido com "quantidade" pregoes consecutivos a partir de 05/01/2026.
    QStringList linhasValidas(int quantidade) const;

    qint64 cadastrarAtivo(const QString& ticker);

    QTemporaryDir m_diretorio;
    qint64 m_ativoId = 0;
};

void TesteImportacao::initTestCase()
{
    QVERIFY2(m_diretorio.isValid(), "Nao foi possivel criar o diretorio temporario dos testes.");
}

void TesteImportacao::init()
{
    // Banco novo a cada teste: o nome do arquivo carrega o nome do caso atual.
    const QString nomeBanco = QStringLiteral("%1.sqlite").arg(QTest::currentTestFunction());
    const QString caminho = QDir(m_diretorio.path()).filePath(nomeBanco);
    QFile::remove(caminho);

    BancoDeDados& banco = BancoDeDados::instancia();
    QVERIFY2(banco.abrir(caminho), qPrintable(banco.ultimoErro()));
    QVERIFY2(banco.executarMigracoes(), qPrintable(banco.ultimoErro()));

    m_ativoId = cadastrarAtivo(QStringLiteral("TSTE4"));
    QVERIFY(m_ativoId > 0);
}

qint64 TesteImportacao::cadastrarAtivo(const QString& ticker)
{
    RepositorioAtivo repositorio;
    Acao acao(0, ticker, QStringLiteral("Empresa de Teste S.A."), QStringLiteral("Testes"),
              8.0, 5.0, 1000000.0);
    if (!repositorio.salvar(acao))
    {
        return 0;
    }
    return acao.id();
}

QString TesteImportacao::escreverArquivo(const QString& nome, const QStringList& linhas) const
{
    const QString caminho = QDir(m_diretorio.path()).filePath(nome);
    QFile arquivo(caminho);
    if (!arquivo.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        return QString();
    }
    QTextStream fluxo(&arquivo);
    fluxo.setEncoding(QStringConverter::Utf8);
    for (const QString& linha : linhas)
    {
        fluxo << linha << Qt::endl;
    }
    arquivo.close();
    return caminho;
}

QStringList TesteImportacao::linhasValidas(int quantidade) const
{
    QStringList linhas;
    linhas.append(QStringLiteral("Data;Abertura;Maxima;Minima;Fechamento;Volume"));

    QDate data(2026, 1, 5);
    double base = 20.00;
    for (int i = 0; i < quantidade; ++i)
    {
        // Serie simples e sempre consistente: minima <= abertura/fechamento <= maxima.
        const double abertura = base;
        const double fechamento = base + 0.30;
        const double maxima = fechamento + 0.20;
        const double minima = abertura - 0.20;
        linhas.append(QStringLiteral("%1;%2;%3;%4;%5;%6")
                          .arg(data.toString(QStringLiteral("dd/MM/yyyy")))
                          .arg(QString::number(abertura, 'f', 2).replace(QLatin1Char('.'), QLatin1Char(',')))
                          .arg(QString::number(maxima, 'f', 2).replace(QLatin1Char('.'), QLatin1Char(',')))
                          .arg(QString::number(minima, 'f', 2).replace(QLatin1Char('.'), QLatin1Char(',')))
                          .arg(QString::number(fechamento, 'f', 2).replace(QLatin1Char('.'), QLatin1Char(',')))
                          .arg(1000000 + i * 1000));
        base += 0.10;
        data = data.addDays(1);
    }
    return linhas;
}

void TesteImportacao::arquivoValidoImportaTodasAsLinhas()
{
    const QString caminho = escreverArquivo(QStringLiteral("valido.csv"), linhasValidas(10));
    QVERIFY(!caminho.isEmpty());

    RepositorioCotacao repositorioCotacao;
    RepositorioAtivo repositorioAtivo;
    RepositorioImportacao repositorioImportacao;
    ServicoImportacao servico(repositorioCotacao, repositorioAtivo, repositorioImportacao);

    const ResultadoImportacao resultado = servico.importarCsv(caminho, m_ativoId);

    QVERIFY2(resultado.sucesso, qPrintable(resultado.erros.join(QLatin1Char('\n'))));
    QCOMPARE(resultado.linhasLidas, 10);
    QCOMPARE(resultado.linhasInseridas, 10);
    QCOMPARE(resultado.linhasIgnoradas, 0);
    QVERIFY(resultado.erros.isEmpty());
    QCOMPARE(repositorioCotacao.contarPorAtivo(m_ativoId), 10);

    // A serie deve voltar ordenada por data crescente.
    const QVector<Cotacao> historico = repositorioCotacao.listarPorAtivo(m_ativoId);
    QCOMPARE(static_cast<int>(historico.size()), 10);
    QCOMPARE(historico.first().data(), QDate(2026, 1, 5));
    QCOMPARE(historico.last().data(), QDate(2026, 1, 14));
    QVERIFY(historico.first().data() < historico.last().data());
}

void TesteImportacao::reimportarMesmoArquivoNaoDuplica()
{
    const QString caminho = escreverArquivo(QStringLiteral("idempotente.csv"), linhasValidas(8));
    RepositorioCotacao repositorioCotacao;
    RepositorioAtivo repositorioAtivo;
    RepositorioImportacao repositorioImportacao;
    ServicoImportacao servico(repositorioCotacao, repositorioAtivo, repositorioImportacao);

    const ResultadoImportacao primeira = servico.importarCsv(caminho, m_ativoId);
    QVERIFY(primeira.sucesso);
    QCOMPARE(primeira.linhasInseridas, 8);

    const ResultadoImportacao segunda = servico.importarCsv(caminho, m_ativoId);
    QVERIFY2(segunda.sucesso, "Reimportacao de arquivo valido deve continuar sendo sucesso.");
    QCOMPARE(segunda.linhasLidas, 8);
    QCOMPARE(segunda.linhasInseridas, 0);
    QCOMPARE(segunda.linhasIgnoradas, 8);

    // O total no banco nao muda: a idempotencia vem do indice UNIQUE (ativo, data).
    QCOMPARE(repositorioCotacao.contarPorAtivo(m_ativoId), 8);
}

void TesteImportacao::arquivoInvalidoNaoGravaNada()
{
    RepositorioCotacao repositorioCotacao;
    RepositorioAtivo repositorioAtivo;
    RepositorioImportacao repositorioImportacao;
    ServicoImportacao servico(repositorioCotacao, repositorioAtivo, repositorioImportacao);

    // Estado inicial conhecido: 5 cotacoes validas ja importadas.
    const QString valido = escreverArquivo(QStringLiteral("base.csv"), linhasValidas(5));
    QVERIFY(servico.importarCsv(valido, m_ativoId).sucesso);
    QCOMPARE(repositorioCotacao.contarPorAtivo(m_ativoId), 5);

    // Arquivo com tres problemas: data invalida, maxima < minima e coluna faltando.
    QStringList linhas = linhasValidas(6);
    linhas[2] = QStringLiteral("2026-01-06;20,10;20,50;19,90;20,40;1001000");
    linhas[4] = QStringLiteral("08/01/2026;20,30;19,10;20,90;20,60;1003000");
    linhas[6] = QStringLiteral("10/01/2026;20,50;20,90;20,30;1005000");
    const QString invalido = escreverArquivo(QStringLiteral("invalido.csv"), linhas);

    const ResultadoImportacao resultado = servico.importarCsv(invalido, m_ativoId);

    QVERIFY2(!resultado.sucesso, "Arquivo com erro estrutural deve ser rejeitado.");
    QCOMPARE(static_cast<int>(resultado.erros.size()), 3);
    QCOMPARE(resultado.linhasInseridas, 0);

    // Prova da transacao: o total permanece exatamente o de antes da tentativa.
    QCOMPARE(repositorioCotacao.contarPorAtivo(m_ativoId), 5);
}

void TesteImportacao::cabecalhoInvalidoRejeitaArquivo()
{
    QStringList linhas = linhasValidas(4);
    linhas[0] = QStringLiteral("Dia;Abre;Alta;Baixa;Fecha;Qtd");
    const QString caminho = escreverArquivo(QStringLiteral("cabecalho.csv"), linhas);

    RepositorioCotacao repositorioCotacao;
    RepositorioAtivo repositorioAtivo;
    RepositorioImportacao repositorioImportacao;
    ServicoImportacao servico(repositorioCotacao, repositorioAtivo, repositorioImportacao);

    const ResultadoImportacao resultado = servico.importarCsv(caminho, m_ativoId);
    QVERIFY(!resultado.sucesso);
    QVERIFY(!resultado.erros.isEmpty());
    QCOMPARE(repositorioCotacao.contarPorAtivo(m_ativoId), 0);
}

void TesteImportacao::arquivoInexistenteRelataErro()
{
    RepositorioCotacao repositorioCotacao;
    RepositorioAtivo repositorioAtivo;
    RepositorioImportacao repositorioImportacao;
    ServicoImportacao servico(repositorioCotacao, repositorioAtivo, repositorioImportacao);

    const QString caminho = QDir(m_diretorio.path()).filePath(QStringLiteral("nao-existe.csv"));
    const ResultadoImportacao resultado = servico.importarCsv(caminho, m_ativoId);

    QVERIFY(!resultado.sucesso);
    QVERIFY(!resultado.erros.isEmpty());
    QVERIFY(!servico.ultimoErro().isEmpty());
}

void TesteImportacao::numeroNoPadraoBrasileiroEConvertido()
{
    // Virgula decimal com e sem separador de milhar, e ponto decimal isolado.
    const std::optional<double> comMilhar = ServicoImportacao::converterNumero(QStringLiteral("1.234,56"));
    QVERIFY(comMilhar.has_value());
    QVERIFY(qAbs(comMilhar.value() - 1234.56) < 1e-9);

    const std::optional<double> simples = ServicoImportacao::converterNumero(QStringLiteral("38,42"));
    QVERIFY(simples.has_value());
    QVERIFY(qAbs(simples.value() - 38.42) < 1e-9);

    const std::optional<double> comPonto = ServicoImportacao::converterNumero(QStringLiteral("38.42"));
    QVERIFY(comPonto.has_value());
    QVERIFY(qAbs(comPonto.value() - 38.42) < 1e-9);

    QVERIFY(!ServicoImportacao::converterNumero(QStringLiteral("abc")).has_value());
    QVERIFY(!ServicoImportacao::converterNumero(QString()).has_value());
}

void TesteImportacao::historicoRegistraEstadoDaImportacao()
{
    RepositorioCotacao repositorioCotacao;
    RepositorioAtivo repositorioAtivo;
    RepositorioImportacao repositorioImportacao;
    ServicoImportacao servico(repositorioCotacao, repositorioAtivo, repositorioImportacao);

    const QString valido = escreverArquivo(QStringLiteral("historico-ok.csv"), linhasValidas(3));
    QVERIFY(servico.importarCsv(valido, m_ativoId).sucesso);

    QStringList linhas = linhasValidas(3);
    linhas[1] = QStringLiteral("32/01/2026;20,00;20,50;19,80;20,30;1000000");
    const QString invalido = escreverArquivo(QStringLiteral("historico-erro.csv"), linhas);
    QVERIFY(!servico.importarCsv(invalido, m_ativoId).sucesso);

    const QVector<Importacao> historico = servico.historico(10);
    QCOMPARE(static_cast<int>(historico.size()), 2);

    // O mais recente vem primeiro: a importacao rejeitada.
    QCOMPARE(historico.first().estado(), EstadoImportacao::Rejeitada);
    QVERIFY(!historico.first().mensagemErro().isEmpty());
    QCOMPARE(historico.first().linhasInseridas(), 0);

    QCOMPARE(historico.last().estado(), EstadoImportacao::Concluida);
    QCOMPARE(historico.last().linhasInseridas(), 3);
    QVERIFY(historico.last().executadaEm().isValid());
}

void TesteImportacao::importarDiretorioCasaTickerComAtivo()
{
    // Subdiretorio proprio para nao misturar com os CSVs dos outros testes.
    const QString pasta = QDir(m_diretorio.path()).filePath(QStringLiteral("lote"));
    QVERIFY(QDir().mkpath(pasta));

    const qint64 outroId = cadastrarAtivo(QStringLiteral("TSTE3"));
    QVERIFY(outroId > 0);

    const auto gravar = [&](const QString& nome) {
        QFile arquivo(QDir(pasta).filePath(nome));
        QVERIFY(arquivo.open(QIODevice::WriteOnly | QIODevice::Text));
        QTextStream fluxo(&arquivo);
        for (const QString& linha : linhasValidas(4))
        {
            fluxo << linha << Qt::endl;
        }
        arquivo.close();
    };

    gravar(QStringLiteral("TSTE4.csv"));
    gravar(QStringLiteral("TSTE3.csv"));
    gravar(QStringLiteral("SEMCADASTRO.csv"));

    RepositorioCotacao repositorioCotacao;
    RepositorioAtivo repositorioAtivo;
    RepositorioImportacao repositorioImportacao;
    ServicoImportacao servico(repositorioCotacao, repositorioAtivo, repositorioImportacao);

    const ResultadoImportacao resultado = servico.importarDiretorio(pasta);

    // Os dois ativos cadastrados sao importados; o arquivo sem ativo vira erro.
    QCOMPARE(repositorioCotacao.contarPorAtivo(m_ativoId), 4);
    QCOMPARE(repositorioCotacao.contarPorAtivo(outroId), 4);
    QCOMPARE(resultado.linhasInseridas, 8);
    QCOMPARE(static_cast<int>(resultado.erros.size()), 1);
    QVERIFY(resultado.erros.first().contains(QStringLiteral("SEMCADASTRO")));
}

}

QTEST_APPLESS_MAIN(analisador::TesteImportacao)
#include "testeimportacao.moc"
