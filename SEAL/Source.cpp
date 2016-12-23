#include<iostream>
#include<cmath>
#include "SEAL.h"
using namespace std;
/*
1. Частотный тест			
2. Последовательный тест	
3. Тест серий				
4. Автокорреляционный тест	
5. Универсальный тест		
*/

void main(int argc, char* argv[]) {
	SEAL seal;
	char a[1024];
	cout << "write your text:" << endl;
	int i = 0, L, n;
	char c;
	while ((c = getchar()) != '\n')
		a[i++] = c;
	a[i] = '\0';
	L = i * 8;//i char'ов, т.е. 8*i бит
	int *text = new int[ceil((float)i / 4)];//его кодируем, размер в 4 раза меньше

	//вот здесь упаковать чары в инт без лишних нулей	
	for (int j = 0; j < ceil((float)i / 4); j++) {
		int tmp = 0;
		for (int k = 0; k < 4; k++) {
			int num = 4 * j + k;
			tmp = tmp << 8;
			if (num < i)//если текст ещё не кончился
				tmp += a[num];
		}
		text[j] = tmp;
	}


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

	int *enc = seal.coding(text, L, key, n);

	cout << "coded" << endl;
	/*for (int j = 0; j < L; j++) 
		cout << (char)(enc[j]>>24);
	cout << endl;*/


	int *dec = seal.coding(enc, L, key, n);

	char *decoded = new char[L / 8];
	int d = 0;//индекс по decoded
	for (int i = 0; i < ceil((float)L / 32); i++) {
		for (int s = 3; s >= 0; s--) {
			decoded[d++] = dec[i] >> (8 * s);
		}
	}


	cout << "decoded:" << endl;
	for (int j = 0; j < L / 8; j++)
		cout << decoded[j];
	cout << endl;


	system("PAUSE");
}