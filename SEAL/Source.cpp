#include<iostream>
#include "SEAL.h"
#include<cmath>
using namespace std;

void main(int argc, char* argv[]) {
	int a = 32*1024*8;
	int b = ceil((float)a/8192);

	cout << b*4 << endl;


	system("PAUSE");
}