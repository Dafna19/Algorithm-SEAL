#include<iostream>
#include "SEAL.h"
#include<cmath>
using namespace std;

void main(int argc, char* argv[]) {
	SEAL seal;
	char a[1024];
	cout << "write your text:" << endl;
	int i = 0, L, n;
	char c;
	while ((c = getchar()) != '\n')
		a[i++] = c;
	L = i;//L int'ов, т.е. 32*L бит
	int *text = new int[i];//его кодируем
	for (int j = 0; j < L; j++)
		text[j] = a[j];
	/*cout << "your text:" << endl;
	for (int j = 0; j < L; j++)
		cout << (char)text[j];
	cout << endl;*/

	/*char key[20];
	for (int i = 0; i < 20; i++)
		key[i] = 0;*/
	int key[5];
	cout << "write your key (5 numbers):" << endl;
	for (int i = 0; i < 5; i++)
		cin >> key[i];
	/*int k[5];
	for (int i = 0; i < 5; i++) {
		int tmp = key[4 * i + 0];
		tmp = tmp << 8 + key[4 * i + 1];
		tmp = tmp << 8 + key[4 * i + 2];
		tmp = tmp << 8 + key[4 * i + 3];
		k[i] = tmp;
	}*/

	cout << "write n:" << endl;
	cin >> n;

	int *enc = seal.coding(text, 32 * L, key, n);

	cout << "coded:" << endl;
	for (int j = 0; j < L; j++)
		cout << (char)enc[j];
	cout << endl;


	int *dec = seal.coding(enc, 32 * L, key, n);
	cout << "decoded:" << endl;
	for (int j = 0; j < L; j++)
		cout << (char)dec[j];
	cout << endl;

	system("PAUSE");
}