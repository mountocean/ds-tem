#include <graphics.h>
#include <stack>
#include <vector>
using std::stack;
using std::vector;

// ��Ϸ��Ϣ
#define WIN_WIDTH	400
#define WIN_HEIGHT	300
#define GAME_WIDTH	11
#define GAME_HEIGHT	11

#define WALL		1
#define GROUND		0
#define FILLSTATE	2
#define ENDPOS		3
#define FILLNUM		10

// ȫ�ֱ����б��������2λ�ã�
int		g_BlockSize;
int		g_GameMap[GAME_HEIGHT][GAME_WIDTH];
POINT	g_EndPos;
POINT   g_Player1Pos;	// ���1λ��
POINT   g_Player2Pos;	// ���2λ��
POINT	g_CameraPos;
IMAGE	g_MapImage;
UINT	g_BeginTime;
UINT	g_LastFillTime;

// �����б�
void initGame();				// ��ʼ����Ϸ
void endGame();					// ������Ϸ
void draw();					// ���ƺ���
bool upDate();					// ���ݸ��º���
void absDelay(int delay);		// �����ӳ�

bool canMove(POINT pos);		// �ж�ĳ��λ���Ƿ�����ƶ�
void computeCameraPos();		// ����������ڵ�ͼ�ϵ�λ��
void rePaintMap();				// �ػ��ͼ

void drawWall(POINT pos);		// ����ǽ��ͼ��ĺ���
void drawGround(POINT pos);		// ���Ƶ���ͼ��ĺ���
void drawFillState(POINT pos);	// �����͵�ͼ��ĺ���
void drawEndPos(POINT pos);		// �����յ�
void drawPlayer();				// ��������ĺ���
void drawView();				// ������Ұ


void initGame()
{
	g_BlockSize = 32;
	srand(GetTickCount());

	// ��ͼ�����߼����ֲ���...

	// ��ʼ�����λ�ã��������2��
	g_Player1Pos = { g_BlockSize * 3 / 2, g_BlockSize * 3 / 2 };
	g_Player2Pos = { g_BlockSize * 5 / 2, g_BlockSize * 5 / 2 }; // ��ͬ��ʼλ��
	computeCameraPos();
	initgraph(WIN_WIDTH, WIN_HEIGHT);
	setbkmode(TRANSPARENT);
	BeginBatchDraw();
}

bool upDate()
{
	// �������1�ƶ����������
	POINT nextPos1 = g_Player1Pos;
	if (GetKeyState(VK_UP) & 0x8000)	nextPos1.y -= 2;
	if (GetKeyState(VK_DOWN) & 0x8000)	nextPos1.y += 2;
	if (GetKeyState(VK_LEFT) & 0x8000)	nextPos1.x -= 2;
	if (GetKeyState(VK_RIGHT) & 0x8000)	nextPos1.x += 2;

	// �������2�ƶ���WASD����
	POINT nextPos2 = g_Player2Pos;
	if (GetKeyState('W') & 0x8000)		nextPos2.y -= 2;
	if (GetKeyState('S') & 0x8000)		nextPos2.y += 2;
	if (GetKeyState('A') & 0x8000)		nextPos2.x -= 2;
	if (GetKeyState('D') & 0x8000)		nextPos2.x += 2;

	// �ƶ��Ϸ��Լ�⣨�ֱ��⣩
	if (!canMove(nextPos1)) nextPos1 = adjustPosition(nextPos1, g_Player1Pos);
	if (!canMove(nextPos2)) nextPos2 = adjustPosition(nextPos2, g_Player2Pos);

	// �������λ��
	g_Player1Pos = nextPos1;
	g_Player2Pos = nextPos2;

	// �յ��ж�����һ��ҵ��Ｔͨ�أ�
	if (checkEndPos(g_Player1Pos) || checkEndPos(g_Player2Pos)) {
		showVictory();
		return false;
	}

	// ɾ����Ұ����߼�
	computeCameraPos();
	return true;
}

void draw()
{
	cleardevice();
	// ֱ�ӻ���������ͼ��ɾ����Ұ���ƣ�
	putimage(0, 0, WIN_WIDTH, WIN_HEIGHT, &g_MapImage, g_CameraPos.x, g_CameraPos.y);

	// ����˫���
	drawPlayer(g_Player1Pos, RGB(255, 0, 0));	// ��ɫ���1
	drawPlayer(g_Player2Pos, RGB(0, 0, 255));	// ��ɫ���2

	// ʱ����ʾ���ֲ���...
	FlushBatchDraw();
}

// ������������
POINT adjustPosition(POINT newPos, POINT oldPos)
{
	if (canMove({ oldPos.x, newPos.y })) return { oldPos.x, newPos.y };
	if (canMove({ newPos.x, oldPos.y })) return { newPos.x, oldPos.y };
	return oldPos;
}

bool checkEndPos(POINT pos)
{
	return g_GameMap[pos.y / g_BlockSize][pos.x / g_BlockSize] == ENDPOS;
}

void drawPlayer(POINT pos, COLORREF color)
{
	setfillcolor(color);
	solidcircle(pos.x - g_CameraPos.x, pos.y - g_CameraPos.y, 3);
}

// ������������ԭ���߼���ɾ���������ݣ�
// 1. g_ViewArray��ر���
// 2. drawView()����
// 3. ��Ұ˥�������߼�
