// Serviços do protótipo de telas (entrega de 13/10):
// mesmas classes de src/servicos/, mas sobre o estado em memória de DadosFixos.
//
// Este arquivo substitui, nos alvos do protótipo, os .cpp reais de serviços.
// Os headers de src/servicos/ continuam sendo o contrato. Regras deste stub:
// - As validações e as mensagens de ultimoErro() são as mesmas do sistema real,
//   para a interface se comportar igual; as ordens de listagem imitam os
//   ORDER BY dos repositórios reais.
// - Os serviços leem e escrevem direto em dados(); os repositórios recebidos no
//   construtor só são guardados (os membros referência exigem).
// - Nada de banco, arquivo ou rede: importação e sincronização devolvem
//   resultados pré-prontos com as mensagens fixas e
//   recomendações não são calculadas, apenas lidas da semente.
// - O "agora" do protótipo é momentoDaSemente() (23/08/2026 12:20), a mesma
//   data das figuras do documento.

#include <algorithm>
#include <memory>

#include <QCryptographicHash>
#include <QRegularExpression>

#include "prototipo/dadosfixos.h"
#include "servicos/fonteyahoofinance.h"
#include "servicos/servicoalerta.h"
#include "servicos/servicoativo.h"
#include "servicos/servicoautenticacao.h"
#include "servicos/servicocarteira.h"
#include "servicos/servicoimportacao.h"
#include "servicos/servicorecomendacao.h"
#include "servicos/servicosincronizacao.h"

