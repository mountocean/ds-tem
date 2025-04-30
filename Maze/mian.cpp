#include "head.h"

int main()
{
	initGame();

	while (1)
	{
		if (!upDate()) break;	// 更新
		draw();					// 绘制
		absDelay(16);			// 绝对延迟 16 毫秒，控制每秒 60 帧
	}

	endGame();
	return 0;
}