#pragma once

// Alerta cadastrado por um usuario sobre um ativo.
//
// Diagrama de estados (obrigatorio no trabalho):
//     Ativo --disparar()--> Disparado --arquivar()--> Arquivado
// Qualquer transicao fora dessa sequencia retorna false e nao altera o objeto.
//
// Os textos de paraTexto() sao exatamente os valores gravados nas colunas
// alerta.estado e alerta.condicao (ver recursos/sql/001_esquema_inicial.sql).

#include <QDate>
#include <QDateTime>
#include <QString>

namespace analisador
{

enum class EstadoAlerta
{
    Ativo,
    Disparado,
    Arquivado
};

enum class TipoCondicaoAlerta
{
    PrecoAcimaDe,
    PrecoAbaixoDe,
    VariacaoDiariaAcimaDe
};

// Texto canonico do estado (interface e persistencia).
QString paraTexto(EstadoAlerta estado);
EstadoAlerta estadoAlertaDeTexto(const QString& texto);

// Texto canonico da condicao (interface e persistencia).
QString paraTexto(TipoCondicaoAlerta condicao);
TipoCondicaoAlerta tipoCondicaoAlertaDeTexto(const QString& texto);

// Texto amigavel da condicao para exibicao ao usuario ("Preço acima de", etc.).
QString descricaoCondicao(TipoCondicaoAlerta condicao);

class Alerta
{
public:
    Alerta();
    Alerta(qint64 id,
           qint64 usuarioId,
           qint64 ativoId,
           TipoCondicaoAlerta condicao,
           double valorReferencia,
           EstadoAlerta estado,
           const QDate& criadoEm);

    qint64 id() const;
    qint64 usuarioId() const;
    qint64 ativoId() const;
    TipoCondicaoAlerta condicao() const;
    double valorReferencia() const;
    EstadoAlerta estado() const;
    QDate criadoEm() const;
    QDateTime disparadoEm() const;

    void definirId(qint64 id);
    void definirUsuarioId(qint64 usuarioId);
    void definirAtivoId(qint64 ativoId);
    void definirCondicao(TipoCondicaoAlerta condicao);
    void definirValorReferencia(double valorReferencia);
    void definirEstado(EstadoAlerta estado);
    void definirCriadoEm(const QDate& criadoEm);
    void definirDisparadoEm(const QDateTime& disparadoEm);

    // Verdadeiro quando o valor observado satisfaz a condicao cadastrada.
    // Para PrecoAcimaDe e PrecoAbaixoDe o valor observado e o preco de fechamento.
    // Para VariacaoDiariaAcimaDe o valor observado e a variacao percentual do dia.
    bool condicaoSatisfeita(double valorObservado) const;

    // Transicao Ativo -> Disparado. Registra o momento do disparo.
    bool disparar(const QDateTime& momento);

    // Transicao Disparado -> Arquivado.
    bool arquivar();

    // Verdadeiro quando o alerta ainda esta sendo monitorado.
    bool estaMonitorando() const;

    // Descricao legivel do alerta, ex.: "Preço acima de R$ 42,00".
    QString descricao() const;

private:
    qint64 m_id = 0;
    qint64 m_usuarioId = 0;
    qint64 m_ativoId = 0;
    TipoCondicaoAlerta m_condicao = TipoCondicaoAlerta::PrecoAcimaDe;
    double m_valorReferencia = 0.0;
    EstadoAlerta m_estado = EstadoAlerta::Ativo;
    QDate m_criadoEm;
    QDateTime m_disparadoEm;
};

}