namespace analisador
{

using prototipo::dados;

namespace
{

// Cotações de demonstração lidas por arquivo e total de arquivos aceitos na
// importação pré-pronta (5 CSVs de dados/ com 130 pregões cada).
const int COTACOES_POR_ARQUIVO = 130;
const int ARQUIVOS_ACEITOS = 5;

int contarAdministradores()
{
    int total = 0;
    for (const Usuario& usuario : dados().usuarios)
    {
        if (usuario.ehAdministrador())
        {
            ++total;
        }
    }
    return total;
}

// Mesma ordem do repositório real: criado_em DESC, id DESC.
void ordenarAlertas(QVector<Alerta>& alertas)
{
    std::sort(alertas.begin(), alertas.end(), [](const Alerta& a, const Alerta& b) {
        if (a.criadoEm() != b.criadoEm())
        {
            return a.criadoEm() > b.criadoEm();
        }
        return a.id() > b.id();
    });
}

// Cópia da carteira com as posições em ordem de ativo_id, como o real lê.
Carteira copiaOrdenada(const Carteira& carteira)
{
    Carteira copia = carteira;
    QVector<Posicao> posicoes = carteira.posicoes();
    std::sort(posicoes.begin(), posicoes.end(), [](const Posicao& a, const Posicao& b) {
        return a.ativoId() < b.ativoId();
    });
    copia.definirPosicoes(posicoes);
    return copia;
}

// Remove tudo o que depende do ativo, como o ON DELETE CASCADE do esquema real
// (cotacao, posicao, alerta, recomendacao e importacao).
void removerDependentesDoAtivo(qint64 ativoId)
{
    prototipo::DadosFixos& d = dados();
    d.cotacoes.remove(ativoId);
    d.pareceresPorRegra.remove(ativoId);
    for (Carteira& carteira : d.carteiras)
    {
        carteira.removerPosicaoDoAtivo(ativoId);
    }
    d.alertas.erase(std::remove_if(d.alertas.begin(), d.alertas.end(),
                                   [ativoId](const Alerta& a) { return a.ativoId() == ativoId; }),
                    d.alertas.end());
    d.recomendacoes.erase(std::remove_if(d.recomendacoes.begin(), d.recomendacoes.end(),
                                         [ativoId](const Recomendacao& r) { return r.ativoId() == ativoId; }),
                          d.recomendacoes.end());
    d.importacoes.erase(std::remove_if(d.importacoes.begin(), d.importacoes.end(),
                                       [ativoId](const Importacao& i) { return i.ativoId() == ativoId; }),
                        d.importacoes.end());
}

// Acrescenta ao histórico uma importação concluída no momento fixo do protótipo.
void registrarImportacao(qint64 ativoId, const QString& arquivo, int lidas, int inseridas)
{
    Importacao registro;
    registro.definirId(prototipo::proximoId());
    registro.definirAtivoId(ativoId);
    registro.definirArquivo(arquivo);
    registro.iniciarValidacao();
    registro.concluir(lidas, inseridas);
    registro.definirMensagemErro(QString::fromUtf8(""));
    registro.definirExecutadaEm(prototipo::momentoDaSemente());
    dados().importacoes.append(registro);
}

}

// ===========================================================================
// ServicoAutenticacao
// ===========================================================================

const QString ServicoAutenticacao::SAL = QStringLiteral("aps-b3-2026");
const int ServicoAutenticacao::TAMANHO_MINIMO_SENHA = 6;

ServicoAutenticacao::ServicoAutenticacao(RepositorioUsuario& repositorio)
    : m_repositorio(repositorio)
{
}

QString ServicoAutenticacao::gerarHash(const QString& senha)
{
    const QByteArray entrada = (SAL + senha).toUtf8();
    return QString::fromLatin1(QCryptographicHash::hash(entrada, QCryptographicHash::Sha256).toHex());
}

bool ServicoAutenticacao::emailPareceValido(const QString& email)
{
    static const QRegularExpression padrao(QStringLiteral("^[^@\\s]+@[^@\\s]+\\.[A-Za-z]{2,}$"));
    return padrao.match(email.trimmed()).hasMatch();
}

std::optional<Usuario> ServicoAutenticacao::autenticar(const QString& email, const QString& senha)
{
    m_ultimoErro.clear();
    const QString emailNormalizado = email.trimmed().toLower();
    if (emailNormalizado.isEmpty() || senha.isEmpty())
    {
        m_ultimoErro = QString::fromUtf8("Informe e-mail e senha.");
        return std::nullopt;
    }

    const Usuario* encontrado = prototipo::usuarioPorEmail(emailNormalizado);
    if (encontrado == nullptr || encontrado->senhaHash() != gerarHash(senha))
    {
        // Mensagem genérica de propósito: não revela se o e-mail existe.
        m_ultimoErro = QString::fromUtf8("E-mail ou senha inválidos.");
        return std::nullopt;
    }
    return *encontrado;
}

bool ServicoAutenticacao::cadastrarUsuario(const QString& nome,
                                           const QString& email,
                                           const QString& senha,
                                           PapelUsuario papel,
                                           Usuario* criado)
{
    m_ultimoErro.clear();
    const QString nomeNormalizado = nome.trimmed();
    const QString emailNormalizado = email.trimmed().toLower();

    if (nomeNormalizado.isEmpty())
    {
        m_ultimoErro = QString::fromUtf8("Informe o nome do usuário.");
        return false;
    }
    if (!emailPareceValido(emailNormalizado))
    {
        m_ultimoErro = QString::fromUtf8("Informe um e-mail válido.");
        return false;
    }
    if (senha.size() < TAMANHO_MINIMO_SENHA)
    {
        m_ultimoErro = QString::fromUtf8("A senha deve ter ao menos %1 caracteres.")
                           .arg(TAMANHO_MINIMO_SENHA);
        return false;
    }
    if (prototipo::usuarioPorEmail(emailNormalizado) != nullptr)
    {
        m_ultimoErro = QString::fromUtf8("Já existe um usuário com o e-mail %1.").arg(emailNormalizado);
        return false;
    }

    Usuario usuario;
    usuario.definirId(prototipo::proximoId());
    usuario.definirNome(nomeNormalizado);
    usuario.definirEmail(emailNormalizado);
    usuario.definirSenhaHash(gerarHash(senha));
    usuario.definirPapel(papel);
    dados().usuarios.append(usuario);

    if (criado != nullptr)
    {
        *criado = usuario;
    }
    return true;
}

bool ServicoAutenticacao::atualizarUsuario(const Usuario& usuario)
{
    m_ultimoErro.clear();
    Usuario normalizado = usuario;
    normalizado.definirNome(usuario.nome().trimmed());
    normalizado.definirEmail(usuario.email().trimmed().toLower());

    if (usuario.id() <= 0)
    {
        m_ultimoErro = QString::fromUtf8("Usuário sem identificador não pode ser atualizado.");
        return false;
    }
    if (usuario.nome().trimmed().isEmpty())
    {
        m_ultimoErro = QString::fromUtf8("Informe o nome do usuário.");
        return false;
    }
    if (!emailPareceValido(usuario.email()))
    {
        m_ultimoErro = QString::fromUtf8("Informe um e-mail válido.");
        return false;
    }

    const Usuario* mesmoEmail = prototipo::usuarioPorEmail(normalizado.email());
    if (mesmoEmail != nullptr && mesmoEmail->id() != usuario.id())
    {
        m_ultimoErro = QString::fromUtf8("O e-mail %1 já pertence a outro usuário.").arg(usuario.email());
        return false;
    }

    Usuario* atual = prototipo::usuarioPorId(usuario.id());
    if (atual == nullptr)
    {
        m_ultimoErro = QString::fromUtf8("Usuário não encontrado.");
        return false;
    }
    // Rebaixar o último administrador deixaria o sistema sem administração.
    if (atual->ehAdministrador() && !usuario.ehAdministrador() && contarAdministradores() <= 1)
    {
        m_ultimoErro = QString::fromUtf8("Este é o único administrador: o papel não pode ser alterado.");
        return false;
    }

    // A senha não muda nesta operação: mantém o hash já guardado.
    normalizado.definirSenhaHash(atual->senhaHash());
    *atual = normalizado;
    return true;
}

bool ServicoAutenticacao::alterarSenha(qint64 usuarioId, const QString& novaSenha)
{
    m_ultimoErro.clear();
    if (novaSenha.size() < TAMANHO_MINIMO_SENHA)
    {
        m_ultimoErro = QString::fromUtf8("A senha deve ter ao menos %1 caracteres.")
                           .arg(TAMANHO_MINIMO_SENHA);
        return false;
    }

    Usuario* usuario = prototipo::usuarioPorId(usuarioId);
    if (usuario == nullptr)
    {
        m_ultimoErro = QString::fromUtf8("Usuário não encontrado.");
        return false;
    }
    usuario->definirSenhaHash(gerarHash(novaSenha));
    return true;
}

bool ServicoAutenticacao::removerUsuario(qint64 usuarioId)
{
    m_ultimoErro.clear();
    const Usuario* encontrado = prototipo::usuarioPorId(usuarioId);
    if (encontrado == nullptr)
    {
        m_ultimoErro = QString::fromUtf8("Usuário não encontrado.");
        return false;
    }
    if (encontrado->ehAdministrador() && contarAdministradores() <= 1)
    {
        m_ultimoErro = QString::fromUtf8("O último administrador do sistema não pode ser removido.");
        return false;
    }

    // ON DELETE CASCADE do esquema real: carteiras (com posições) e alertas do usuário.
    prototipo::DadosFixos& d = dados();
    d.usuarios.erase(std::remove_if(d.usuarios.begin(), d.usuarios.end(),
                                    [usuarioId](const Usuario& u) { return u.id() == usuarioId; }),
                     d.usuarios.end());
    d.carteiras.erase(std::remove_if(d.carteiras.begin(), d.carteiras.end(),
                                     [usuarioId](const Carteira& c) { return c.usuarioId() == usuarioId; }),
                      d.carteiras.end());
    d.alertas.erase(std::remove_if(d.alertas.begin(), d.alertas.end(),
                                   [usuarioId](const Alerta& a) { return a.usuarioId() == usuarioId; }),
                    d.alertas.end());
    return true;
}

QVector<Usuario> ServicoAutenticacao::listarUsuarios() const
{
    // Mesmo ORDER BY nome do repositório real.
    QVector<Usuario> usuarios = dados().usuarios;
    std::sort(usuarios.begin(), usuarios.end(), [](const Usuario& a, const Usuario& b) {
        return a.nome() < b.nome();
    });
    return usuarios;
}

QString ServicoAutenticacao::ultimoErro() const
{
    return m_ultimoErro;
}

// ===========================================================================
// ServicoAtivo
// ===========================================================================

ServicoAtivo::ServicoAtivo(RepositorioAtivo& repositorio)
    : m_repositorio(repositorio)
{
}

bool ServicoAtivo::tickerValido(const QString& ticker)
{
    static const QRegularExpression padrao(QStringLiteral("^[A-Z0-9]{4,6}$"));
    return padrao.match(ticker.trimmed().toUpper()).hasMatch();
}

bool ServicoAtivo::salvar(Ativo& ativo)
{
    m_ultimoErro.clear();

    if (!tickerValido(ativo.ticker()))
    {
        m_ultimoErro = QString::fromUtf8(
            "Ticker inválido: use de 4 a 6 caracteres entre letras e números (ex.: PETR4).");
        return false;
    }
    if (ativo.nomeEmpresa().trimmed().isEmpty())
    {
        m_ultimoErro = QString::fromUtf8("Informe o nome da empresa ou do fundo.");
        return false;
    }
    if (ativo.setor().trimmed().isEmpty())
    {
        m_ultimoErro = QString::fromUtf8("Informe o setor do ativo.");
        return false;
    }
    const Ativo* mesmoTicker = prototipo::ativoPorTicker(ativo.ticker().trimmed());
    if (mesmoTicker != nullptr && mesmoTicker->id() != ativo.id())
    {
        m_ultimoErro = QString::fromUtf8("Já existe um ativo cadastrado com o ticker %1.")
                           .arg(ativo.ticker().trimmed().toUpper());
        return false;
    }

    ativo.definirTicker(ativo.ticker().trimmed().toUpper());

    std::vector<std::unique_ptr<Ativo>>& ativos = dados().ativos;
    if (ativo.id() == 0)
    {
        ativo.definirId(prototipo::proximoId());
        ativos.push_back(prototipo::clonarAtivo(ativo));
        return true;
    }

    for (std::unique_ptr<Ativo>& existente : ativos)
    {
        if (existente && existente->id() == ativo.id())
        {
            existente = prototipo::clonarAtivo(ativo);
            return true;
        }
    }
    m_ultimoErro = QString::fromUtf8("Ativo não encontrado no cadastro.");
    return false;
}

bool ServicoAtivo::remover(qint64 id)
{
    m_ultimoErro.clear();
    if (id <= 0)
    {
        m_ultimoErro = QString::fromUtf8("Selecione um ativo válido para remover.");
        return false;
    }

    std::vector<std::unique_ptr<Ativo>>& ativos = dados().ativos;
    ativos.erase(std::remove_if(ativos.begin(), ativos.end(),
                                [id](const std::unique_ptr<Ativo>& a) { return a && a->id() == id; }),
                 ativos.end());
    removerDependentesDoAtivo(id);
    return true;
}

std::vector<std::unique_ptr<Ativo>> ServicoAtivo::listarTodos() const
{
    return m_repositorio.listarTodos();
}

std::vector<std::unique_ptr<Ativo>> ServicoAtivo::buscar(const QString& termo) const
{
    const QString procurado = termo.trimmed();
    std::vector<std::unique_ptr<Ativo>> todos = m_repositorio.listarTodos();
    if (procurado.isEmpty())
    {
        return todos;
    }

    // Mesmo critério do LIKE do real: parte do ticker ou do nome, sem caixa.
    std::vector<std::unique_ptr<Ativo>> encontrados;
    for (std::unique_ptr<Ativo>& ativo : todos)
    {
        if (ativo->ticker().contains(procurado, Qt::CaseInsensitive)
            || ativo->nomeEmpresa().contains(procurado, Qt::CaseInsensitive))
        {
            encontrados.push_back(std::move(ativo));
        }
    }
    return encontrados;
}

std::unique_ptr<Ativo> ServicoAtivo::buscarPorId(qint64 id) const
{
    const Ativo* ativo = prototipo::ativoPorId(id);
    return ativo == nullptr ? nullptr : prototipo::clonarAtivo(*ativo);
}

std::unique_ptr<Ativo> ServicoAtivo::buscarPorTicker(const QString& ticker) const
{
    return m_repositorio.buscarPorTicker(ticker.trimmed().toUpper());
}

QString ServicoAtivo::ultimoErro() const
{
    return m_ultimoErro;
}

// ===========================================================================
// ServicoImportacao
// ===========================================================================

QString ResultadoImportacao::resumo() const
{
    if (!sucesso)
    {
        return QString::fromUtf8("Importação rejeitada: %1 erro(s) encontrado(s). Nenhuma cotação foi gravada.")
            .arg(erros.size());
    }
    // Resultado pré-pronto do protótipo. Só importarDiretorio preenche erros
    // (o exemplo inválido rejeitado): sem erros, o resultado é de um arquivo só.
    if (erros.isEmpty())
    {
        return QString::fromUtf8("Importação concluída: %1 cotações em 1 arquivo.").arg(linhasLidas);
    }
    return prototipo::mensagemImportacaoFixa();
}

ServicoImportacao::ServicoImportacao(RepositorioCotacao& repositorioCotacao,
                                     RepositorioAtivo& repositorioAtivo,
                                     RepositorioImportacao& repositorioImportacao)
    : m_repositorioCotacao(repositorioCotacao)
    , m_repositorioAtivo(repositorioAtivo)
    , m_repositorioImportacao(repositorioImportacao)
{
}

ResultadoImportacao ServicoImportacao::importarCsv(const QString& caminho, qint64 ativoId)
{
    m_ultimoErro.clear();
    ResultadoImportacao resultado;

    if (prototipo::ativoPorId(ativoId) == nullptr)
    {
        m_ultimoErro = QString::fromUtf8("Ativo informado não existe no cadastro.");
        resultado.erros.append(m_ultimoErro);
        return resultado;
    }

    // O arquivo não é lido: as cotações de demonstração do ativo já estão
    // carregadas, então todas as linhas "lidas" seriam ignoradas por duplicidade.
    // Ativo cadastrado durante a apresentação (sem série na semente): usa o
    // tamanho de um CSV de demonstração.
    const qsizetype naSemente = dados().cotacoes.value(ativoId).size();
    const int cotacoesDoAtivo = naSemente > 0 ? static_cast<int>(naSemente) : COTACOES_POR_ARQUIVO;
    resultado.sucesso = true;
    resultado.linhasLidas = cotacoesDoAtivo;
    resultado.linhasInseridas = 0;
    resultado.linhasIgnoradas = cotacoesDoAtivo;
    registrarImportacao(ativoId, caminho, resultado.linhasLidas, resultado.linhasInseridas);
    return resultado;
}

ResultadoImportacao ServicoImportacao::importarDiretorio(const QString& caminho)
{
    m_ultimoErro.clear();
    ResultadoImportacao total;

    // Nenhum arquivo é aberto: resultado fixo de 5 arquivos aceitos e o exemplo
    // inválido rejeitado, com o mesmo texto de erro que o real produz para ele.
    total.sucesso = true;
    total.linhasLidas = COTACOES_POR_ARQUIVO * ARQUIVOS_ACEITOS;
    total.linhasInseridas = 0;
    total.linhasIgnoradas = total.linhasLidas;
    total.erros.append(QString::fromUtf8("%1: nenhum ativo cadastrado com o ticker %2.")
                           .arg(QStringLiteral("INVALIDO-exemplo.csv"),
                                QStringLiteral("INVALIDO-EXEMPLO")));

    // Uma única linha no histórico para a pasta inteira (sem ativo específico).
    registrarImportacao(0, caminho, total.linhasLidas, total.linhasInseridas);
    return total;
}

QVector<Importacao> ServicoImportacao::historico(int limite) const
{
    // Mesmo ORDER BY id DESC LIMIT do repositório real.
    QVector<Importacao> recentes = dados().importacoes;
    std::reverse(recentes.begin(), recentes.end());
    if (limite > 0 && recentes.size() > limite)
    {
        recentes.resize(limite);
    }
    return recentes;
}

QString ServicoImportacao::ultimoErro() const
{
    return m_ultimoErro;
}

// ===========================================================================
// ServicoCarteira
// ===========================================================================

bool ResumoCarteira::vazia() const
{
    return linhas.isEmpty();
}

ServicoCarteira::ServicoCarteira(RepositorioCarteira& repositorioCarteira,
                                 RepositorioCotacao& repositorioCotacao,
                                 RepositorioAtivo& repositorioAtivo)
    : m_repositorioCarteira(repositorioCarteira)
    , m_repositorioCotacao(repositorioCotacao)
    , m_repositorioAtivo(repositorioAtivo)
{
}

std::optional<Carteira> ServicoCarteira::carteiraDoUsuario(qint64 usuarioId)
{
    m_ultimoErro.clear();
    if (usuarioId <= 0)
    {
        m_ultimoErro = QString::fromUtf8("Usuário inválido.");
        return std::nullopt;
    }

    const Carteira* existente = prototipo::carteiraDoUsuario(usuarioId);
    if (existente != nullptr)
    {
        return copiaOrdenada(*existente);
    }

    // Primeiro acesso do investidor: cria a carteira principal automaticamente.
    Carteira nova;
    nova.definirId(prototipo::proximoId());
    nova.definirUsuarioId(usuarioId);
    nova.definirNome(QStringLiteral("Carteira Principal"));
    nova.definirCriadaEm(prototipo::momentoDaSemente().date());
    dados().carteiras.append(nova);
    return nova;
}

bool ServicoCarteira::comprar(qint64 carteiraId,
                              qint64 ativoId,
                              int quantidade,
                              double preco,
                              const QDate& data)
{
    m_ultimoErro.clear();
    if (carteiraId <= 0 || ativoId <= 0)
    {
        m_ultimoErro = QString::fromUtf8("Selecione a carteira e o ativo.");
        return false;
    }
    if (quantidade <= 0)
    {
        m_ultimoErro = QString::fromUtf8("A quantidade comprada deve ser maior que zero.");
        return false;
    }
    if (preco <= 0.0)
    {
        m_ultimoErro = QString::fromUtf8("O preço de compra deve ser maior que zero.");
        return false;
    }

    Carteira* carteira = prototipo::carteiraPorId(carteiraId);
    if (carteira == nullptr || prototipo::ativoPorId(ativoId) == nullptr)
    {
        m_ultimoErro = QString::fromUtf8("Selecione a carteira e o ativo.");
        return false;
    }

    Posicao* existente = carteira->posicaoDoAtivo(ativoId);
    if (existente != nullptr)
    {
        // Preço médio ponderado calculado pelo domínio.
        if (!existente->incorporarCompra(quantidade, preco))
        {
            m_ultimoErro = QString::fromUtf8("Não foi possível incorporar a compra à posição existente.");
            return false;
        }
        return true;
    }

    Posicao nova;
    nova.definirId(prototipo::proximoId());
    nova.definirCarteiraId(carteiraId);
    nova.definirAtivoId(ativoId);
    nova.definirQuantidade(quantidade);
    nova.definirPrecoMedio(preco);
    nova.definirCompradaEm(data.isValid() ? data : prototipo::momentoDaSemente().date());
    carteira->adicionarPosicao(nova);
    return true;
}

bool ServicoCarteira::vender(qint64 carteiraId, qint64 ativoId, int quantidade)
{
    m_ultimoErro.clear();
    if (quantidade <= 0)
    {
        m_ultimoErro = QString::fromUtf8("A quantidade vendida deve ser maior que zero.");
        return false;
    }

    Carteira* carteira = prototipo::carteiraPorId(carteiraId);
    Posicao* posicao = carteira == nullptr ? nullptr : carteira->posicaoDoAtivo(ativoId);
    if (posicao == nullptr)
    {
        m_ultimoErro = QString::fromUtf8("Não há posição desse ativo na carteira.");
        return false;
    }

    if (quantidade > posicao->quantidade())
    {
        m_ultimoErro = QString::fromUtf8("Você possui apenas %1 unidade(s); não é possível vender %2.")
                           .arg(posicao->quantidade())
                           .arg(quantidade);
        return false;
    }

    if (!posicao->venderParcial(quantidade))
    {
        m_ultimoErro = QString::fromUtf8("Não foi possível registrar a venda.");
        return false;
    }

    if (posicao->zerada())
    {
        // Posição totalmente vendida deixa de existir na carteira.
        carteira->removerPosicaoDoAtivo(ativoId);
    }
    return true;
}

ResumoCarteira ServicoCarteira::calcularResumo(qint64 carteiraId) const
{
    ResumoCarteira resumo;
    const Carteira* carteira = prototipo::carteiraPorId(carteiraId);
    if (carteira == nullptr)
    {
        return resumo;
    }

    const QVector<Posicao> posicoes = copiaOrdenada(*carteira).posicoes();
    for (const Posicao& posicao : posicoes)
    {
        LinhaResumoCarteira linha;
        linha.ativoId = posicao.ativoId();
        linha.quantidade = posicao.quantidade();
        linha.precoMedio = posicao.precoMedio();
        linha.valorInvestido = posicao.custoTotal();

        const Ativo* ativo = prototipo::ativoPorId(posicao.ativoId());
        if (ativo != nullptr)
        {
            linha.ticker = ativo->ticker();
            linha.nomeAtivo = ativo->nomeEmpresa();
        }
        else
        {
            linha.ticker = QString::fromUtf8("(removido)");
        }

        // Último fechamento das cotações de demonstração.
        const QVector<Cotacao>& serie = prototipo::cotacoesDoAtivo(posicao.ativoId());
        if (!serie.isEmpty())
        {
            linha.temCotacao = true;
            linha.precoAtual = serie.last().fechamento();
        }
        else
        {
            linha.temCotacao = false;
            linha.precoAtual = posicao.precoMedio();
        }

        linha.valorAtual = posicao.valorAtual(linha.precoAtual);
        linha.lucroPrejuizo = posicao.lucroPrejuizo(linha.precoAtual);
        linha.rentabilidadePercentual = posicao.rentabilidadePercentual(linha.precoAtual);

        resumo.valorInvestido += linha.valorInvestido;
        resumo.valorAtual += linha.valorAtual;
        resumo.linhas.append(linha);
    }

    resumo.lucroPrejuizo = resumo.valorAtual - resumo.valorInvestido;
    resumo.rentabilidadePercentual = resumo.valorInvestido > 0.0
        ? (resumo.lucroPrejuizo / resumo.valorInvestido) * 100.0
        : 0.0;
    return resumo;
}

QString ServicoCarteira::ultimoErro() const
{
    return m_ultimoErro;
}

// ===========================================================================
// ServicoAlerta - Observer manual, igual ao real
// ===========================================================================

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
    // Percorre uma cópia: um observador pode se remover ao ser notificado.
    const std::vector<ObservadorAlerta*> destinatarios = m_observadores;
    for (ObservadorAlerta* observador : destinatarios)
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
    if (prototipo::ativoPorId(ativoId) == nullptr)
    {
        m_ultimoErro = QString::fromUtf8("Ativo não encontrado no cadastro.");
        return false;
    }

