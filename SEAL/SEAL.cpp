#include "SEAL.h"
#include<iostream>
#include<cmath>
#include <time.h>
using namespace std;

void SEAL::G(int a[5], int i) {
	int W[80];
	W[0] = i;
	for (int y = 1; y < 16; y++)
		W[y] = 0;
	for (int t = 16; t < 80; t++) {
		W[t] = W[t - 3] ^ W[t - 8] ^ W[t - 14] ^ W[t - 16];
	}
	int A = a[0], B = a[1], C = a[2], D = a[3], E = a[4];

	for (int t = 0; t < 80; t++) {
		int tmp = shiftR(A, 27) + f(t, B, C, D) + E + W[t] + K(t);
		E = D;
		D = C;
		C = shiftR(B, 2);
		B = A;
		A = tmp;
	}

	H[0] = a[0] + A;
	H[1] = a[1] + B;
	H[2] = a[2] + C;
	H[3] = a[3] + D;
	H[4] = a[4] + E;
}

int SEAL::K(int t) {
	if (t >= 0 && t <= 19)
		return 0x5a827999;
	if (t >= 20 && t <= 39)
		return 0x6ed9eba1;
	if (t >= 40 && t <= 59)
		return 0x8f1bbcdc;
	if (t >= 60 && t <= 79)
		return 0xca62c1d6;
}

int SEAL::f(int t, int B, int C, int D) {
	if (t >= 0 && t <= 19)
		return (B&C) | ((~B)&D);
	if (t >= 20 && t <= 39 || t >= 60 && t <= 79)
		return B^C^D;
	if (t >= 40 && t <= 59)
		return (B&C) | (B&D) | (C&D);
}

int SEAL::shiftR(int n, int t) {
	//если слева будут 1 
	int buf = 0;
	for (int i = 0; i < 32 - t; i++) {
		buf = buf << 1;
		buf += 1;
	}
	int p1 = (n >> t)&buf;
	int p2 = n << (32 - t);
	return p1 | p2;
}

int SEAL::Gamma(int a[5], int i) {
	G(a, i / 5);
	return H[i % 5];
}

void SEAL::makeTables(int a[5], int L) {//L бит
	for (int i = 0; i < 512; i++)
		T[i] = Gamma(a, i);
	for (int j = 0; j < 256; j++)
		S[j] = Gamma(a, 0x1000 + j);
	int N = 4 * ceil((float)(L - 1) / 8192);
	R = new int[N];
	for (int k = 0; k < N; k++)
		R[k] = Gamma(a, 0x2000 + k);
}

int* SEAL::Seal(int a[5], int n, int L) {//L бит
	int len = ceil((float)L / 128) * 4;
	int *y = new int[len];
	for (int i = 0; i < len; i++)
		y[i] = 0;

	int k = 0;
	for (int l = 0; ; l++) {
		initialize(n, l);
		for (int i = 1; i <= 64; i++) {
			P = A&numb;
			B = B + T[P / 4];
			A = shiftR(A, 9);
			B = B^A;

			Q = B&numb;
			C = C^T[Q / 4];
			B = shiftR(B, 9);
			C = C + B;

			P = (P + C)&numb;
			D = D + T[P / 4];
			C = shiftR(C, 9);
			D = D^C;

			Q = (Q + D)&numb;
			A = A^T[Q / 4];
			D = shiftR(D, 9);
			A = A + D;

			P = (P + A)&numb;
			B = B^T[P / 4];
			A = shiftR(A, 9);

			Q = (Q + B)&numb;
			C = C + T[Q / 4];
			B = shiftR(B, 9);

			P = (P + C)&numb;
			D = D^T[P / 4];
			C = shiftR(C, 9);

			Q = (Q + D)&numb;
			A = A + T[Q / 4];
			D = shiftR(D, 9);

			y[k++] = B + S[4 * i - 4];
			y[k++] = C^S[4 * i - 3];
			y[k++] = D + S[4 * i - 2];
			y[k++] = A^S[4 * i - 1];
			if (k >= ceil((float)L / 32))
				return y;
			if (i % 2 == 0) {//четное
				A = A + n1;
				C = C + n2;
			}
			else {
				A = A + n3;
				C = C + n4;
			}
		}
	}
}

