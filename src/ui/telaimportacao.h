#pragma once

// Tela de importacao de cotacoes (exclusiva do administrador).
// Permite importar um arquivo CSV para um ativo especifico ou um diretorio
// inteiro (casando TICKER.csv com o cadastro), exibindo o relatorio completo da
// operacao e o historico das importacoes anteriores.
//
// Tambem oferece a sincronizacao online (RF020), que baixa da fonte remota
// apenas o periodo faltante de cada ativo. O CSV continua sendo o caminho
// primario: a sincronizacao e um atalho opcional e pode falhar sem rede.

#include <QWidget>

class QComboBox;
class QLabel;
class QLineEdit;
class QPlainTextEdit;
class QPushButton;
class QTableWidget;

namespace analisador
{

class Contexto;

class TelaImportacao : public QWidget
{
    Q_OBJECT

public:
    explicit TelaImportacao(Contexto& contexto, QWidget* pai = nullptr);

public slots:
    void atualizar();

signals:
    // Cotacoes novas mudam graficos, carteira e recomendacoes.
    void cotacoesImportadas();

private slots:
    void escolherArquivo();
    void importarArquivo();
    void importarDiretorio();
    void sincronizarOnline();

private:
    void carregarAtivos();
    void carregarHistorico();
    void relatar(const QString& resumo, const QStringList& erros, bool sucesso);

    // Liga/desliga os botoes de operacao enquanto uma delas esta em andamento.
    void habilitarOperacoes(bool habilitado);

    // Diretorio de CSVs sugerido: "dados" ao lado do executavel ou do projeto.
    static QString diretorioSugerido();

    Contexto& m_contexto;
    QComboBox* m_campoAtivo = nullptr;
    QLineEdit* m_campoArquivo = nullptr;
    QPushButton* m_botaoEscolher = nullptr;
    QPushButton* m_botaoImportar = nullptr;
    QPushButton* m_botaoDiretorio = nullptr;
    QPushButton* m_botaoSincronizar = nullptr;
    QLabel* m_rotuloResumo = nullptr;
    QPlainTextEdit* m_areaErros = nullptr;
    QTableWidget* m_tabelaHistorico = nullptr;
};

}