    Alerta alerta;
    alerta.definirId(prototipo::proximoId());
    alerta.definirUsuarioId(usuarioId);
    alerta.definirAtivoId(ativoId);
    alerta.definirCondicao(condicao);
    alerta.definirValorReferencia(valorReferencia);
    alerta.definirEstado(EstadoAlerta::Ativo);
    alerta.definirCriadoEm(prototipo::momentoDaSemente().date());
    dados().alertas.append(alerta);

    if (criado != nullptr)
    {
        *criado = alerta;
    }
    return true;
}

bool ServicoAlerta::arquivarAlerta(qint64 alertaId)
{
    m_ultimoErro.clear();
    Alerta* encontrado = prototipo::alertaPorId(alertaId);
    if (encontrado == nullptr)
    {
        m_ultimoErro = QString::fromUtf8("Alerta não encontrado.");
        return false;
    }

    Alerta alerta = *encontrado;
    if (!alerta.arquivar())
    {
        m_ultimoErro = QString::fromUtf8(
            "Somente alertas já disparados podem ser arquivados (estado atual: %1).")
            .arg(paraTexto(alerta.estado()));
        return false;
    }
    *encontrado = alerta;
    return true;
}

bool ServicoAlerta::removerAlerta(qint64 alertaId)
{
    m_ultimoErro.clear();
    // Como o DELETE do real, remover um id inexistente não é erro.
    QVector<Alerta>& alertas = dados().alertas;
    alertas.erase(std::remove_if(alertas.begin(), alertas.end(),
                                 [alertaId](const Alerta& a) { return a.id() == alertaId; }),
                  alertas.end());
    return true;
}