void SEAL::initialize(int n, int l) {
	A = n^R[4 * l];
	B = shiftR(n, 8) ^ R[4 * l + 1];
	C = shiftR(n, 16) ^ R[4 * l + 2];
	D = shiftR(n, 24) ^ R[4 * l + 3];
	for (int j = 1; j <= 2; j++) {
		P = A&numb;
		B = B + T[P / 4];
		A = shiftR(A, 9);

		P = B&numb;
		C = C + T[P / 4];
		B = shiftR(B, 9);

		P = C&numb;
		D = D + T[P / 4];
		C = shiftR(C, 9);

		P = D&numb;
		A = A + T[P / 4];
		D = shiftR(D, 9);
	}
	n1 = A;
	n2 = B;
	n3 = C;
	n4 = D;

	P = A&numb;
	B = B + T[P / 4];
	A = shiftR(A, 9);

	P = B&numb;
	C = C + T[P / 4];
	B = shiftR(B, 9);

	P = C&numb;
	D = D + T[P / 4];
	C = shiftR(C, 9);

	P = D&numb;
	A = A + T[P / 4];
	D = shiftR(D, 9);
}

void SEAL::freqTest(int * x, int size) {
	ones = zeros = 0;
	for (int i = 0; i < size; i++) {
		int t = x[i];
		for (int j = 0; j < 32; j++) {
			if (t % 2 == 0) zeros++;
			else ones++;
			t = t >> 1;
		}
	}
	all = ones + zeros;
	float X1 = ((float)pow((zeros - ones), 2)) / all;
	if (X1 < 3.8415)
		cout << "freqTest passed with alpha = 0.05" << endl;
	else if (X1 < 7.8794)
		cout << "freqTest passed with alpha = 0.005" << endl;
	else
		cout << "freqTest failed" << endl;
}

void SEAL::serialTest(int * x, int size) {
	// ones, zeros уже посчитаны
	n[0] = n[1] = n[2] = n[3] = 0;
	for (int i = 0; i < size; i++) {
		int t = x[i];
		for (int j = 0; j < 32; j++) {
			int ind = t & 3;//то же что t % 4
			n[ind]++;
			t = t >> 1;
		}
	}
	int sqr = 0; //n00^2+n01^2+n10^2+n11^2
	for (int i = 0; i < 4; i++) sqr += pow(n[i], 2);
	float a = ((float)4) / (all - 1), b = (float)2 / all;
	float X2 = a*sqr - b*(pow(zeros, 2) + pow(ones, 2)) + 1;

	if (X2 < 5.9915)
		cout << "serialTest passed with alpha = 0.05" << endl;
	else if (X2 < 10.5966)
		cout << "serialTest passed with alpha = 0.005" << endl;
	else
		cout << "serialTest failed" << endl;
}

