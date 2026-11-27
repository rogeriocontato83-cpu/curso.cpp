#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cctype>
#include <algorithm>
#include <cstdlib>

using namespace std;

// Estrutura de dados alinhada ao Estatuto Social (Art. 5o e Art. 8o) e as solicitacoes da Diretoria
struct Associado {
    int id;
    string nome;
    string categoria;           // Art. 5o: I-Contribuinte, II-Benemerito, III-Colaborador, IV-Voluntario, V-Beneficiario/Usuario
    string nucleo;              // Art. 8o: Nucleos Tematicos Interdisciplinares
    string telefone;
    int horasProjeto;           // Horas dedicadas / expectativa de horas no projeto
    bool ativo;
    bool termoAssinado;         // Termo de Ciencia/Adesao (Colaborador) ou Termo de Adesao Lei n. 9.608/1998 (Voluntario)
    bool lgpdAceito;
    string urlDocumentosDrive;  // Link Nuvem: PDF da Mini-bio + Termo assinado via Gov.br
    string urlMidiasDrive;      // Link Nuvem: Foto + Video de apresentacao (1 min)
};

vector<Associado> listaAssociados;
const string ARQUIVO_CSV = "associados_makambira.csv";

// Prototipos das funcoes
void limparTela();
void pausarTela();
int lerNumero();
int lerNumeroIntervalo(int min, int max);
int lerNumeroNaoNegativo();
bool lerBooleano(const string& mensagem);
string lerNomeValido(const string& mensagem);
string lerTelefoneValido(const string& mensagem);
string lerLinkNuvemValido(const string& mensagem);
string selecionarNucleo(bool obrigatorio);
string sanitizarParaCSV(string texto);
void carregarDeArquivoCSV();
void salvarEmArquivoCSV();
void cadastrarAssociado();
void listarAssociados();
void listarPendencias();
void inativarAssociado();
void exibirResumoHoras();

int main() {
    carregarDeArquivoCSV();

    if (!listaAssociados.empty()) {
        pausarTela();
    }

    int opcao = 0;
    do {
        limparTela();
        cout << "============================================================\n";
        cout << "   SISTEMA DE GESTAO DE ASSOCIADOS - INSTITUTO MAKAMBIRA    \n";
        cout << "============================================================\n";
        cout << "1. Cadastrar Novo Associado\n";
        cout << "2. Listar TODOS os Associados\n";
        cout << "3. Listar Apenas Associados com PENDENCIAS (Termo/LGPD)\n";
        cout << "4. Alternar Status (Ativo/Inativo) do Associado\n";
        cout << "5. Exibir Resumo de Horas no Projeto e Relatorio Geral\n";
        cout << "6. Salvar Dados em CSV (Continuar no Sistema)\n";
        cout << "7. Salvar Dados em CSV e Sair\n";
        cout << "============================================================\n";
        cout << "Escolha uma opcao: ";
        opcao = lerNumero();

        switch (opcao) {
            case 1:
                cadastrarAssociado();
                pausarTela();
                break;
            case 2:
                listarAssociados();
                pausarTela();
                break;
            case 3:
                listarPendencias();
                pausarTela();
                break;
            case 4:
                inativarAssociado();
                pausarTela();
                break;
            case 5:
                exibirResumoHoras();
                pausarTela();
                break;
            case 6:
                limparTela();
                salvarEmArquivoCSV();
                pausarTela();
                break;
            case 7:
                limparTela();
                salvarEmArquivoCSV();
                cout << "Encerrando o sistema...\n";
                break;
            default:
                cout << "[ERRO] Opcao invalida! Digite um numero entre 1 e 7.\n";
                pausarTela();
        }
    } while (opcao != 7);

    return 0;
}

// --- FUNCOES AUXILIARES DE VALIDACAO E INTERFACE ---

void limparTela() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pausarTela() {
    cout << "\nPressione ENTER para voltar ao menu principal...";
    string lixo;
    getline(cin, lixo);
}

