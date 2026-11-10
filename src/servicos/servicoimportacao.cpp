#include "servicos/servicoimportacao.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSet>
#include <QStringConverter>
#include <QTextStream>

#include "persistencia/bancodedados.h"

namespace analisador
{

const int ServicoImportacao::COLUNAS_ESPERADAS = 6;
const QChar ServicoImportacao::SEPARADOR = QLatin1Char(';');

QString ResultadoImportacao::resumo() const
{
    if (!sucesso)
    {
        return QString::fromUtf8("Importação rejeitada: %1 erro(s) encontrado(s). Nenhuma cotação foi gravada.")
            .arg(erros.size());
    }
    return QString::fromUtf8("Importação concluída: %1 linha(s) lida(s), %2 inserida(s), %3 ignorada(s) por duplicidade.")
        .arg(linhasLidas)
        .arg(linhasInseridas)
        .arg(linhasIgnoradas);
}

ServicoImportacao::ServicoImportacao(RepositorioCotacao& repositorioCotacao,
                                     RepositorioAtivo& repositorioAtivo,
                                     RepositorioImportacao& repositorioImportacao)
    : m_repositorioCotacao(repositorioCotacao)
    , m_repositorioAtivo(repositorioAtivo)
    , m_repositorioImportacao(repositorioImportacao)
{
}

std::optional<double> ServicoImportacao::converterNumero(const QString& texto)
{
    QString normalizado = texto.trimmed();
    if (normalizado.isEmpty())
    {
        return std::nullopt;
    }

    // Padrao brasileiro: a virgula e o separador decimal e o ponto agrupa milhares.
    // Quando nao ha virgula, o ponto e tratado como separador decimal.
    if (normalizado.contains(QLatin1Char(',')))
    {
        normalizado.remove(QLatin1Char('.'));
        normalizado.replace(QLatin1Char(','), QLatin1Char('.'));
    }

    bool ok = false;
    const double valor = normalizado.toDouble(&ok);
    if (!ok)
    {
        return std::nullopt;
    }
    return valor;
}

bool ServicoImportacao::cabecalhoValido(const QString& linha)
{
    const QStringList colunas = linha.split(SEPARADOR);
    if (colunas.size() != COLUNAS_ESPERADAS)
    {
        return false;
    }
    return colunas.at(0).trimmed().compare(QStringLiteral("Data"), Qt::CaseInsensitive) == 0;
}

std::optional<Cotacao> ServicoImportacao::converterLinha(const QString& linha,
                                                        qint64 ativoId,
                                                        int numeroLinha,
                                                        QString* erro)
{
    const auto relatar = [erro, numeroLinha](const QString& mensagem) {
        if (erro != nullptr)
        {
            *erro = QString::fromUtf8("Linha %1: %2").arg(numeroLinha).arg(mensagem);
        }
        return std::nullopt;
    };

    const QStringList colunas = linha.split(SEPARADOR);
    if (colunas.size() != COLUNAS_ESPERADAS)
    {
        return relatar(QString::fromUtf8("esperadas %1 colunas separadas por ';', encontradas %2.")
                           .arg(COLUNAS_ESPERADAS)
                           .arg(colunas.size()));
    }

    const QDate data = QDate::fromString(colunas.at(0).trimmed(), QStringLiteral("dd/MM/yyyy"));
    if (!data.isValid())
    {
        return relatar(QString::fromUtf8("data \"%1\" fora do formato dd/MM/aaaa.")
                           .arg(colunas.at(0).trimmed()));
    }

    const std::optional<double> abertura = converterNumero(colunas.at(1));
    const std::optional<double> maxima = converterNumero(colunas.at(2));
    const std::optional<double> minima = converterNumero(colunas.at(3));
    const std::optional<double> fechamento = converterNumero(colunas.at(4));
    const std::optional<double> volume = converterNumero(colunas.at(5));

    if (!abertura.has_value() || !maxima.has_value() || !minima.has_value()
        || !fechamento.has_value() || !volume.has_value())
    {
        return relatar(QString::fromUtf8("valor numérico inválido em uma das colunas de preço ou volume."));
    }
    if (abertura.value() <= 0.0 || maxima.value() <= 0.0 || minima.value() <= 0.0
        || fechamento.value() <= 0.0)
    {
        return relatar(QString::fromUtf8("preços devem ser maiores que zero."));
    }
    if (maxima.value() < minima.value())
    {
        return relatar(QString::fromUtf8("máxima (%1) menor que a mínima (%2).")
                           .arg(maxima.value(), 0, 'f', 2)
                           .arg(minima.value(), 0, 'f', 2));
    }
    if (abertura.value() < minima.value() || abertura.value() > maxima.value()
        || fechamento.value() < minima.value() || fechamento.value() > maxima.value())
    {
        return relatar(QString::fromUtf8("abertura e fechamento devem ficar entre a mínima e a máxima."));
    }
    if (volume.value() < 0.0)
    {
        return relatar(QString::fromUtf8("volume negativo."));
    }

    Cotacao cotacao;
    cotacao.definirAtivoId(ativoId);
    cotacao.definirData(data);
    cotacao.definirAbertura(abertura.value());
    cotacao.definirMaxima(maxima.value());
    cotacao.definirMinima(minima.value());
    cotacao.definirFechamento(fechamento.value());
    cotacao.definirVolume(static_cast<qint64>(volume.value()));
    return cotacao;
}

ResultadoImportacao ServicoImportacao::importarCsv(const QString& caminho, qint64 ativoId)
{
    m_ultimoErro.clear();
    ResultadoImportacao resultado;

    Importacao registro;
    registro.definirAtivoId(ativoId);
    registro.definirArquivo(caminho);

    const std::unique_ptr<Ativo> ativo = m_repositorioAtivo.buscarPorId(ativoId);
    if (!ativo)
    {
        m_ultimoErro = QString::fromUtf8("Ativo informado não existe no cadastro.");
        resultado.erros.append(m_ultimoErro);
        registro.rejeitar(m_ultimoErro);
        m_repositorioImportacao.salvar(registro);
        return resultado;
    }

    QFile arquivo(caminho);
    if (!arquivo.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        m_ultimoErro = QString::fromUtf8("Não foi possível abrir o arquivo %1.").arg(caminho);
        resultado.erros.append(m_ultimoErro);
        registro.rejeitar(m_ultimoErro);
        m_repositorioImportacao.salvar(registro);
        return resultado;
    }

    registro.iniciarValidacao();

    QTextStream fluxo(&arquivo);
    fluxo.setEncoding(QStringConverter::Utf8);

    QVector<Cotacao> cotacoes;
    QSet<QString> datasNoArquivo;
    int numeroLinha = 0;
    bool cabecalhoLido = false;

    while (!fluxo.atEnd())
    {
        const QString linha = fluxo.readLine();
        ++numeroLinha;

        if (linha.trimmed().isEmpty())
        {
            continue;
        }
        if (!cabecalhoLido)
        {
            cabecalhoLido = true;
            if (cabecalhoValido(linha))
            {
                continue;
            }
            // Sem cabecalho reconhecivel a estrutura do arquivo e considerada invalida.
            resultado.erros.append(QString::fromUtf8(
                "Linha 1: cabeçalho inválido. Esperado \"Data;Abertura;Maxima;Minima;Fechamento;Volume\"."));
            break;
        }

        ++resultado.linhasLidas;
        QString erroLinha;
        const std::optional<Cotacao> cotacao = converterLinha(linha, ativoId, numeroLinha, &erroLinha);
        if (!cotacao.has_value())
        {
            resultado.erros.append(erroLinha);
            continue;
        }

        const QString chaveData = cotacao.value().data().toString(Qt::ISODate);
        if (datasNoArquivo.contains(chaveData))
        {
            resultado.erros.append(QString::fromUtf8("Linha %1: data %2 repetida no próprio arquivo.")
                                       .arg(numeroLinha)
                                       .arg(cotacao.value().data().toString(QStringLiteral("dd/MM/yyyy"))));
            continue;
        }
        datasNoArquivo.insert(chaveData);
        cotacoes.append(cotacao.value());
    }
    arquivo.close();

    if (!resultado.erros.isEmpty())
    {
        // Rejeicao total: nenhuma escrita de cotacao acontece.
        m_ultimoErro = QString::fromUtf8("Arquivo rejeitado com %1 erro(s).").arg(resultado.erros.size());
        registro.rejeitar(resultado.erros.first());
        registro.definirLinhasLidas(resultado.linhasLidas);
        m_repositorioImportacao.salvar(registro);
        return resultado;
    }
    if (cotacoes.isEmpty())
    {
        m_ultimoErro = QString::fromUtf8("O arquivo não contém nenhuma cotação válida.");
        resultado.erros.append(m_ultimoErro);
        registro.rejeitar(m_ultimoErro);
        m_repositorioImportacao.salvar(registro);
        return resultado;
    }

    BancoDeDados& banco = BancoDeDados::instancia();
    if (!banco.iniciarTransacao())
    {
        m_ultimoErro = banco.ultimoErro();
        resultado.erros.append(m_ultimoErro);
        registro.rejeitar(m_ultimoErro);
        registro.definirLinhasLidas(resultado.linhasLidas);
        m_repositorioImportacao.salvar(registro);
        return resultado;
    }

    const int inseridas = m_repositorioCotacao.inserirEmLote(cotacoes);

    // Dupla verificacao proposital: o repositorio devolve -1 quando alguma insercao
    // falha e tambem preenche ultimoErro() (que ele limpa em caso de sucesso).
    // Checar os dois sinais evita que um erro de banco passe por importacao valida.
    const QString erroDoLote = m_repositorioCotacao.ultimoErro();
    if (inseridas < 0 || !erroDoLote.isEmpty())
    {
        banco.desfazer();
        m_ultimoErro = erroDoLote.isEmpty()
            ? QString::fromUtf8("Falha ao gravar as cotações do arquivo.")
            : erroDoLote;
        resultado.erros.append(m_ultimoErro);
        resultado.linhasInseridas = 0;
        resultado.linhasIgnoradas = 0;
        registro.rejeitar(m_ultimoErro);
        registro.definirLinhasLidas(resultado.linhasLidas);
        m_repositorioImportacao.salvar(registro);
        return resultado;
    }

    resultado.linhasInseridas = inseridas;
    resultado.linhasIgnoradas = cotacoes.size() - inseridas;
    resultado.sucesso = true;

    registro.concluir(resultado.linhasLidas, resultado.linhasInseridas);
    // A coluna mensagem_erro e NOT NULL: string vazia, porem nao nula.
    registro.definirMensagemErro(QString::fromUtf8(""));
    if (!m_repositorioImportacao.salvar(registro))
    {
        banco.desfazer();
        m_ultimoErro = m_repositorioImportacao.ultimoErro();
        resultado.sucesso = false;
        resultado.linhasInseridas = 0;
        resultado.linhasIgnoradas = 0;
        resultado.erros.append(m_ultimoErro);
        return resultado;
    }

    if (!banco.confirmar())
    {
        // Commit recusado deixa a transacao aberta: desfazer antes de sair.
        m_ultimoErro = banco.ultimoErro();
        banco.desfazer();
        resultado.sucesso = false;
        resultado.linhasInseridas = 0;
        resultado.linhasIgnoradas = 0;
        resultado.erros.append(m_ultimoErro);
    }
    return resultado;
}

ResultadoImportacao ServicoImportacao::importarDiretorio(const QString& caminho)
{
    m_ultimoErro.clear();
    ResultadoImportacao total;
    total.sucesso = true;

    QDir diretorio(caminho);
    if (!diretorio.exists())
    {
        total.sucesso = false;
        m_ultimoErro = QString::fromUtf8("Diretório %1 não encontrado.").arg(caminho);
        total.erros.append(m_ultimoErro);
        return total;
    }

    const QStringList arquivos = diretorio.entryList(QStringList() << QStringLiteral("*.csv"),
                                                     QDir::Files,
                                                     QDir::Name);
    if (arquivos.isEmpty())
    {
        total.sucesso = false;
        m_ultimoErro = QString::fromUtf8("Nenhum arquivo .csv encontrado em %1.").arg(caminho);
        total.erros.append(m_ultimoErro);
        return total;
    }

    for (const QString& nome : arquivos)
    {
        const QString ticker = QFileInfo(nome).completeBaseName().toUpper();
        const std::unique_ptr<Ativo> ativo = m_repositorioAtivo.buscarPorTicker(ticker);
        if (!ativo)
        {
            total.erros.append(QString::fromUtf8("%1: nenhum ativo cadastrado com o ticker %2.")
                                   .arg(nome, ticker));
            continue;
        }

        const ResultadoImportacao parcial = importarCsv(diretorio.filePath(nome), ativo->id());
        total.linhasLidas += parcial.linhasLidas;
        total.linhasInseridas += parcial.linhasInseridas;
        total.linhasIgnoradas += parcial.linhasIgnoradas;
        for (const QString& erro : parcial.erros)
        {
            total.erros.append(QStringLiteral("%1 - %2").arg(nome, erro));
        }
        if (!parcial.sucesso)
        {
            total.sucesso = false;
        }
    }
    return total;
}

QVector<Importacao> ServicoImportacao::historico(int limite) const
{
    return m_repositorioImportacao.listarRecentes(limite);
}

QString ServicoImportacao::ultimoErro() const
{
    return m_ultimoErro;
}

}