void SEAL::runsTest(int * x, int size) {
	int e[20];//число ожидаемых серий (но индексация от 0)
	int k;
	for (int i = 0; i < 20; i++) {
		e[i] = (size * 32 - i + 2) / pow(2, i + 3);
		if (e[i] < 5) {
			k = i;
			break;
		}
	}
	int *B = new int[k];//1   B[0] = B1; B[k-1] = Bk
	int *G = new int[k];//0
	for (int i = 0; i < k; i++)
		B[i] = G[i] = 0;

	int prev;
	int len = 0;//длина серии
	for (int i = 0; i < size; i++) {
		int t = x[i];
		for (int j = 31; j >= 0; j--) {
			if (len == 0) {
				prev = (t >> j) % 2;
				len = 1;
				continue;
			}
			if ((t >> j) % 2 == prev)
				len++;
			else {
				len = fmin(len, k);
				if (prev)//если была серия из 1
					B[len - 1]++;
				else G[len - 1]++;
				len = 1;
				prev = (prev + 1) % 2;
			}
		}
	}
	float sum1 = 0, sum2 = 0;
	for (int i = 0; i < k; i++) {
		sum1 += pow(B[i] - e[i], 2) / e[i];
		sum2 += pow(G[i] - e[i], 2) / e[i];
	}
	float X3 = sum1 + sum2;
	float table[6][3] = {
		{1, 3.8415, 7.8794},//степень свободы 1
		{2, 5.9915, 10.5966},
		{4, 9.4877, 14.8603},
		{10, 18.307, 25.1882},
		{20, 31.4104, 39.9968},
		{22, 33.9244, 42.7957}
	};
	int v;
	for (int i = 5; i >= 0; i--) {
		if (2 * k - 2 >= table[i][0]) {
			v = i;
			break;
		}
	}

	if (X3 < table[v][1])
		cout << "runsTest passed with alpha = 0.05" << endl;
	else if (X3 < table[v][2])
		cout << "runsTest passed with alpha = 0.005" << endl;
	else
		cout << "runsTest failed " << endl;
}

void SEAL::autocorrTest(int * x, int size) {
	//all посчитано
	srand(time(NULL));
	int d = rand() % ((size * 32 - 1) / 2) + 1;//сдвиг
	int A = 0;
	for (int i = 0; i < size; i++) {
		int t = x[i];
		for (int j = 0; j < 32; j++) {
			int a = (t >> d) % 2;
			int b = t % 2;
			if (a^b > 0)
				A++;
			t = t >> 1;
		}
	}
	float up = 2 * (A - ((float)(all - d)) / 2);
	float X4 = up / sqrt(all - d);
	if (X4 < 1.6449)
		cout << "autocorrTest passed with alpha = 0.05" << endl;
	else if (X4 < 2.5758)
		cout << "autocorrTest passed with alpha = 0.005" << endl;
	else
		cout << "autocorrTest failed" << endl;
}

void SEAL::universTest(int * x, int size) {
	srand(time(NULL));
	int L = 9;
	int number = 1;//маска
	for (int i = 0; i < L - 1; i++) {
		number = number << 1;
		number++;
	}
	int Q = ceil((size * 32 * 0.2) / L);
	int K = size * 32 / L - Q;
	int *T = new int[Q + K];
	for (int j = 0; j < Q + K; j++)
		T[j] = 0;

	int q = 0;
	float sum = 0;
	for (int i = 0; i < size; i++) {
		int t = x[i];
		for (int j = 0; j < ceil(((float)32) / L); j++) {
			if (q < Q) { //инициализация
				T[t&number] = q;
			}
			if (q >= Q && q < Q + K) {
				sum += log10(q - T[t&number]);
				T[t&number] = q;
			}
			t = t >> L;
			q++;
		}
		if (q >= Q + K) break;
	}
	float Xu = sum / K;
	if (Xu < 1.6449)
		cout << "universTest passed with alpha = 0.05" << endl;
	else if (Xu < 2.5758)
		cout << "universTest passed with alpha = 0.005" << endl;
	else
		cout << "universTest failed" << endl;
}

int* SEAL::coding(int * text, int L, int a[5], int n) {//L бит
	int *coded = new int[ceil((float)L / 32)];
	int *y;
	makeTables(a, L);
	y = Seal(a, n, L);//создали гамму-последовательность
	for (int i = 0; i < ceil((float)L / 32); i++)
		coded[i] = text[i] ^ y[i];
	return coded;
}

void SEAL::tests(int L, int a[5], int n) {
	int *y;
	makeTables(a, L);
	y = Seal(a, n, L);//создали гамму-последовательность

	freqTest(y, ceil((float)L / 128) * 4);//кол-во int'ов
	serialTest(y, ceil((float)L / 128) * 4);
	runsTest(y, ceil((float)L / 128) * 4);
	autocorrTest(y, ceil((float)L / 128) * 4);
	universTest(y, ceil((float)L / 128) * 4);
}