int lerNumero() {
    string entrada;
    while (true) {
        getline(cin, entrada);
        size_t inicio = entrada.find_first_not_of(" \t\r\n");
        size_t fim = entrada.find_last_not_of(" \t\r\n");
        if (inicio == string::npos) {
            cout << "[ERRO] Entrada vazia. Digite um numero valido: ";
            continue;
        }
        entrada = entrada.substr(inicio, fim - inicio + 1);

        try {
            size_t pos = 0;
            int valor = stoi(entrada, &pos);
            if (pos != entrada.length()) {
                cout << "[ERRO] Digite apenas numeros validos: ";
                continue;
            }
            return valor;
        } catch (...) {
            cout << "[ERRO] Digite um numero valido: ";
        }
    }
}

int lerNumeroIntervalo(int min, int max) {
    while (true) {
        int valor = lerNumero();
        if (valor >= min && valor <= max) {
            return valor;
        }
        cout << "[ERRO] Digite um numero entre " << min << " e " << max << ": ";
    }
}

int lerNumeroNaoNegativo() {
    while (true) {
        int valor = lerNumero();
        if (valor >= 0) {
            return valor;
        }
        cout << "[ERRO] O valor nao pode ser negativo. Digite novamente: ";
    }
}

bool lerBooleano(const string& mensagem) {
    while (true) {
        cout << mensagem << " (1-Sim / 0-Nao): ";
        int op = lerNumero();
        if (op == 1) return true;
        if (op == 0) return false;
        cout << "[ERRO] Opcao invalida. Digite 1 para Sim ou 0 para Nao.\n";
    }
}

string lerNomeValido(const string& mensagem) {
    string texto;
    while (true) {
        cout << mensagem;
        getline(cin, texto);

        size_t inicio = texto.find_first_not_of(" \t\r\n");
        if (inicio == string::npos) {
            cout << "[ERRO] O nome nao pode ficar em branco.\n";
            continue;
        }
        size_t fim = texto.find_last_not_of(" \t\r\n");
        texto = texto.substr(inicio, fim - inicio + 1);

        bool temNumero = any_of(texto.begin(), texto.end(), [](unsigned char c) {
            return isdigit(c);
        });

        if (temNumero) {
            cout << "[ERRO] Nome de pessoa nao pode conter numeros. Digite novamente.\n";
            continue;
        }
        return texto;
    }
}

string lerTelefoneValido(const string& mensagem) {
    string telefone;
    while (true) {
        cout << mensagem;
        getline(cin, telefone);

        size_t inicio = telefone.find_first_not_of(" \t\r\n");
        if (inicio == string::npos) {
            cout << "[ERRO] O telefone nao pode ficar em branco.\n";
            continue;
        }
        size_t fim = telefone.find_last_not_of(" \t\r\n");
        telefone = telefone.substr(inicio, fim - inicio + 1);

        bool temLetra = any_of(telefone.begin(), telefone.end(), [](unsigned char c) {
            return isalpha(c);
        });
        int qtdDigitos = count_if(telefone.begin(), telefone.end(), [](unsigned char c) {
            return isdigit(c);
        });

        bool caractereInvalido = any_of(telefone.begin(), telefone.end(), [](unsigned char c) {
            return !isdigit(c) && c != ' ' && c != '(' && c != ')' && c != '-' && c != '+';
        });

        if (temLetra || caractereInvalido || qtdDigitos < 8) {
            cout << "[ERRO] Telefone invalido! Nao digite letras. Use apenas numeros (ex: 11 99999-9999).\n";
            continue;
        }
        return telefone;
    }
}

