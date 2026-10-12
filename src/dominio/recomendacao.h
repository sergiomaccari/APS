#pragma once

// Recomendacao de investimento gerada pelo MotorAnalise para um ativo.
//
// Diagrama de estados (obrigatorio no trabalho):
//     Gerada --marcarComoVigente()--> Vigente --expirar()----> Expirada
//        |                               |
//        +---------invalidar()-----------+--> Invalidada
// Transicoes fora dessa maquina retornam false e nao alteram o objeto.
//
// Os textos de paraTexto() sao exatamente os valores gravados nas colunas
// recomendacao.tipo e recomendacao.estado (ver recursos/sql/001_esquema_inicial.sql).

#include <QDate>
#include <QString>

namespace analisador
{

enum class TipoRecomendacao
{
    Compra,
    Venda,
    Neutro
};

enum class EstadoRecomendacao
{
    Gerada,
    Vigente,
    Expirada,
    Invalidada
};

QString paraTexto(TipoRecomendacao tipo);
TipoRecomendacao tipoRecomendacaoDeTexto(const QString& texto);

QString paraTexto(EstadoRecomendacao estado);
EstadoRecomendacao estadoRecomendacaoDeTexto(const QString& texto);

class Recomendacao
{
public:
    // Limiar de pontuacao que separa Neutro de Compra/Venda no MotorAnalise.
    static const double LIMIAR_DECISAO;

    Recomendacao();
    Recomendacao(qint64 id,
                 qint64 ativoId,
                 TipoRecomendacao tipo,
                 const QString& justificativa,
                 const QString& regraAplicada,
                 const QDate& geradaEm,
                 double pontuacao);

    qint64 id() const;
    qint64 ativoId() const;
    TipoRecomendacao tipo() const;
    QString justificativa() const;
    QString regraAplicada() const;
    QDate geradaEm() const;
    EstadoRecomendacao estado() const;

    // Pontuacao agregada no intervalo [-1, 1]: negativa puxa para venda,
    // positiva para compra, proxima de zero indica ausencia de sinal claro.
    double pontuacao() const;

    void definirId(qint64 id);
    void definirAtivoId(qint64 ativoId);
    void definirTipo(TipoRecomendacao tipo);
    void definirJustificativa(const QString& justificativa);
    void definirRegraAplicada(const QString& regraAplicada);
    void definirGeradaEm(const QDate& geradaEm);
    void definirEstado(EstadoRecomendacao estado);
    void definirPontuacao(double pontuacao);

    // Transicao Gerada -> Vigente (recomendacao publicada ao investidor).
    bool marcarComoVigente();

    // Transicao Vigente -> Expirada (perdeu validade pelo tempo).
    bool expirar();

    // Transicao Gerada|Vigente -> Invalidada (substituida por analise mais recente).
    bool invalidar();

    // Verdadeiro quando a recomendacao esta publicada para o investidor.
    bool estaVigente() const;

    // Texto curto para tabelas, ex.: "Compra (0,62)".
    QString descricaoCurta() const;

    // Classifica uma pontuacao agregada em tipo de recomendacao usando LIMIAR_DECISAO.
    static TipoRecomendacao tipoPorPontuacao(double pontuacao);

private:
    qint64 m_id = 0;
    qint64 m_ativoId = 0;
    TipoRecomendacao m_tipo = TipoRecomendacao::Neutro;
    QString m_justificativa;
    QString m_regraAplicada;
    QDate m_geradaEm;
    EstadoRecomendacao m_estado = EstadoRecomendacao::Gerada;
    double m_pontuacao = 0.0;
};

}
