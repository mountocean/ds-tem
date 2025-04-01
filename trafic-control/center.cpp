#include "head.h"
using namespace std;

int main() {
	cout << "直线行走请按1，十字路口请按2" << endl;
	int panduan;
	cin >> panduan;
	if (panduan == 1) {
		main1();
	}
	else {
		return 0;
	}
}