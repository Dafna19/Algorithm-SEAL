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

void SEAL::makeTables(int a[5], int L){
	for (int i = 0; i < 512; i++)
		T[i] = Gamma(a, i);
	for (int j = 0; j < 256; j++)
		S[j] = Gamma(a, 0x1000 + j);
	int N = 4 * ceil((float)(L - 1) / 8192);
	R = new int[N];
	for (int k = 0; k < N; k++)
		R[k] = Gamma(a, 0x2000 + k);
}
