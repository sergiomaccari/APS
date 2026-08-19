#include "servicos/servicoalerta.h"

#include <algorithm>
#include <memory>

#include <QDateTime>

#include "dominio/cotacao.h"

namespace analisador
{

ServicoAlerta::ServicoAlerta(RepositorioAlerta& repositorioAlerta,
                             RepositorioCotacao& repositorioCotacao,
                             RepositorioAtivo& repositorioAtivo)
    : m_repositorioAlerta(repositorioAlerta)
    , m_repositorioCotacao(repositorioCotacao)
    , m_repositorioAtivo(repositorioAtivo)
{
}

void ServicoAlerta::registrarObservador(ObservadorAlerta* observador)
{
    if (observador == nullptr)
    {
        return;
    }
    if (std::find(m_observadores.begin(), m_observadores.end(), observador) == m_observadores.end())
    {
        m_observadores.push_back(observador);
    }
}

void ServicoAlerta::removerObservador(ObservadorAlerta* observador)
{
    m_observadores.erase(std::remove(m_observadores.begin(), m_observadores.end(), observador),
                         m_observadores.end());
}

void ServicoAlerta::notificar(const Alerta& alerta, const Ativo& ativo, double valorObservado)
{
    for (ObservadorAlerta* observador : m_observadores)
    {
        if (observador != nullptr)
        {
            observador->alertaDisparado(alerta, ativo, valorObservado);
        }
    }
}

bool ServicoAlerta::criarAlerta(qint64 usuarioId,
                                qint64 ativoId,
                                TipoCondicaoAlerta condicao,
                                double valorReferencia,
                                Alerta* criado)
{
    m_ultimoErro.clear();
    if (usuarioId <= 0 || ativoId <= 0)
    {
        m_ultimoErro = QString::fromUtf8("Selecione o ativo do alerta.");
        return false;
    }
    if (valorReferencia <= 0.0)
    {
        m_ultimoErro = QString::fromUtf8("O valor de referência deve ser maior que zero.");
        return false;
    }
    if (!m_repositorioAtivo.buscarPorId(ativoId))
    {
        m_ultimoErro = QString::fromUtf8("Ativo não encontrado no cadastro.");
        return false;
    }

    Alerta alerta;
    alerta.definirUsuarioId(usuarioId);
    alerta.definirAtivoId(ativoId);
    alerta.definirCondicao(condicao);
    alerta.definirValorReferencia(valorReferencia);
    alerta.definirEstado(EstadoAlerta::Ativo);
    alerta.definirCriadoEm(QDate::currentDate());

    if (!m_repositorioAlerta.salvar(alerta))
    {
        m_ultimoErro = m_repositorioAlerta.ultimoErro();
        return false;
    }
    if (criado != nullptr)
    {
        *criado = alerta;
    }
    return true;
}

bool ServicoAlerta::arquivarAlerta(qint64 alertaId)
{
    m_ultimoErro.clear();
    std::optional<Alerta> encontrado = m_repositorioAlerta.buscarPorId(alertaId);
    if (!encontrado.has_value())
    {
        m_ultimoErro = QString::fromUtf8("Alerta não encontrado.");
        return false;
    }

    Alerta alerta = encontrado.value();
    if (!alerta.arquivar())
    {
        m_ultimoErro = QString::fromUtf8(
            "Somente alertas já disparados podem ser arquivados (estado atual: %1).")
            .arg(paraTexto(alerta.estado()));
        return false;
    }
    if (!m_repositorioAlerta.atualizar(alerta))
    {
        m_ultimoErro = m_repositorioAlerta.ultimoErro();
        return false;
    }
    return true;
}

bool ServicoAlerta::removerAlerta(qint64 alertaId)
{
    m_ultimoErro.clear();
    if (!m_repositorioAlerta.remover(alertaId))
    {
        m_ultimoErro = m_repositorioAlerta.ultimoErro();
        return false;
    }
    return true;
}

QVector<Alerta> ServicoAlerta::listarPorUsuario(qint64 usuarioId) const
{
    return m_repositorioAlerta.listarPorUsuario(usuarioId);
}

int ServicoAlerta::avaliarAlertas(qint64 usuarioId)
{
    m_ultimoErro.clear();
    int disparados = 0;

    const QVector<Alerta> monitorados = m_repositorioAlerta.listarMonitoradosPorUsuario(usuarioId);
    for (const Alerta& original : monitorados)
    {
        const std::optional<Cotacao> ultima = m_repositorioCotacao.buscarUltima(original.ativoId());
        if (!ultima.has_value())
        {
            // Sem cotacao importada nao ha o que comparar; o alerta segue monitorando.
            continue;
        }

        // Preco para condicoes de preco; variacao do dia para a condicao de variacao.
        const double valorObservado = original.condicao() == TipoCondicaoAlerta::VariacaoDiariaAcimaDe
            ? ultima.value().variacaoPercentual()
            : ultima.value().fechamento();

        if (!original.condicaoSatisfeita(valorObservado))
        {
            continue;
        }

        Alerta alerta = original;
        if (!alerta.disparar(QDateTime::currentDateTime()))
        {
            continue;
        }
        if (!m_repositorioAlerta.atualizar(alerta))
        {
            m_ultimoErro = m_repositorioAlerta.ultimoErro();
            continue;
        }

        ++disparados;
        const std::unique_ptr<Ativo> ativo = m_repositorioAtivo.buscarPorId(alerta.ativoId());
        if (ativo)
        {
            notificar(alerta, *ativo, valorObservado);
        }
    }
    return disparados;
}

QString ServicoAlerta::ultimoErro() const
{
    return m_ultimoErro;
}

}
