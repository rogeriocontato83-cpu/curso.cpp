#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <limits>

using namespace std;

// Estrutura de dados do Associado
struct Associado {
    int id;
    string nome;
    string categoria;
    string nucleo;
    string telefone;
    int horasDoadas;
    bool ativo;
    bool termoAssinado;
    bool lgpdAceito;
    string urlDrive;
};

vector<Associado> listaAssociados;

// Prototipos das funcoes (avisam o compilador que elas existem abaixo do main)
int lerNumero();
bool lerBooleano(const string& mensagem);
void cadastrarAssociado();
void listarAssociados();
void listarPendencias();
void inativarAssociado();
void exibirResumoHoras();
void salvarEmArquivoCSV();

int main() {
    int opcao = 0;
    
    do {
        cout << "\n============================================================\n";
        cout << "     SISTEMA DE GESTAO DE ASSOCIADOS - INSTITUTO MAKAMBIRA  \n";
        cout << "============================================================\n";
        cout << "1. Cadastrar Novo Associado\n";
        cout << "2. Listar TODOS os Associados\n";
        cout << "3. Listar Apenas Associados com PENDENCIAS (Termo/LGPD)\n";
        cout << "4. Alternar Status / Inativar Associado\n";
        cout << "5. Exibir Resumo de Horas e Relatorio Geral\n";
        cout << "6. Salvar Dados em CSV e Sair\n";
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
                cout << "Encerrando o sistema...\n";
                break;
            default:
                cout << "Opcao invalida! Digite um numero entre 1 e 6.\n";
        }
    } while (opcao != 6);
    
    return 0;
}

// --- IMPLEMENTACAO DAS FUNCOES ---

