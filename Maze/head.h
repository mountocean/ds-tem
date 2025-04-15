#include <graphics.h>
#include <stack>
#include <vector>
using std::stack;				// 使用STL的栈
using std::vector;				// 使用STL的数组容器


void initGame();				// 初始化游戏
void endGame();					// 结束游戏
void draw();					// 绘制函数
bool upDate();					// 数据更新函数
void absDelay(int delay);		// 绝对延迟