string lerLinkNuvemValido(const string& mensagem) {
    const vector<string> dominiosPermitidos = {
        "drive.google.com",
        "docs.google.com",
        "onedrive.live.com",
        "1drv.ms",
        "sharepoint.com",
        "dropbox.com",
        "icloud.com",
        "box.com"
    };

    string link;
    while (true) {
        cout << mensagem;
        getline(cin, link);

        size_t inicio = link.find_first_not_of(" \t\r\n");
        if (inicio == string::npos) {
            return "N/A";
        }
        size_t fim = link.find_last_not_of(" \t\r\n");
        link = link.substr(inicio, fim - inicio + 1);

        string linkMinusculo = link;
        transform(linkMinusculo.begin(), linkMinusculo.end(), linkMinusculo.begin(), [](unsigned char c) {
            return tolower(c);
        });

        if (linkMinusculo.find(' ') != string::npos) {
            cout << "[ERRO] O link nao pode conter espacos.\n";
            continue;
        }

        size_t posProtocolo = string::npos;
        if (linkMinusculo.rfind("https://", 0) == 0) {
            posProtocolo = 8;
        } else if (linkMinusculo.rfind("http://", 0) == 0) {
            posProtocolo = 7;
        } else {
            cout << "[ERRO] O link deve comecar com 'https://' (ex: https://drive.google.com/...).\n";
            continue;
        }

        size_t fimHost = linkMinusculo.find_first_of("/?", posProtocolo);
        string host = (fimHost == string::npos)
                          ? linkMinusculo.substr(posProtocolo)
                          : linkMinusculo.substr(posProtocolo, fimHost - posProtocolo);

        bool nuvemValida = false;
        for (const string& dominio : dominiosPermitidos) {
            if (host == dominio ||
                (host.size() > dominio.size() &&
                 host.compare(host.size() - dominio.size() - 1, dominio.size() + 1, "." + dominio) == 0)) {
                nuvemValida = true;
                break;
            }
        }

        bool temCaminho = (fimHost != string::npos && link.size() > fimHost + 1);

        if (!nuvemValida || !temCaminho) {
            cout << "[ERRO] Link rejeitado! Insira um link valido de armazenamento em nuvem\n"
                 << "       (Google Drive, OneDrive, SharePoint, Dropbox, iCloud ou Box) ou pressione ENTER para N/A.\n";
            continue;
        }

        return link;
    }
}

string selecionarNucleo(bool obrigatorio) {
    cout << "\nSelecione o Nucleo Tematico Interdisciplinar (Art. 8o do Estatuto):\n";
    cout << "  1. Acoes Territoriais e Comunitarias\n";
    cout << "  2. Politicas Publicas do Cuidado\n";
    cout << "  3. Politicas para a Longevidade\n";
    cout << "  4. Politicas para Pessoa com Deficiencia e Doencas Raras\n";
    cout << "  5. Estudos, Pesquisas e Informacao/Formacao Continuada\n";
    cout << "  6. Comunicacao Social, Midias Digitais e Producao de Conteudo Educativo\n";
    if (!obrigatorio) {
        cout << "  7. Nao vinculado a Nucleo Tematico especifico\n";
    }
    cout << "Opcao: ";

    int maxOp = obrigatorio ? 6 : 7;
    int nucOp = lerNumeroIntervalo(1, maxOp);

    switch (nucOp) {
        case 1: return "Acoes Territoriais e Comunitarias";
        case 2: return "Politicas Publicas do Cuidado";
        case 3: return "Politicas para a Longevidade";
        case 4: return "Pessoa com Deficiencia e Doencas Raras";
        case 5: return "Estudos, Pesquisas e Informacao/Formacao Continuada";
        case 6: return "Comunicacao Social, Midias Digitais e Producao de Conteudo Educativo";
        default: return "Nao vinculado";
    }
}

string sanitizarParaCSV(string texto) {
    for (char& c : texto) {
        if (c == ';' || c == '\n' || c == '\r') {
            c = ' ';
        }
    }
    return texto;
}

// --- FUNCOES PRINCIPAIS DO SISTEMA ---