int lerNumero() {
    string entrada;
    int valor = 0;
    while (true) {
        getline(cin, entrada);
        try {
            valor = stoi(entrada);
            return valor;
        } catch (...) {
            cout << "[ERRO] Digite um numero valido: ";
        }
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

void cadastrarAssociado() {
    Associado a;
    a.id = listaAssociados.size() + 1;
    
    cout << "\n============================================================\n";
    cout << "             CADASTRO DE NOVO ASSOCIADO / MEMBRO            \n";
    cout << "============================================================\n";
    
    cout << "Nome Completo: ";
    getline(cin, a.nome);
    
    cout << "\nSelecione a Categoria (Estatuto):\n";
    cout << " 1. Socio Efetivo / Contribuinte\n";
    cout << " 2. Socio Benemerito (Doacao de Saberes e Competencias)\n";
    cout << " 3. Socio Colaborador\n";
    cout << " 4. Socio Voluntario\n";
    cout << " 5. Socio Finalistico / Beneficiario\n";
    cout << "Opcao: ";
    int catOp = lerNumero();
    
    switch (catOp) {
        case 1: a.categoria = "Socio Efetivo / Contribuinte"; break;
        case 2: a.categoria = "Socio Benemerito"; break;
        case 3: a.categoria = "Socio Colaborador"; break;
        case 4: a.categoria = "Socio Voluntario"; break;
        case 5: a.categoria = "Socio Finalistico / Beneficiario"; break;
        default: a.categoria = "Outros / Nao Classificado"; break;
    }
    
    cout << "\nNucleo de Atuacao (Saude, Envelhecimento, Assistencia Social, Educacao, Cultura, Meio Ambiente, Tecnologia): ";
    getline(cin, a.nucleo);
    
    cout << "Telefone de Contato: ";
    getline(cin, a.telefone);
    
    cout << "Horas Doadas/Dedicadas no Mes: ";
    a.horasDoadas = lerNumero();
    
    cout << "\n--- CONFORMIDADE LEGAL E MIDIA ---\n";
    a.termoAssinado = lerBooleano("Assinou o Termo de Voluntariado / Adesao?");
    a.lgpdAceito = lerBooleano("Autorizou o uso de dados / LGPD?");
    
    cout << "Link do Google Drive (Fotos/Documentos/Midias): ";
    getline(cin, a.urlDrive);
    if (a.urlDrive.empty()) a.urlDrive = "N/A";
    
    a.ativo = lerBooleano("Status do Associado no sistema");
    
    listaAssociados.push_back(a);
    cout << "\n[OK] Associado cadastrado com sucesso!\n";
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
        cout << "ID: " << a.id << " | Nome: " << a.nome << "\n"
             << "  Categoria: " << a.categoria << " | Nucleo: " << a.nucleo << "\n"
             << "  Telefone: " << a.telefone << " | Horas Doadas: " << a.horasDoadas << "h\n"
             << "  Status: " << (a.ativo ? "Ativo" : "Inativo")
             << " | Termo: " << (a.termoAssinado ? "OK" : "PENDENTE")
             << " | LGPD: " << (a.lgpdAceito ? "OK" : "PENDENTE") << "\n"
             << "  Drive: " << a.urlDrive << "\n";
        cout << "------------------------------------------------------------\n";
    }
}

void listarPendencias() {
    cout << "\n============================================================\n";
    cout << "          RELATORIO DE ASSOCIADOS COM PENDENCIAS            \n";
    cout << "============================================================\n";
    
    if (listaAssociados.empty()) {
        cout << "Nenhum associado cadastrado no sistema.\n";
        return;
    }
    
    int contagemPendentes = 0;
    
    for (const auto& a : listaAssociados) {
        if (!a.termoAssinado || !a.lgpdAceito) {
            contagemPendentes++;
            cout << "ID: " << a.id << " | Nome: " << a.nome << "\n"
                 << "  Categoria: " << a.categoria << " | Telefone: " << a.telefone << "\n"
                 << "  Pendencias -> " 
                 << (!a.termoAssinado ? "[TERMO DE ADESAO PENDENTE] " : "")
                 << (!a.lgpdAceito ? "[LGPD PENDENTE]" : "") << "\n";
            cout << "------------------------------------------------------------\n";
        }
    }
    
    if (contagemPendentes == 0) {
        cout << "[OK] Parabens! Todos os associados estao com a documentacao em dia.\n";
    } else {
        cout << "Total de associados com pendencias encontradas: " << contagemPendentes << "\n";
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
    cout << "            RESUMO OPERACIONAL E RELATORIO DE HORAS         \n";
    cout << "============================================================\n";
    
    if (listaAssociados.empty()) {
        cout << "Nenhum dado cadastrado para gerar relatorio.\n";
        return;
    }
    
    int totalHoras = 0;
    int totalAtivos = 0;
    int pendentesTermo = 0;
    int pendentesLGPD = 0;
    
    for (const auto& a : listaAssociados) {
        totalHoras += a.horasDoadas;
        if (a.ativo) totalAtivos++;
        if (!a.termoAssinado) pendentesTermo++;
        if (!a.lgpdAceito) pendentesLGPD++;
    }
    
    cout << "Total de Associados Cadastrados: " << listaAssociados.size() << "\n";
    cout << "Associados Ativos: " << totalAtivos << "\n";
    cout << "Total de Horas Doadas no Mes: " << totalHoras << " horas\n";
    cout << "Pendencias de Termo de Adesao: " << pendentesTermo << "\n";
    cout << "Pendencias de Conformidade LGPD: " << pendentesLGPD << "\n";
    cout << "============================================================\n";
}

void salvarEmArquivoCSV() {
    ofstream arquivo("associados_macambira.csv");
    if (!arquivo.is_open()) {
        cout << "[ERRO] Nao foi possivel criar o arquivo de dados.\n";
        return;
    }
    
    arquivo << "ID;Nome;Categoria;Nucleo;Telefone;HorasDoadas;Ativo;TermoAssinado;LGPDAceito;URLDrive\n";
    
    for (const auto& a : listaAssociados) {
        arquivo << a.id << ";"
                << a.nome << ";"
                << a.categoria << ";"
                << a.nucleo << ";"
                << a.telefone << ";"
                << a.horasDoadas << ";"
                << (a.ativo ? "1" : "0") << ";"
                << (a.termoAssinado ? "1" : "0") << ";"
                << (a.lgpdAceito ? "1" : "0") << ";"
                << a.urlDrive << "\n";
    }
    
    arquivo.close();
    cout << "[OK] Dados salvos com sucesso no arquivo 'associados_macambira.csv'!\n";
}