QVector<Alerta> ServicoAlerta::listarPorUsuario(qint64 usuarioId) const
{
    QVector<Alerta> doUsuario;
    for (const Alerta& alerta : dados().alertas)
    {
        if (alerta.usuarioId() == usuarioId)
        {
            doUsuario.append(alerta);
        }
    }
    ordenarAlertas(doUsuario);
    return doUsuario;
}

int ServicoAlerta::avaliarAlertas(qint64 usuarioId)
{
    m_ultimoErro.clear();
    int disparados = 0;

    QVector<Alerta> monitorados;
    for (const Alerta& alerta : dados().alertas)
    {
        if (alerta.usuarioId() == usuarioId && alerta.estado() == EstadoAlerta::Ativo)
        {
            monitorados.append(alerta);
        }
    }
    ordenarAlertas(monitorados);

    for (const Alerta& original : monitorados)
    {
        const QVector<Cotacao>& serie = prototipo::cotacoesDoAtivo(original.ativoId());
        if (serie.isEmpty())
        {
            // Sem cotação não há o que comparar; o alerta segue monitorando.
            continue;
        }
        const Cotacao& ultima = serie.last();

        // Preço para condições de preço; variação do dia para a condição de variação.
        const double valorObservado = original.condicao() == TipoCondicaoAlerta::VariacaoDiariaAcimaDe
            ? ultima.variacaoPercentual()
            : ultima.fechamento();

        if (!original.condicaoSatisfeita(valorObservado))
        {
            continue;
        }

        Alerta alerta = original;
        if (!alerta.disparar(prototipo::momentoDaSemente()))
        {
            continue;
        }
        Alerta* guardado = prototipo::alertaPorId(alerta.id());
        if (guardado == nullptr)
        {
            continue;
        }
        *guardado = alerta;

        ++disparados;
        const Ativo* ativo = prototipo::ativoPorId(alerta.ativoId());
        if (ativo != nullptr)
        {
            // Cópia: um observador pode alterar dados() e invalidar o ponteiro.
            const std::unique_ptr<Ativo> copia = prototipo::clonarAtivo(*ativo);
            notificar(alerta, *copia, valorObservado);
        }
    }
    return disparados;
}

