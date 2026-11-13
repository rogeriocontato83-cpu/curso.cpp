#include <iostream>
using namespace std;

void verificarMeta(double venda, int &bateram, int &naoBateram) {
	if (venda >= 1000) {
		bateram++;
	} else {
		naoBateram = naoBateram + 1;
	}
}

int main() {
double vendas [5]; //vetor de vendas
int bateram = 0;
int naoBateram = 0;
for (int i = 0; i < 5; i++) {
	cout << "Digite o valor da venda " << i + 1 << ": ";
	cin >> vendas[i];
verificarMeta(vendas[i], bateram, naoBateram);
}
cout << "Quantidade de vendedores que bateram a meta: " << bateram << endl;
cout << "Quantidade de vendedores que nao bateram a meta: " << naoBateram << endl;
	return 0;
}