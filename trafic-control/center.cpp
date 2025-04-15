#include "head.h"
using namespace std;

int main() {
	cout << "欢迎使用proximate交通模拟系统" << endl;
	cout << "直线模拟请按1，十字路口请按2" << endl;
	int panduan;
	cin >> panduan;
	if (panduan == 1) {
		main1();
	}
	else if (panduan == 2) {
		main2();
	}
	else {
		return 0;
	}
}