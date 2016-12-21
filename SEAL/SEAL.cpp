#include "SEAL.h"
#include<iostream>

void SEAL::G(int a[5], int i){
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

int SEAL::K(int t){
	if (t >= 0 && t <= 19)
		return 0x5a827999;
	if (t >= 20 && t <= 39)
		return 0x6ed9eba1;
	if (t >= 40 && t <= 59)
		return 0x8f1bbcdc;
	if (t >= 60 && t <= 79)
		return 0xca62c1d6;
}

int SEAL::f(int t, int B, int C, int D){
	if (t >= 0 && t <= 19)
		return (B&C) | ((~B)&D);
	if (t >= 20 && t <= 39 || t >= 60 && t <= 79)
		return B^C^D;
	if (t >= 40 && t <= 59)
		return (B&C) | (B&D) | (C&D);
}

int SEAL::shiftR(int n, int t){
	//если слева будут 1
	int buf = 0;
	for (int i = 0; i < 32 - t; i++) {
		buf = buf << 1;
		buf += 1;
	}
	int p1 = (n >> t)&buf;
	int p2 = n << (32 - t);
	return p1|p2;
}

int SEAL::Gamma(int a[5], int i){
	G(a, i / 5);
	return H[i%5];
}

void SEAL::makeTables(int a[5], int L){//L бит
	for (int i = 0; i < 512; i++)
		T[i] = Gamma(a, i);
	for (int j = 0; j < 256; j++)
		S[j] = Gamma(a, 0x1000 + j);
	int N = 4 * ceil((float)(L - 1) / 8192);
	R = new int[N];
	for (int k = 0; k < N; k++)
		R[k] = Gamma(a, 0x2000 + k);
}

int* SEAL::Seal(int a[5], int n, int L){//L бит
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
			if (k >= L / 32)
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

void SEAL::initialize(int n, int l){
	A = n^R[4*l];
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

int* SEAL::coding(int * text, int L, int a[5], int n){//L бит
	int *coded = new int[L/32];
	int *y;
	makeTables(a, L);
	y = Seal(a, n, L);//создали гамму-последовательность
	for (int i = 0; i < L / 32; i++)
		coded[i] = text[i] ^ y[i];

	return coded;
}
