#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cctype>
#include <algorithm>

using namespace std;

// Estrutura de dados atualizada conforme solicitacoes da presidente Luzia Monteiro
struct Associado {
    int id;
    string nome;
    string categoria;           // 1-Contribuinte, 2-Benemerito, 3-Colaborador, 4-Voluntario, 5-Finalistico
    string nucleo;              // Selecionado via menu numerico (Art. 8o do Estatuto)
    string telefone;
    string miniBio;             // NOVO: 3 a 5 linhas (formacao e historico)
    int horasProjeto;           // NOVO: Horas dedicadas / expectativa de horas no projeto
    bool ativo;
    bool termoAssinado;         // Termo de Ciencia (Colaborador) ou Termo de Voluntariado (Voluntario)
    bool lgpdAceito;
    string urlDocumentosDrive;  // NOVO: PDF da mini-bio + Termo assinado via Gov.br
    string urlMidiasDrive;      // NOVO: Foto + Video de apresentacao (1 min)
};

vector<Associado> listaAssociados;
const string ARQUIVO_CSV = "associados_makambira.csv";

// Prototipos das funcoes
int lerNumero();
int lerNumeroIntervalo(int min, int max);
int lerNumeroNaoNegativo();
bool lerBooleano(const string& mensagem);
string lerTextoValido(const string& mensagem, bool permitirNumeros);
string lerMiniBio();
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

    int opcao = 0;
    do {
        cout << "\n============================================================\n";
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
                break;
            case 2:
                listarAssociados();
                break;
            case 3:
                listarPendencias();
                break;
            case 4:
                inativarAssociado();
                break;
            case 5:
                exibirResumoHoras();
                break;
            case 6:
                salvarEmArquivoCSV();
                break;
            case 7:
                salvarEmArquivoCSV();
                cout << "Encerrando o sistema...\n";
                break;
            default:
                cout << "[ERRO] Opcao invalida! Digite um numero entre 1 e 7.\n";
        }
    } while (opcao != 7);

    return 0;
}

// --- FUNCOES AUXILIARES DE VALIDACAO E ENTRADA ---