QString ServicoAlerta::ultimoErro() const
{
    return m_ultimoErro;
}

// ===========================================================================
// ServicoRecomendacao: lê as recomendações fixas, sem MotorAnalise
// ===========================================================================

ServicoRecomendacao::ServicoRecomendacao(RepositorioRecomendacao& repositorioRecomendacao,
                                         RepositorioCotacao& repositorioCotacao,
                                         RepositorioAtivo& repositorioAtivo,
                                         RepositorioRegra& repositorioRegra)
    : m_repositorioRecomendacao(repositorioRecomendacao)
    , m_repositorioCotacao(repositorioCotacao)
    , m_repositorioAtivo(repositorioAtivo)
    , m_repositorioRegra(repositorioRegra)
{
}

std::optional<Recomendacao> ServicoRecomendacao::gerarParaAtivo(qint64 ativoId)
{
    m_ultimoErro.clear();
    if (prototipo::ativoPorId(ativoId) == nullptr)
    {
        m_ultimoErro = QString::fromUtf8("Ativo não encontrado no cadastro.");
        return std::nullopt;
    }

    const Recomendacao* fixa = prototipo::recomendacaoVigenteDoAtivo(ativoId);
    if (fixa == nullptr)
    {
        m_ultimoErro = QString::fromUtf8(
            "No protótipo, a análise está disponível apenas para os ativos de demonstração.");
        return std::nullopt;
    }
    return *fixa;
}

