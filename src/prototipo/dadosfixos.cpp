#include "prototipo/dadosfixos.h"

#include <algorithm>

#include <QDebug>
#include <QFile>
#include <QLocale>
#include <QStringList>
#include <QTextStream>

#include "dominio/acao.h"
#include "dominio/fundoimobiliario.h"

namespace analisador::prototipo
{

namespace
{

// Data de referência das recomendações: último pregão dos CSVs. Função, e não
// constante de namespace, porque QDate não é constexpr: a inicialização
// dinâmica não teria ordem garantida se dados() fosse chamada a partir do
// inicializador estático de outra unidade de tradução.
QDate dataDasRecomendacoes()
{
    return QDate(2026, 8, 14);
}

// Converte número no formato brasileiro do CSV ("41,37").
double numeroDoCsv(const QString& texto)
{
    QString normalizado = texto.trimmed();
    normalizado.replace(QLatin1Char(','), QLatin1Char('.'));
    return normalizado.toDouble();
}

// Lê :/dados/<TICKER>.csv (Data;Abertura;Maxima;Minima;Fechamento;Volume).
// Os arquivos embutidos são os mesmos de dados/, já validados pelo sistema
// completo; por isso a leitura não repete as validações da importação real.
QVector<Cotacao> lerCotacoes(const QString& ticker, qint64 ativoId, qint64& proximaCotacao)
{
    QVector<Cotacao> serie;
    QFile arquivo(QStringLiteral(":/dados/%1.csv").arg(ticker));
    if (!arquivo.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        // Sem o recurso embutido (prototipo.qrc) a série fica vazia e os
        // gráficos saem em branco: avisa em vez de falhar em silêncio.
        qWarning().noquote() << "Protótipo: não foi possível abrir" << arquivo.fileName()
                             << "- a série de" << ticker << "fica vazia.";
        return serie;
    }

    QTextStream fluxo(&arquivo);
    bool cabecalho = true;
    while (!fluxo.atEnd())
    {
        const QString linha = fluxo.readLine().trimmed();
        if (cabecalho)
        {
            cabecalho = false;
            continue;
        }
        const QStringList campos = linha.split(QLatin1Char(';'));
        if (campos.size() != 6)
        {
            continue;
        }
        const QDate data = QDate::fromString(campos.at(0).trimmed(), QStringLiteral("dd/MM/yyyy"));
        if (!data.isValid())
        {
            qWarning().noquote() << "Protótipo: data inválida ignorada em" << arquivo.fileName()
                                 << ":" << linha;
            continue;
        }
        serie.append(Cotacao(proximaCotacao++,
                             ativoId,
                             data,
                             numeroDoCsv(campos.at(1)),
                             numeroDoCsv(campos.at(2)),
                             numeroDoCsv(campos.at(3)),
                             numeroDoCsv(campos.at(4)),
                             campos.at(5).trimmed().toLongLong()));
    }
    std::sort(serie.begin(), serie.end());
    return serie;
}

// Parecer individual de uma regra (o que MotorAnalise::avaliarRegrasIndividualmente produz).
Recomendacao parecer(qint64 ativoId, const QString& regra, double pontuacao, const QString& justificativa)
{
    return Recomendacao(0,
                        ativoId,
                        Recomendacao::tipoPorPontuacao(pontuacao),
                        justificativa,
                        regra,
                        dataDasRecomendacoes(),
                        pontuacao);
}

// Recomendação consolidada com o mesmo texto que MotorAnalise::analisar monta
// (média simples, pois todas as regras têm peso 1,0).
Recomendacao consolidar(qint64 id, const Ativo& ativo, const QVector<Recomendacao>& pareceres)
{
    const QLocale brasil(QLocale::Portuguese, QLocale::Brazil);
    double soma = 0.0;
    QStringList nomes;
    QStringList linhas;
    for (int i = 0; i < pareceres.size(); ++i)
    {
        const Recomendacao& item = pareceres.at(i);
        soma += item.pontuacao();
        nomes.append(item.regraAplicada());
        linhas.append(QStringLiteral("%1. [%2 | %3 | %4] %5")
                          .arg(i + 1)
                          .arg(item.regraAplicada(),
                               paraTexto(item.tipo()),
                               brasil.toString(item.pontuacao(), 'f', 2),
                               item.justificativa()));
    }
    const double pontuacao = soma / pareceres.size();
    const TipoRecomendacao tipo = Recomendacao::tipoPorPontuacao(pontuacao);
    const QString cabecalho =
        QString::fromUtf8(
            "Parecer consolidado de %1 regra(s) para %2 em %3: %4 (pontuação %5, limiar de decisão %6).")
            .arg(pareceres.size())
            .arg(ativo.ticker(),
                 dataDasRecomendacoes().toString(QStringLiteral("dd/MM/yyyy")),
                 paraTexto(tipo),
                 brasil.toString(pontuacao, 'f', 2),
                 brasil.toString(Recomendacao::LIMIAR_DECISAO, 'f', 2));

    Recomendacao consolidada(id,
                             ativo.id(),
                             tipo,
                             cabecalho + QStringLiteral("\n\n") + linhas.join(QStringLiteral("\n")),
                             nomes.join(QStringLiteral(", ")),
                             dataDasRecomendacoes(),
                             pontuacao);
    consolidada.marcarComoVigente();
    return consolidada;
}

void semearUsuarios(DadosFixos& d)
{
    // Hashes iguais aos da semente do sistema completo (admin123 / investidor123).
    d.usuarios.append(Usuario(1,
                              QStringLiteral("Administrador do Sistema"),
                              QStringLiteral("admin@analisadorb3.com"),
                              QStringLiteral("4530d89349b3188c7dd80d2edf0b8fc5b5ea61a6650f35943c86a19296d11482"),
                              PapelUsuario::Administrador));
    d.usuarios.append(Usuario(2,
                              QStringLiteral("Investidor de Demonstracao"),
                              QStringLiteral("investidor@analisadorb3.com"),
                              QStringLiteral("502e7f2bf3488cf83791f055f4e6b73ad7df2f01cc72de14690a0a031c8af111"),
                              PapelUsuario::Investidor));
}

void semearAtivos(DadosFixos& d)
{
    d.ativos.push_back(std::make_unique<Acao>(1,
                                              QStringLiteral("PETR4"),
                                              QStringLiteral("Petroleo Brasileiro S.A. - Petrobras"),
                                              QStringLiteral("Petroleo, Gas e Biocombustiveis"),
                                              4.8, 12.4, 495000000000.0));
    d.ativos.push_back(std::make_unique<Acao>(2,
                                              QStringLiteral("VALE3"),
                                              QStringLiteral("Vale S.A."),
                                              QStringLiteral("Mineracao"),
                                              6.2, 8.1, 280000000000.0));
    d.ativos.push_back(std::make_unique<Acao>(3,
                                              QStringLiteral("ITUB4"),
                                              QStringLiteral("Itau Unibanco Holding S.A."),
                                              QStringLiteral("Bancos"),
                                              9.1, 6.3, 320000000000.0));
    d.ativos.push_back(std::make_unique<FundoImobiliario>(
        4,
        QStringLiteral("HGLG11"),
        QStringLiteral("CSHG Logistica Fundo de Investimento Imobiliario"),
        QStringLiteral("Fundos Imobiliarios"),
        9.2, 168.40, QStringLiteral("Logistica")));
    d.ativos.push_back(std::make_unique<Acao>(5,
                                              QStringLiteral("MGLU3"),
                                              QStringLiteral("Magazine Luiza S.A."),
                                              QStringLiteral("Comercio Varejista"),
                                              28.0, 0.4, 12500000000.0));
}

void semearCotacoes(DadosFixos& d)
{
    qint64 proximaCotacao = 1;
    for (const auto& ativo : d.ativos)
    {
        d.cotacoes.insert(ativo->id(), lerCotacoes(ativo->ticker(), ativo->id(), proximaCotacao));
    }
}

void semearRegras(DadosFixos& d)
{
    d.regras.append(RegraConfigurada(1, QStringLiteral("Cruzamento de Medias Moveis"), true, 9, 21, 1.0));
    d.regras.append(RegraConfigurada(2, QStringLiteral("Indice de Forca Relativa"), true, 30, 70, 1.0));
    d.regras.append(RegraConfigurada(3, QStringLiteral("Dividend Yield"), true, 6, 10, 1.0));
    d.regras.append(RegraConfigurada(4, QStringLiteral("Preco sobre Lucro"), true, 8, 20, 1.0));
}

void semearCarteira(DadosFixos& d)
{
    Carteira carteira(1, 2, QStringLiteral("Carteira Principal"), QDate(2026, 3, 2));
    carteira.definirPosicoes({
        Posicao(1, 1, 1, 200, 36.90, QDate(2026, 3, 5)),
        Posicao(2, 1, 2, 100, 61.25, QDate(2026, 4, 10)),
        Posicao(3, 1, 4, 30, 158.70, QDate(2026, 5, 20)),
    });
    d.carteiras.append(carteira);
}

// Mesma regra de ferramentas/capturar-telas.cpp: PETR4 acima de 97% do último
// fechamento (dispara na primeira avaliação) e VALE3 abaixo de 90% (monitorando).
void semearAlertas(DadosFixos& d)
{
    const QDate criadoEm = momentoDaSemente().date();
    const QVector<Cotacao>& petr = d.cotacoes.value(1);
    const QVector<Cotacao>& vale = d.cotacoes.value(2);
    const double fechamentoPetr = petr.isEmpty() ? 39.25 : petr.last().fechamento();
    const double fechamentoVale = vale.isEmpty() ? 59.86 : vale.last().fechamento();

    d.alertas.append(Alerta(1, 2, 1, TipoCondicaoAlerta::PrecoAcimaDe, fechamentoPetr * 0.97,
                            EstadoAlerta::Ativo, criadoEm));
    d.alertas.append(Alerta(2, 2, 2, TipoCondicaoAlerta::PrecoAbaixoDe, fechamentoVale * 0.90,
                            EstadoAlerta::Ativo, criadoEm));
}

// Pareceres transcritos da execução do motor real sobre os mesmos CSVs
// (capturar-telas, 12/10/2026). As médias dão PETR4 0,717 · VALE3 0,450 ·
// HGLG11 0,375 · ITUB4 −0,079 · MGLU3 −0,367.
void semearRecomendacoes(DadosFixos& d)
{
    const QString cruzamento = QStringLiteral("Cruzamento de Medias Moveis");
    const QString dividendos = QStringLiteral("Dividend Yield");
    const QString precoLucro = QStringLiteral("Preco sobre Lucro");

    d.pareceresPorRegra.insert(4, {
        parecer(4, cruzamento, 0.35, QString::fromUtf8(
            "A média de 9 dias (R$ 158,79) segue acima da média de 21 dias (R$ 157,61) em 14/08/2026: "
            "tendência de alta em curso, sem cruzamento recente.")),
        parecer(4, dividendos, 0.40, QString::fromUtf8(
            "Dividend Yield de 9,20% atende o mínimo exigido (6,0%): bom pagador de proventos.")),
    });
    d.pareceresPorRegra.insert(3, {
        parecer(3, cruzamento, -0.80, QString::fromUtf8(
            "A média de 9 dias (R$ 34,73) cruzou abaixo da média de 21 dias (R$ 34,79) em 14/08/2026, "
            "sinal de reversão para tendência de baixa.")),
        parecer(3, dividendos, 0.40, QString::fromUtf8(
            "Dividend Yield de 6,30% atende o mínimo exigido (6,0%): bom pagador de proventos.")),
        parecer(3, precoLucro, 0.49 / 3.0, QString::fromUtf8(
            "Preço/Lucro de 9,10 está na faixa intermediária (entre 8,0 e 20,0): múltiplo dentro do "
            "esperado, sem sinal forte.")),
    });
    d.pareceresPorRegra.insert(5, {
        parecer(5, cruzamento, -0.35, QString::fromUtf8(
            "A média de 9 dias (R$ 9,82) segue abaixo da média de 21 dias (R$ 10,16) em 14/08/2026: "
            "tendência de baixa em curso, sem cruzamento recente.")),
        parecer(5, dividendos, -0.15, QString::fromUtf8(
            "Dividend Yield de 0,40% está abaixo do mínimo exigido (6,0%): retorno em proventos pouco "
            "atrativo.")),
        parecer(5, precoLucro, -0.60, QString::fromUtf8(
            "Preço/Lucro de 28,00 supera a faixa considerada elevada (20,0): ação cara em relação ao "
            "lucro gerado.")),
    });
    d.pareceresPorRegra.insert(1, {
        parecer(1, cruzamento, 0.80, QString::fromUtf8(
            "A média de 9 dias (R$ 38,22) cruzou acima da média de 21 dias (R$ 38,09) em 14/08/2026, "
            "sinal de reversão para tendência de alta.")),
        parecer(1, dividendos, 0.75, QString::fromUtf8(
            "Dividend Yield de 12,40% supera o patamar de excelência (10,0%): distribuição de proventos "
            "muito acima da média para Ação.")),
        parecer(1, precoLucro, 0.60, QString::fromUtf8(
            "Preço/Lucro de 4,80 está dentro da faixa atrativa (até 8,0): ação negociada a múltiplo baixo "
            "em relação ao lucro.")),
    });
    d.pareceresPorRegra.insert(2, {
        parecer(2, cruzamento, 0.35, QString::fromUtf8(
            "A média de 9 dias (R$ 59,07) segue acima da média de 21 dias (R$ 58,43) em 14/08/2026: "
            "tendência de alta em curso, sem cruzamento recente.")),
        parecer(2, dividendos, 0.40, QString::fromUtf8(
            "Dividend Yield de 8,10% atende o mínimo exigido (6,0%): bom pagador de proventos.")),
        parecer(2, precoLucro, 0.60, QString::fromUtf8(
            "Preço/Lucro de 6,20 está dentro da faixa atrativa (até 8,0): ação negociada a múltiplo baixo "
            "em relação ao lucro.")),
    });

    // Ordem de geração do sistema real (gerarParaTodos segue a ordem de ticker).
    const qint64 ordem[] = {4, 3, 5, 1, 2};
    qint64 id = 1;
    for (const qint64 ativoId : ordem)
    {
        const Ativo* ativo = nullptr;
        for (const auto& candidato : d.ativos)
        {
            if (candidato->id() == ativoId)
            {
                ativo = candidato.get();
            }
        }
        d.recomendacoes.append(consolidar(id++, *ativo, d.pareceresPorRegra.value(ativoId)));
    }
}

void semearImportacoes(DadosFixos& d)
{
    const qint64 ordem[] = {4, 3, 5, 1, 2};
    qint64 id = 1;
    for (const qint64 ativoId : ordem)
    {
        QString ticker;
        for (const auto& candidato : d.ativos)
        {
            if (candidato->id() == ativoId)
            {
                ticker = candidato->ticker();
            }
        }
        const int linhas = d.cotacoes.value(ativoId).size();
        Importacao importacao(id++, ativoId, QStringLiteral("dados/%1.csv").arg(ticker));
        importacao.iniciarValidacao();
        importacao.concluir(linhas, linhas);
        importacao.definirExecutadaEm(momentoDaSemente());
        d.importacoes.append(importacao);
    }
}

DadosFixos criarSemente()
{
    DadosFixos d;
    semearUsuarios(d);
    semearAtivos(d);
    semearCotacoes(d);
    semearRegras(d);
    semearCarteira(d);
    semearAlertas(d);
    semearRecomendacoes(d);
    semearImportacoes(d);
    return d;
}

}

DadosFixos& dados()
{
    static DadosFixos instancia = criarSemente();
    return instancia;
}

qint64 proximoId()
{
    return ++dados().ultimoId;
}

Ativo* ativoPorId(qint64 id)
{
    for (const auto& ativo : dados().ativos)
    {
        if (ativo->id() == id)
        {
            return ativo.get();
        }
    }
    return nullptr;
}

Ativo* ativoPorTicker(const QString& ticker)
{
    const QString procurado = ticker.trimmed();
    for (const auto& ativo : dados().ativos)
    {
        if (ativo->ticker().compare(procurado, Qt::CaseInsensitive) == 0)
        {
            return ativo.get();
        }
    }
    return nullptr;
}

std::unique_ptr<Ativo> clonarAtivo(const Ativo& ativo)
{
    if (const auto* acao = dynamic_cast<const Acao*>(&ativo))
    {
        return std::make_unique<Acao>(acao->id(), acao->ticker(), acao->nomeEmpresa(), acao->setor(),
                                      acao->precoLucro(), acao->dividendYield(), acao->valorMercado());
    }
    if (const auto* fundo = dynamic_cast<const FundoImobiliario*>(&ativo))
    {
        return std::make_unique<FundoImobiliario>(fundo->id(), fundo->ticker(), fundo->nomeEmpresa(),
                                                  fundo->setor(), fundo->dividendYield(),
                                                  fundo->valorPatrimonialPorCota(), fundo->segmento());
    }
    return nullptr;
}

Usuario* usuarioPorId(qint64 id)
{
    for (Usuario& usuario : dados().usuarios)
    {
        if (usuario.id() == id)
        {
            return &usuario;
        }
    }
    return nullptr;
}

Usuario* usuarioPorEmail(const QString& email)
{
    const QString procurado = email.trimmed();
    for (Usuario& usuario : dados().usuarios)
    {
        if (usuario.email().compare(procurado, Qt::CaseInsensitive) == 0)
        {
            return &usuario;
        }
    }
    return nullptr;
}

const QVector<Cotacao>& cotacoesDoAtivo(qint64 ativoId)
{
    static const QVector<Cotacao> vazia;
    const auto& cotacoes = dados().cotacoes;
    const auto encontrada = cotacoes.constFind(ativoId);
    return encontrada == cotacoes.constEnd() ? vazia : encontrada.value();
}

RegraConfigurada* regraPorId(qint64 id)
{
    for (RegraConfigurada& regra : dados().regras)
    {
        if (regra.id() == id)
        {
            return &regra;
        }
    }
    return nullptr;
}

Carteira* carteiraPorId(qint64 id)
{
    for (Carteira& carteira : dados().carteiras)
    {
        if (carteira.id() == id)
        {
            return &carteira;
        }
    }
    return nullptr;
}

Carteira* carteiraDoUsuario(qint64 usuarioId)
{
    for (Carteira& carteira : dados().carteiras)
    {
        if (carteira.usuarioId() == usuarioId)
        {
            return &carteira;
        }
    }
    return nullptr;
}

Alerta* alertaPorId(qint64 id)
{
    for (Alerta& alerta : dados().alertas)
    {
        if (alerta.id() == id)
        {
            return &alerta;
        }
    }
    return nullptr;
}

Recomendacao* recomendacaoVigenteDoAtivo(qint64 ativoId)
{
    // Percorre de trás para frente: a vigente mais recente vence.
    QVector<Recomendacao>& recomendacoes = dados().recomendacoes;
    for (auto it = recomendacoes.rbegin(); it != recomendacoes.rend(); ++it)
    {
        if (it->ativoId() == ativoId && it->estaVigente())
        {
            return &*it;
        }
    }
    return nullptr;
}

const QVector<Recomendacao>& pareceresDoAtivo(qint64 ativoId)
{
    static const QVector<Recomendacao> vazio;
    const auto& pareceres = dados().pareceresPorRegra;
    const auto encontrado = pareceres.constFind(ativoId);
    return encontrado == pareceres.constEnd() ? vazio : encontrado.value();
}

QDateTime momentoDaSemente()
{
    return QDateTime(QDate(2026, 8, 23), QTime(12, 20));
}

QString mensagemImportacaoFixa()
{
    return QString::fromUtf8(
        "Importação concluída: 650 cotações em 5 arquivos; 1 arquivo rejeitado (INVALIDO-exemplo.csv).");
}

QString mensagemSincronizacaoFixa()
{
    return QString::fromUtf8(
        "Sincronização online prevista para a versão final (2º bimestre). Nenhuma cotação alterada.");
}

}
