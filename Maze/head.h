#include <graphics.h>
#include <stack>
#include <vector>
using std::stack;				// ʹ��STL��ջ
using std::vector;				// ʹ��STL����������


void initGame();				// ��ʼ����Ϸ
void endGame();					// ������Ϸ
void draw();					// ���ƺ���
bool upDate();					// ���ݸ��º���
void absDelay(int delay);		// �����ӳ