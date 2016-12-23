﻿#pragma once
#include <stdio.h>

class SEAL {
private:
	//таблицы от ключа
	int T[512];
	int S[256];
	int *R;//размер зависит от L

	int P, Q, numb = 0x7fc;
	int A, B, C, D, n1, n2, n3, n4;
	//для тестов
	int ones, zeros, all;
	int n[4];

	//результат G
	int H[5];

	//для построения таблиц
	void G(int a[5], int i);

	int K(int t);

	int f(int t, int B, int C, int D);

	//циклический сдвиг вправо: n - что сдвигаем, t - на сколько
	int shiftR(int n, int t);

	int Gamma(int a[5], int i);

	void makeTables(int a[5], int L);

	int* Seal(int a[5], int n, int L);

	void initialize(int n, int l);

	void freqTest(int *x, int size);

	//Последовательный тест
	void serialTest(int *x, int size);

	void runsTest(int *x, int size);

	void autocorrTest(int *x, int size);

	void universTest(int *x, int size);

public:
	int* coding(int *text, int L, int a[5], int n);

	void tests(int L, int a[5], int n);
};
