#include "head.h"
using namespace std;

int main() {
	cout << "��ӭʹ��proximate��ͨģ��ϵͳ" << endl;
	cout << "ֱ��ģ���밴1��ʮ��·���밴2" << endl;
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