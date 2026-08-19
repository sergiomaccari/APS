#include "ui/telaimportacao.h"

#include <memory>

#include <QApplication>
#include <QComboBox>
#include <QCoreApplication>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

#include "dominio/ativo.h"
#include "ui/contexto.h"

namespace analisador
{

TelaImportacao::TelaImportacao(Contexto& contexto, QWidget* pai)
    : QWidget(pai)
    , m_contexto(contexto)
{
    auto* titulo = new QLabel(QString::fromUtf8("Importação de Cotações"), this);
    titulo->setObjectName(QStringLiteral("tituloTela"));

    auto* explicacao = new QLabel(
        QString::fromUtf8("Formato esperado: Data;Abertura;Maxima;Minima;Fechamento;Volume — "
                          "data dd/mm/aaaa e decimal com vírgula. A importação é transacional: "
                          "qualquer erro estrutural rejeita o arquivo inteiro; datas já "
                          "existentes são ignoradas."),
        this);
    explicacao->setWordWrap(true);
    explicacao->setObjectName(QStringLiteral("textoAuxiliar"));

    m_campoAtivo = new QComboBox(this);
    m_campoAtivo->setMinimumWidth(240);

    m_campoArquivo = new QLineEdit(this);
    m_campoArquivo->setPlaceholderText(QString::fromUtf8("Caminho do arquivo .csv"));

    auto* botaoEscolher = new QPushButton(QStringLiteral("Escolher..."), this);
    m_botaoImportar = new QPushButton(QStringLiteral("Importar arquivo"), this);
    auto* botaoDiretorio = new QPushButton(QString::fromUtf8("Importar pasta inteira"), this);

    auto* linhaAtivo = new QHBoxLayout();
    linhaAtivo->addWidget(new QLabel(QStringLiteral("Ativo:"), this));
    linhaAtivo->addWidget(m_campoAtivo);
    linhaAtivo->addStretch();

    auto* linhaArquivo = new QHBoxLayout();
    linhaArquivo->addWidget(new QLabel(QStringLiteral("Arquivo:"), this));
    linhaArquivo->addWidget(m_campoArquivo, 1);
    linhaArquivo->addWidget(botaoEscolher);
    linhaArquivo->addWidget(m_botaoImportar);
    linhaArquivo->addWidget(botaoDiretorio);

    m_rotuloResumo = new QLabel(QString(), this);
    m_rotuloResumo->setWordWrap(true);
    m_rotuloResumo->setVisible(false);

    m_areaErros = new QPlainTextEdit(this);
    m_areaErros->setReadOnly(true);
    m_areaErros->setMaximumHeight(140);
    m_areaErros->setVisible(false);
    m_areaErros->setPlaceholderText(QStringLiteral("Erros encontrados no arquivo"));

    m_tabelaHistorico = new QTableWidget(this);
    m_tabelaHistorico->setColumnCount(6);
    m_tabelaHistorico->setHorizontalHeaderLabels(QStringList()
                                                 << QStringLiteral("Arquivo")
                                                 << QStringLiteral("Estado")
                                                 << QStringLiteral("Lidas")
                                                 << QStringLiteral("Inseridas")
                                                 << QStringLiteral("Quando")
                                                 << QStringLiteral("Mensagem"));
    m_tabelaHistorico->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tabelaHistorico->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tabelaHistorico->setAlternatingRowColors(true);
    m_tabelaHistorico->verticalHeader()->setVisible(false);
    m_tabelaHistorico->horizontalHeader()->setStretchLastSection(true);

    auto* grupoHistorico = new QGroupBox(QString::fromUtf8("Histórico de importações"), this);
    auto* disposicaoHistorico = new QVBoxLayout(grupoHistorico);
    disposicaoHistorico->addWidget(m_tabelaHistorico);

    auto* disposicao = new QVBoxLayout(this);
    disposicao->setContentsMargins(20, 18, 20, 18);
    disposicao->setSpacing(10);
    disposicao->addWidget(titulo);
    disposicao->addWidget(explicacao);
    disposicao->addLayout(linhaAtivo);
    disposicao->addLayout(linhaArquivo);
    disposicao->addWidget(m_rotuloResumo);
    disposicao->addWidget(m_areaErros);
    disposicao->addWidget(grupoHistorico, 1);

    connect(botaoEscolher, &QPushButton::clicked, this, &TelaImportacao::escolherArquivo);
    connect(m_botaoImportar, &QPushButton::clicked, this, &TelaImportacao::importarArquivo);
    connect(botaoDiretorio, &QPushButton::clicked, this, &TelaImportacao::importarDiretorio);

    atualizar();
}

QString TelaImportacao::diretorioSugerido()
{
    // Procura a pasta "dados" ao lado do executavel e, se nao houver, sobe ate
    // encontrar a do projeto - conveniente tanto na apresentacao quanto no build.
    QDir candidato(QCoreApplication::applicationDirPath());
    for (int nivel = 0; nivel < 4; ++nivel)
    {
        if (candidato.exists(QStringLiteral("dados")))
        {
            return candidato.filePath(QStringLiteral("dados"));
        }
        if (!candidato.cdUp())
        {
            break;
        }
    }
    return QDir::homePath();
}

void TelaImportacao::atualizar()
{
    carregarAtivos();
    carregarHistorico();
}

void TelaImportacao::carregarAtivos()
{
    const qint64 selecionadoAntes = m_campoAtivo->currentData().toLongLong();

    m_campoAtivo->clear();
    const std::vector<std::unique_ptr<Ativo>> ativos = m_contexto.servicoAtivo.listarTodos();
    for (const std::unique_ptr<Ativo>& ativo : ativos)
    {
        if (ativo)
        {
            m_campoAtivo->addItem(ativo->rotuloCompleto(), ativo->id());
        }
    }

    const int indice = m_campoAtivo->findData(selecionadoAntes);
    if (indice >= 0)
    {
        m_campoAtivo->setCurrentIndex(indice);
    }

    const bool temAtivo = m_campoAtivo->count() > 0;
    m_botaoImportar->setEnabled(temAtivo);
    if (!temAtivo)
    {
        relatar(QString::fromUtf8("Cadastre um ativo antes de importar cotações."),
                QStringList(), false);
    }
}

void TelaImportacao::carregarHistorico()
{
    const QVector<Importacao> historico = m_contexto.servicoImportacao.historico(20);
    m_tabelaHistorico->setRowCount(historico.size());

    for (int linha = 0; linha < historico.size(); ++linha)
    {
        const Importacao& registro = historico.at(linha);
        m_tabelaHistorico->setItem(linha, 0, new QTableWidgetItem(registro.nomeArquivo()));
        m_tabelaHistorico->setItem(linha, 1, new QTableWidgetItem(paraTexto(registro.estado())));
        m_tabelaHistorico->setItem(linha, 2, new QTableWidgetItem(QString::number(registro.linhasLidas())));
        m_tabelaHistorico->setItem(linha, 3, new QTableWidgetItem(QString::number(registro.linhasInseridas())));
        m_tabelaHistorico->setItem(
            linha, 4,
            new QTableWidgetItem(registro.executadaEm().isValid()
                                     ? registro.executadaEm().toString(QStringLiteral("dd/MM/yyyy HH:mm"))
                                     : QStringLiteral("--")));
        m_tabelaHistorico->setItem(linha, 5, new QTableWidgetItem(registro.mensagemErro()));
    }
}

void TelaImportacao::relatar(const QString& resumo, const QStringList& erros, bool sucesso)
{
    m_rotuloResumo->setText(resumo);
    m_rotuloResumo->setVisible(!resumo.isEmpty());
    m_rotuloResumo->setStyleSheet(sucesso ? QStringLiteral("color: #2ecc71;")
                                          : QStringLiteral("color: #e74c3c;"));

    if (erros.isEmpty())
    {
        m_areaErros->clear();
        m_areaErros->setVisible(false);
        return;
    }
    m_areaErros->setPlainText(erros.join(QLatin1Char('\n')));
    m_areaErros->setVisible(true);
}

void TelaImportacao::escolherArquivo()
{
    const QString escolhido = QFileDialog::getOpenFileName(
        this,
        QString::fromUtf8("Selecionar arquivo de cotações"),
        diretorioSugerido(),
        QStringLiteral("Arquivos CSV (*.csv);;Todos os arquivos (*)"));

    if (escolhido.isEmpty())
    {
        return;
    }
    m_campoArquivo->setText(escolhido);

    // Conveniencia: se o nome do arquivo for o ticker, ja seleciona o ativo.
    const QString ticker = QFileInfo(escolhido).completeBaseName().toUpper();
    const std::unique_ptr<Ativo> ativo = m_contexto.servicoAtivo.buscarPorTicker(ticker);
    if (ativo)
    {
        const int indice = m_campoAtivo->findData(ativo->id());
        if (indice >= 0)
        {
            m_campoAtivo->setCurrentIndex(indice);
        }
    }
}

void TelaImportacao::importarArquivo()
{
    const QString caminho = m_campoArquivo->text().trimmed();
    if (caminho.isEmpty())
    {
        relatar(QString::fromUtf8("Escolha um arquivo .csv para importar."), QStringList(), false);
        return;
    }
    if (m_campoAtivo->currentIndex() < 0)
    {
        relatar(QString::fromUtf8("Selecione o ativo de destino."), QStringList(), false);
        return;
    }

    const qint64 ativoId = m_campoAtivo->currentData().toLongLong();

    QApplication::setOverrideCursor(Qt::WaitCursor);
    const ResultadoImportacao resultado = m_contexto.servicoImportacao.importarCsv(caminho, ativoId);
    QApplication::restoreOverrideCursor();

    relatar(resultado.resumo(), resultado.erros, resultado.sucesso);
    carregarHistorico();

    if (resultado.sucesso)
    {
        emit cotacoesImportadas();
    }
}

void TelaImportacao::importarDiretorio()
{
    const QString diretorio = QFileDialog::getExistingDirectory(
        this,
        QString::fromUtf8("Selecionar pasta com arquivos TICKER.csv"),
        diretorioSugerido());

    if (diretorio.isEmpty())
    {
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    const ResultadoImportacao resultado = m_contexto.servicoImportacao.importarDiretorio(diretorio);
    QApplication::restoreOverrideCursor();

    relatar(resultado.resumo(), resultado.erros, resultado.sucesso);
    carregarHistorico();

    if (resultado.linhasInseridas > 0)
    {
        emit cotacoesImportadas();
    }
}

}
