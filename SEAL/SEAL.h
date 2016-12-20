#pragma once
#include <stdio.h>

class SEAL {
private:
	//������� �� �����
	int T[512];
	int S[256];
	int *R;//������ ������� �� L

	//��������� G
	int H[5];

	//��� ���������� ������
	void G(int a[5], int i);

	int K(int t);

	int f(int t, int B, int C, int D);

	//����������� ����� ������: n - ���, t - �� �������
	int shiftR(int n, int t);

	int Gamma(int a[5], int i);

	void makeTables(int a[5], int L);
};