int lerNumero() {
    string entrada;
    while (true) {
        getline(cin, entrada);
        // Remove espacos nas extremidades
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

string lerTextoValido(const string& mensagem, bool permitirNumeros) {
    string texto;
    while (true) {
        cout << mensagem;
        getline(cin, texto);

        size_t inicio = texto.find_first_not_of(" \t\r\n");
        if (inicio == string::npos) {
            cout << "[ERRO] Este campo nao pode ficar em branco.\n";
            continue;
        }
        size_t fim = texto.find_last_not_of(" \t\r\n");
        texto = texto.substr(inicio, fim - inicio + 1);

        if (!permitirNumeros) {
            bool temNumero = any_of(texto.begin(), texto.end(), [](unsigned char c) {
                return isdigit(c);
            });
            if (temNumero) {
                cout << "[ERRO] Este campo nao aceita numeros. Digite apenas texto.\n";
                continue;
            }
        }
        return texto;
    }
}

string lerMiniBio() {
    cout << "Mini-biografia (3 a 5 linhas sobre formacao e historico).\n";
    cout << "(Digite o texto e pressione ENTER em uma linha vazia para finalizar):\n";

    string bioCompleta;
    string linha;
    int linhasLidas = 0;

    while (linhasLidas < 5) {
        getline(cin, linha);
        size_t inicio = linha.find_first_not_of(" \t\r\n");

        if (inicio == string::npos) {
            if (linhasLidas == 0) {
                cout << "[ERRO] A mini-biografia e obrigatoria. Digite ao menos 1 linha: ";
                continue;
            }
            break; // Encerra ao pressionar ENTER em linha vazia apos digitar conteudo
        }

        size_t fim = linha.find_last_not_of(" \t\r\n");
        linha = linha.substr(inicio, fim - inicio + 1);

        if (!bioCompleta.empty()) {
            bioCompleta += " ";
        }
        bioCompleta += linha;
        linhasLidas++;
    }

    return bioCompleta;
}

string selecionarNucleo(bool obrigatorio) {
    cout << "\nSelecione o Nucleo Tematico (Art. 8o do Estatuto):\n";
    cout << "  1. Nucleo de Acoes Territoriais e Comunitarias\n";
    cout << "  2. Nucleo de Politicas Publicas do Cuidado\n";
    cout << "  3. Nucleo de Politicas para a Longevidade\n";
    cout << "  4. Nucleo de Politicas para Pessoa com Deficiencia e Doencas Raras\n";
    cout << "  5. Nucleo de Estudos, Pesquisas e Informacao/Formacao Continuada\n";
    cout << "  6. Nucleo de Comunicacao Social, Midias Digitais e Conteudo Educativo\n";
    if (!obrigatorio) {
        cout << "  7. Nao vinculado a nucleo especifico\n";
    }
    cout << "Opcao: ";

    int maxOp = obrigatorio ? 6 : 7;
    int nucOp = lerNumeroIntervalo(1, maxOp);

    switch (nucOp) {
        case 1: return "Acoes Territoriais e Comunitarias";
        case 2: return "Politicas Publicas do Cuidado";
        case 3: return "Politicas para a Longevidade";
        case 4: return "Pessoa com Deficiencia e Doencas Raras";
        case 5: return "Estudos, Pesquisas e Formacao Continuada";
        case 6: return "Comunicacao Social e Midias Digitais";
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
    Associado a;
    int maiorId = 0;
    for (const auto& item : listaAssociados) {
        if (item.id > maiorId) maiorId = item.id;
    }
    a.id = maiorId + 1;

    cout << "\n============================================================\n";
    cout << "             CADASTRO DE NOVO ASSOCIADO / MEMBRO            \n";
    cout << "============================================================\n";

    a.nome = lerTextoValido("Nome Completo: ", false);

    cout << "\nSelecione a Categoria (Estatuto):\n";
    cout << "  1. Socio Efetivo / Contribuinte\n";
    cout << "  2. Socio Benemerito (Doacao de Saberes e Competencias)\n";
    cout << "  3. Socio Colaborador (Pesquisadores / Pareceristas / Projetos)\n";
    cout << "  4. Socio Voluntario (Atuacao em Campo - Lei n. 9.608/1998)\n";
    cout << "  5. Socio Finalistico / Beneficiario\n";
    cout << "Opcao: ";
    int catOp = lerNumeroIntervalo(1, 5);

    switch (catOp) {
        case 1: a.categoria = "Socio Efetivo / Contribuinte"; break;
        case 2: a.categoria = "Socio Benemerito"; break;
        case 3: a.categoria = "Socio Colaborador"; break;
        case 4: a.categoria = "Socio Voluntario"; break;
        case 5: a.categoria = "Socio Finalistico / Beneficiario"; break;
    }

    // Fluxo especifico conforme o perfil da categoria (foco no Colaborador e Voluntario)
    if (catOp == 3) {
        a.nucleo = selecionarNucleo(true);
        a.telefone = lerTextoValido("\nTelefone de Contato: ", true);
        a.miniBio = lerMiniBio();
        cout << "Expectativa de Horas Dedicadas ao Projeto: ";
        a.horasProjeto = lerNumeroNaoNegativo();

        cout << "\n--- CONFORMIDADE LEGAL E REPOSITORIO DRIVE ---\n";
        a.termoAssinado = lerBooleano("Assinou o Termo de Ciencia e Adesao ao Projeto?");
    } else if (catOp == 4) {
        a.nucleo = selecionarNucleo(true);
        a.telefone = lerTextoValido("\nTelefone de Contato: ", true);
        a.miniBio = lerMiniBio();
        cout << "Horas Dedicadas / Acordadas no Projeto: ";
        a.horasProjeto = lerNumeroNaoNegativo();

        cout << "\n--- CONFORMIDADE LEGAL E REPOSITORIO DRIVE ---\n";
        a.termoAssinado = lerBooleano("Assinou o Termo de Voluntariado (Lei n. 9.608/1998)?");
    } else {
        a.nucleo = selecionarNucleo(false);
        a.telefone = lerTextoValido("\nTelefone de Contato: ", true);
        a.miniBio = "N/A";
        a.horasProjeto = 0;
        a.termoAssinado = true; // Termo especifico de projeto/voluntariado nao exigido para esta categoria

        cout << "\n--- CONFORMIDADE LEGAL E REPOSITORIO DRIVE ---\n";
    }

    a.lgpdAceito = lerBooleano("Autorizou o uso de dados / LGPD?");

    cout << "Link do Google Drive - DOCUMENTOS (PDF mini-bio + Termo Gov.br) [ENTER para N/A]: ";
    getline(cin, a.urlDocumentosDrive);
    if (a.urlDocumentosDrive.empty()) a.urlDocumentosDrive = "N/A";

    cout << "Link do Google Drive - MIDIAS (Foto + Video apresentacao 1 min) [ENTER para N/A]: ";
    getline(cin, a.urlMidiasDrive);
    if (a.urlMidiasDrive.empty()) a.urlMidiasDrive = "N/A";

    a.ativo = lerBooleano("Status do Associado ativo no sistema?");

    listaAssociados.push_back(a);
    cout << "\n[OK] Associado cadastrado com sucesso (ID: " << a.id << ")!\n";
}

void listarAssociados() {
    cout << "\n============================================================\n";
    cout << "               LISTA DE ASSOCIADOS CADASTRADOS              \n";
    cout << "============================================================\n";

    if (listaAssociados.empty()) {
        cout << "Nenhum associado cadastrado ate o momento.\n";
        return;
    }

    for (const auto& a : listaAssociados) {
        string statusDoc = (a.termoAssinado && a.lgpdAceito) ? "Concluido / Finalizado" : "Pendente";

        cout << "ID: " << a.id << " | Nome: " << a.nome << "\n"
             << "  Categoria: " << a.categoria << " | Nucleo: " << a.nucleo << "\n"
             << "  Telefone: " << a.telefone << " | Horas no Projeto: " << a.horasProjeto << "h\n"
             << "  Mini-Bio: " << a.miniBio << "\n"
             << "  Status Cadastro: " << (a.ativo ? "Ativo" : "Inativo")
             << " | Status da Documentacao: " << statusDoc << "\n"
             << "  Detalhes Legais -> Termo: " << (a.termoAssinado ? "OK" : "PENDENTE")
             << " | LGPD: " << (a.lgpdAceito ? "OK" : "PENDENTE") << "\n"
             << "  Drive Documentos (PDF/Termo): " << a.urlDocumentosDrive << "\n"
             << "  Drive Midias (Foto/Video):    " << a.urlMidiasDrive << "\n";
        cout << "------------------------------------------------------------\n";
    }
}

void listarPendencias() {
    cout << "\n============================================================\n";
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
            string tipoTermo = (a.categoria == "Socio Colaborador")
                                   ? "[TERMO DE CIENCIA PENDENTE] "
                                   : "[TERMO DE VOLUNTARIADO PENDENTE] ";

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
    cout << "\n--- ALTERACAO DE STATUS DE ASSOCIADO ---\n";
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
    cout << "\n============================================================\n";
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

    arquivo << "ID;Nome;Categoria;Nucleo;Telefone;MiniBio;HorasProjeto;Ativo;TermoAssinado;LGPDAceito;URLDocumentos;URLMidias\n";

    for (const auto& a : listaAssociados) {
        arquivo << a.id << ";"
                << sanitizarParaCSV(a.nome) << ";"
                << sanitizarParaCSV(a.categoria) << ";"
                << sanitizarParaCSV(a.nucleo) << ";"
                << sanitizarParaCSV(a.telefone) << ";"
                << sanitizarParaCSV(a.miniBio) << ";"
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
        return; // Primeira execucao: arquivo ainda nao existe
    }

    string linha;
    getline(arquivo, linha); // Ignora cabecalho

    while (getline(arquivo, linha)) {
        if (linha.empty()) continue;

        stringstream ss(linha);
        string campo;
        vector<string> colunas;

        while (getline(ss, campo, ';')) {
            colunas.push_back(campo);
        }

        if (colunas.size() == 12) {
            try {
                Associado a;
                a.id = stoi(colunas[0]);
                a.nome = colunas[1];
                a.categoria = colunas[2];
                a.nucleo = colunas[3];
                a.telefone = colunas[4];
                a.miniBio = colunas[5];
                a.horasProjeto = stoi(colunas[6]);
                a.ativo = (colunas[7] == "1");
                a.termoAssinado = (colunas[8] == "1");
                a.lgpdAceito = (colunas[9] == "1");
                a.urlDocumentosDrive = colunas[10];
                a.urlMidiasDrive = colunas[11];
                listaAssociados.push_back(a);
            } catch (...) {
                // Ignora linhas malformadas
            }
        }
    }

    arquivo.close();
    if (!listaAssociados.empty()) {
        cout << "[INFO] " << listaAssociados.size()
             << " registro(s) carregado(s) de '" << ARQUIVO_CSV << "'.\n";
    }
}