QVector<Recomendacao> ServicoRecomendacao::gerarParaTodos()
{
    m_ultimoErro.clear();
    QVector<Recomendacao> publicadas;

    // Mesma ordem do real: ativos por ticker.
    const std::vector<std::unique_ptr<Ativo>> ativos = m_repositorioAtivo.listarTodos();
    for (const std::unique_ptr<Ativo>& ativo : ativos)
    {
        const Recomendacao* fixa = prototipo::recomendacaoVigenteDoAtivo(ativo->id());
        if (fixa != nullptr)
        {
            publicadas.append(*fixa);
        }
    }
    if (publicadas.isEmpty())
    {
        m_ultimoErro = QString::fromUtf8(
            "No protótipo, a análise está disponível apenas para os ativos de demonstração.");
    }
    return publicadas;
}

QVector<Recomendacao> ServicoRecomendacao::listarVigentes() const
{
    // Mesmo ORDER BY gerada_em DESC, id DESC do repositório real.
    QVector<Recomendacao> vigentes;
    for (const Recomendacao& recomendacao : dados().recomendacoes)
    {
        if (recomendacao.estaVigente())
        {
            vigentes.append(recomendacao);
        }
    }
    std::sort(vigentes.begin(), vigentes.end(), [](const Recomendacao& a, const Recomendacao& b) {
        if (a.geradaEm() != b.geradaEm())
        {
            return a.geradaEm() > b.geradaEm();
        }
        return a.id() > b.id();
    });
    return vigentes;
}

