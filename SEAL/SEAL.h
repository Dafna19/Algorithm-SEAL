#pragma once
#include <stdio.h>

class SEAL {
private:
	//таблицы от ключа
	int T[512];
	int S[256];
	int *R;//размер зависит от L

	//результат G
	int H[5];

	//для построения таблиц
	void G(int a[5], int i);

	int K(int t);

	int f(int t, int B, int C, int D);

	//циклический сдвиг вправо: n - что, t - на сколько
	int shiftR(int n, int t);

	int Gamma(int a[5], int i);

	void makeTables(int a[5], int L);
};
