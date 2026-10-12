#pragma once

// Registro de uma importacao de cotacoes a partir de arquivo CSV.
//
// Diagrama de estados (obrigatorio no trabalho):
//     Pendente --iniciarValidacao()--> Validando --concluir()--> Concluida
//         |                                |
//         +----------rejeitar()------------+--> Rejeitada
// A importacao e transacional: quando o estado final e Rejeitada, nenhuma
// cotacao do arquivo foi gravada no banco.
//
// Os textos de paraTexto() sao exatamente os valores gravados na coluna
// importacao.estado (ver recursos/sql/001_esquema_inicial.sql).

#include <QDateTime>
#include <QString>

namespace analisador
{

enum class EstadoImportacao
{
    Pendente,
    Validando,
    Concluida,
    Rejeitada
};

QString paraTexto(EstadoImportacao estado);
EstadoImportacao estadoImportacaoDeTexto(const QString& texto);

class Importacao
{
public:
    Importacao();
    Importacao(qint64 id, qint64 ativoId, const QString& arquivo);

    qint64 id() const;
    qint64 ativoId() const;
    QString arquivo() const;
    EstadoImportacao estado() const;
    int linhasLidas() const;
    int linhasInseridas() const;
    QString mensagemErro() const;
    QDateTime executadaEm() const;

    void definirId(qint64 id);
    void definirAtivoId(qint64 ativoId);
    void definirArquivo(const QString& arquivo);
    void definirEstado(EstadoImportacao estado);
    void definirLinhasLidas(int linhasLidas);
    void definirLinhasInseridas(int linhasInseridas);
    void definirMensagemErro(const QString& mensagemErro);
    void definirExecutadaEm(const QDateTime& executadaEm);

    // Transicao Pendente -> Validando.
    bool iniciarValidacao();

    // Transicao Validando -> Concluida, registrando as contagens do arquivo.
    bool concluir(int linhasLidas, int linhasInseridas);

    // Transicao Pendente|Validando -> Rejeitada, guardando o motivo.
    bool rejeitar(const QString& motivo);

    // Verdadeiro quando a importacao terminou com sucesso.
    bool concluidaComSucesso() const;

    // Nome do arquivo sem o caminho, para exibicao em tabelas.
    QString nomeArquivo() const;

    // Resumo textual: "PETR4.csv - Concluida (120 lidas, 118 inseridas)".
    QString resumo() const;

private:
    qint64 m_id = 0;
    qint64 m_ativoId = 0;
    QString m_arquivo;
    EstadoImportacao m_estado = EstadoImportacao::Pendente;
    int m_linhasLidas = 0;
    int m_linhasInseridas = 0;
    QString m_mensagemErro;
    QDateTime m_executadaEm;
};

}