void cadastrarAssociado() {
    limparTela();

    Associado a;
    int maiorId = 0;
    for (const auto& item : listaAssociados) {
        if (item.id > maiorId) maiorId = item.id;
    }
    a.id = maiorId + 1;

    cout << "============================================================\n";
    cout << "             CADASTRO DE NOVO ASSOCIADO (ART. 5o)           \n";
    cout << "============================================================\n";

    a.nome = lerNomeValido("Nome Completo: ");

    cout << "\nSelecione a Categoria Associativa (Art. 5o do Estatuto Social):\n";
    cout << "  1. Associado Contribuinte (Gestao, solidez institucional e direito a voto)\n";
    cout << "  2. Associado Benemerito (Contribuicao relevante a causa ou ao patrimonio)\n";
    cout << "  3. Associado Colaborador (Natureza tecnica, cientifica e consultiva)\n";
    cout << "  4. Associado Voluntario (Atuacao gratuita e comunitaria - Lei n. 9.608/1998)\n";
    cout << "  5. Associado Beneficiario/Usuario (Controle social e Conselho Consultivo)\n";
    cout << "Opcao: ";
    int catOp = lerNumeroIntervalo(1, 5);

    switch (catOp) {
        case 1: a.categoria = "Associado Contribuinte"; break;
        case 2: a.categoria = "Associado Benemerito"; break;
        case 3: a.categoria = "Associado Colaborador"; break;
        case 4: a.categoria = "Associado Voluntario"; break;
        case 5: a.categoria = "Associado Beneficiario/Usuario"; break;
    }

    if (catOp == 3) {
        a.nucleo = selecionarNucleo(true);
        a.telefone = lerTelefoneValido("\nTelefone de Contato (apenas numeros/DDD): ");
        cout << "Expectativa de Horas Dedicadas ao Projeto: ";
        a.horasProjeto = lerNumeroNaoNegativo();

        cout << "\n--- CONFORMIDADE LEGAL E REPOSITORIO EM NUVEM ---\n";
        a.termoAssinado = lerBooleano("Assinou o Termo de Ciencia e Adesao ao Projeto?");
    } else if (catOp == 4) {
        a.nucleo = selecionarNucleo(true);
        a.telefone = lerTelefoneValido("\nTelefone de Contato (apenas numeros/DDD): ");
        cout << "Horas Dedicadas / Acordadas no Projeto: ";
        a.horasProjeto = lerNumeroNaoNegativo();

        cout << "\n--- CONFORMIDADE LEGAL E REPOSITORIO EM NUVEM ---\n";
        a.termoAssinado = lerBooleano("Assinou o Termo de Adesao ao Servico Voluntario (Lei n. 9.608/1998)?");
    } else {
        a.nucleo = selecionarNucleo(false);
        a.telefone = lerTelefoneValido("\nTelefone de Contato (apenas numeros/DDD): ");
        a.horasProjeto = 0;
        a.termoAssinado = true;

        cout << "\n--- CONFORMIDADE LEGAL E REPOSITORIO EM NUVEM ---\n";
    }

    a.lgpdAceito = lerBooleano("Autorizou o uso de dados / LGPD?");

    a.urlDocumentosDrive = lerLinkNuvemValido(
        "Link Nuvem - DOCUMENTOS (PDF Mini-bio + Termo Gov.br) [ENTER para N/A]: "
    );

    a.urlMidiasDrive = lerLinkNuvemValido(
        "Link Nuvem - MIDIAS (Foto + Video de apresentacao 1 min) [ENTER para N/A]: "
    );

    a.ativo = lerBooleano("Status do Associado ativo no sistema?");

    listaAssociados.push_back(a);

    limparTela();
    cout << "[OK] " << a.categoria << " '" << a.nome << "' cadastrado com sucesso (ID: " << a.id << ")!\n";
}

