#include "head.h"

int main()
{
	initGame();

	while (1)
	{
		if (!upDate()) break;	// ����
		draw();					// ����
		absDelay(16);			// �����ӳ� 16 ���룬����ÿ�� 60 ֡
	}

	endGame();
	return 0;
}