std::optional<Recomendacao> ServicoRecomendacao::vigenteDoAtivo(qint64 ativoId) const
{
    const Recomendacao* vigente = prototipo::recomendacaoVigenteDoAtivo(ativoId);
    if (vigente == nullptr)
    {
        return std::nullopt;
    }
    return *vigente;
}

QVector<Recomendacao> ServicoRecomendacao::detalharPorRegra(qint64 ativoId) const
{
    if (prototipo::ativoPorId(ativoId) == nullptr)
    {
        return QVector<Recomendacao>();
    }
    return prototipo::pareceresDoAtivo(ativoId);
}

QString ServicoRecomendacao::ultimoErro() const
{
    return m_ultimoErro;
}

// ===========================================================================
// ServicoSincronizacao: sem rede, mensagem fixa
// ===========================================================================

QString ResultadoSincronizacao::resumo() const
{
    return prototipo::mensagemSincronizacaoFixa();
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

ResultadoSincronizacao ServicoSincronizacao::sincronizarTodos()
{
    // Nenhuma requisição e nenhuma cotação alterada; os ativos contam como
    // verificados para a tela exibir a mensagem como informação, não como erro.
    ResultadoSincronizacao resultado;
    resultado.ativosProcessados = static_cast<int>(dados().ativos.size());
    return resultado;
}

// ===========================================================================
// FonteYahooFinance: só o necessário para a vtable (sem Qt Network)
// ===========================================================================

FonteYahooFinance::FonteYahooFinance()
{
}

FonteYahooFinance::~FonteYahooFinance()
{
}

QString FonteYahooFinance::nome() const
{
    return QStringLiteral("Yahoo Finance");
}

bool FonteYahooFinance::buscar(const QString& ticker,
                               qint64 ativoId,
                               const QDate& de,
                               const QDate& ate,
                               QVector<Cotacao>* destino,
                               QString* erro)
{
    Q_UNUSED(ticker);
    Q_UNUSED(ativoId);
    Q_UNUSED(de);
    Q_UNUSED(ate);
    if (destino != nullptr)
    {
        destino->clear();
    }
    if (erro != nullptr)
    {
        *erro = prototipo::mensagemSincronizacaoFixa();
    }
    return false;
}

}