void listarAssociados() {
    limparTela();
    cout << "============================================================\n";
    cout << "               LISTA DE ASSOCIADOS CADASTRADOS              \n";
    cout << "============================================================\n";

    if (listaAssociados.empty()) {
        cout << "Nenhum associado cadastrado ate o momento.\n";
        return;
    }

    for (const auto& a : listaAssociados) {
        string statusDoc = (a.termoAssinado && a.lgpdAceito) ? "Concluido / Finalizado" : "Pendente";

        cout << "ID: " << a.id << " | Nome: " << a.nome << "\n"
             << "  Categoria (Art. 5o): " << a.categoria << "\n"
             << "  Nucleo Tematico (Art. 8o): " << a.nucleo << "\n"
             << "  Telefone: " << a.telefone << " | Horas no Projeto: " << a.horasProjeto << "h\n"
             << "  Status Cadastro: " << (a.ativo ? "Ativo" : "Inativo")
             << " | Status da Documentacao: " << statusDoc << "\n"
             << "  Detalhes Legais -> Termo: " << (a.termoAssinado ? "OK" : "PENDENTE")
             << " | LGPD: " << (a.lgpdAceito ? "OK" : "PENDENTE") << "\n"
             << "  Nuvem Documentos (PDF Mini-bio/Termo): " << a.urlDocumentosDrive << "\n"
             << "  Nuvem Midias (Foto/Video):             " << a.urlMidiasDrive << "\n";
        cout << "------------------------------------------------------------\n";
    }
}

void listarPendencias() {
    limparTela();
    cout << "============================================================\n";
    cout << "            RELATORIO DE ASSOCIADOS COM PENDENCIAS          \n";
    cout << "============================================================\n";

    if (listaAssociados.empty()) {
        cout << "Nenhum associado cadastrado no sistema.\n";
        return;
    }

    int contagemPendentes = 0;
    for (const auto& a : listaAssociados) {
        if (!a.termoAssinado || !a.lgpdAceito) {
            contagemPendentes++;
            string tipoTermo = (a.categoria == "Associado Colaborador")
                                   ? "[TERMO DE CIENCIA E ADESAO PENDENTE] "
                                   : "[TERMO DE VOLUNTARIADO (LEI 9.608/98) PENDENTE] ";

            cout << "ID: " << a.id << " | Nome: " << a.nome << "\n"
                 << "  Categoria: " << a.categoria << " | Nucleo: " << a.nucleo << "\n"
                 << "  Telefone: " << a.telefone << " | Status da Documentacao: Pendente\n"
                 << "  Pendencias -> "
                 << (!a.termoAssinado ? tipoTermo : "")
                 << (!a.lgpdAceito ? "[LGPD PENDENTE]" : "") << "\n";
            cout << "------------------------------------------------------------\n";
        }
    }

    if (contagemPendentes == 0) {
        cout << "[OK] Parabens! Todos os associados estao com a documentacao concluida.\n";
    } else {
        cout << "Total de associados com documentacao pendente: " << contagemPendentes << "\n";
    }
}

void inativarAssociado() {
    limparTela();
    cout << "============================================================\n";
    cout << "             ALTERACAO DE STATUS DE ASSOCIADO               \n";
    cout << "============================================================\n";
    cout << "Digite o ID do associado: ";
    int idBusca = lerNumero();

    bool encontrado = false;
    for (auto& a : listaAssociados) {
        if (a.id == idBusca) {
            encontrado = true;
            a.ativo = !a.ativo;
            cout << "[OK] O status de '" << a.nome
                 << "' foi alterado para: " << (a.ativo ? "ATIVO" : "INATIVO") << "\n";
            break;
        }
    }

    if (!encontrado) {
        cout << "[ERRO] Associado com ID " << idBusca << " nao encontrado.\n";
    }
}

