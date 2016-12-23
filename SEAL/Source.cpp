#include<iostream>
#include<cmath>
#include "SEAL.h"
using namespace std;
/*
1. ��������� ����			
2. ���������������� ����	
3. ���� �����				
4. ������������������ ����	
5. ������������� ����		
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
	L = i * 8;//i char'��, �.�. 8*i ���
	int *text = new int[ceil((float)i / 4)];//��� ��������, ������ � 4 ���� ������

	//��� ����� ��������� ���� � ��� ��� ������ �����	
	for (int j = 0; j < ceil((float)i / 4); j++) {
		int tmp = 0;
		for (int k = 0; k < 4; k++) {
			int num = 4 * j + k;
			tmp = tmp << 8;
			if (num < i)//���� ����� ��� �� ��������
				tmp += a[num];
		}
		text[j] = tmp;
	}

	
	int key[5];
	cout << "write your key (5 numbers):" << endl;
	for (int i = 0; i < 5; i++)
		cin >> key[i];
	

	cout << "write n:" << endl;
	cin >> n;

	int *enc = seal.coding(text, L, key, n);

	cout << "coded" << endl;

	int *dec = seal.coding(enc, L, key, n);

	char *decoded = new char[L / 8];
	int d = 0;//������ �� decoded
	for (int i = 0; i < ceil((float)L / 32); i++) {
		for (int s = 3; s >= 0; s--) {
			decoded[d++] = dec[i] >> (8 * s);
		}
	}

	cout << "\ndecoded:" << endl;
	for (int j = 0; j < L / 8; j++)
		cout << decoded[j];
	cout << endl;

	cout << "\ntests:\n";
	seal.tests(100000, key, n);


	system("PAUSE");
}