void exibirResumoHoras() {
    limparTela();
    cout << "============================================================\n";
    cout << "          RESUMO OPERACIONAL E RELATORIO DE PROJETOS        \n";
    cout << "============================================================\n";

    if (listaAssociados.empty()) {
        cout << "Nenhum dado cadastrado para gerar relatorio.\n";
        return;
    }

    int totalHorasProjeto = 0;
    int totalAtivos = 0;
    int docConcluida = 0;
    int docPendente = 0;
    int pendentesTermo = 0;
    int pendentesLGPD = 0;

    for (const auto& a : listaAssociados) {
        totalHorasProjeto += a.horasProjeto;
        if (a.ativo) totalAtivos++;
        if (a.termoAssinado && a.lgpdAceito) {
            docConcluida++;
        } else {
            docPendente++;
        }
        if (!a.termoAssinado) pendentesTermo++;
        if (!a.lgpdAceito) pendentesLGPD++;
    }

    cout << "Total de Associados Cadastrados:        " << listaAssociados.size() << "\n";
    cout << "Associados Ativos:                      " << totalAtivos << "\n";
    cout << "Total de Horas Dedicadas aos Projetos:  " << totalHorasProjeto << " horas\n";
    cout << "Documentacao Concluida / Finalizada:    " << docConcluida << "\n";
    cout << "Documentacao Pendente:                  " << docPendente << "\n";
    cout << "  - Pendencias de Termo (Ciencia/Vol.): " << pendentesTermo << "\n";
    cout << "  - Pendencias de Conformidade LGPD:    " << pendentesLGPD << "\n";
    cout << "============================================================\n";
}

void salvarEmArquivoCSV() {
    ofstream arquivo(ARQUIVO_CSV);
    if (!arquivo.is_open()) {
        cout << "[ERRO] Nao foi possivel criar o arquivo '" << ARQUIVO_CSV << "'.\n";
        return;
    }

    arquivo << "ID;Nome;Categoria;NucleoTematico;Telefone;HorasProjeto;Ativo;TermoAssinado;LGPDAceito;URLDocumentos;URLMidias\n";

    for (const auto& a : listaAssociados) {
        arquivo << a.id << ";"
                << sanitizarParaCSV(a.nome) << ";"
                << sanitizarParaCSV(a.categoria) << ";"
                << sanitizarParaCSV(a.nucleo) << ";"
                << sanitizarParaCSV(a.telefone) << ";"
                << a.horasProjeto << ";"
                << (a.ativo ? "1" : "0") << ";"
                << (a.termoAssinado ? "1" : "0") << ";"
                << (a.lgpdAceito ? "1" : "0") << ";"
                << sanitizarParaCSV(a.urlDocumentosDrive) << ";"
                << sanitizarParaCSV(a.urlMidiasDrive) << "\n";
    }

    arquivo.close();
    cout << "[OK] Dados salvos com sucesso no arquivo '" << ARQUIVO_CSV << "'!\n";
}

void carregarDeArquivoCSV() {
    ifstream arquivo(ARQUIVO_CSV);
    if (!arquivo.is_open()) {
        return;
    }

    string linha;
    getline(arquivo, linha);

    while (getline(arquivo, linha)) {
        if (linha.empty()) continue;

        stringstream ss(linha);
        string campo;
        vector<string> colunas;

        while (getline(ss, campo, ';')) {
            colunas.push_back(campo);
        }

        if (colunas.size() == 11) {
            try {
                Associado a;
                a.id = stoi(colunas[0]);
                a.nome = colunas[1];
                a.categoria = colunas[2];
                a.nucleo = colunas[3];
                a.telefone = colunas[4];
                a.horasProjeto = stoi(colunas[5]);
                a.ativo = (colunas[6] == "1");
                a.termoAssinado = (colunas[7] == "1");
                a.lgpdAceito = (colunas[8] == "1");
                a.urlDocumentosDrive = colunas[9];
                a.urlMidiasDrive = colunas[10];
                listaAssociados.push_back(a);
            } catch (...) {
            }
        }
    }

    arquivo.close();
    if (!listaAssociados.empty()) {
        cout << "[INFO] " << listaAssociados.size()
             << " registro(s) carregado(s) de '" << ARQUIVO_CSV << "'.\n";
    }
}
