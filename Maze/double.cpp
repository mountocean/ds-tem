#include <graphics.h>
#include <stack>
#include <vector>
using std::stack;
using std::vector;

// 游戏信息
#define WIN_WIDTH	400
#define WIN_HEIGHT	300
#define GAME_WIDTH	11
#define GAME_HEIGHT	11

#define WALL		1
#define GROUND		0
#define FILLSTATE	2
#define ENDPOS		3
#define FILLNUM		10

// 全局变量列表（新增玩家2位置）
int		g_BlockSize;
int		g_GameMap[GAME_HEIGHT][GAME_WIDTH];
POINT	g_EndPos;
POINT   g_Player1Pos;	// 玩家1位置
POINT   g_Player2Pos;	// 玩家2位置
POINT	g_CameraPos;
IMAGE	g_MapImage;
UINT	g_BeginTime;
UINT	g_LastFillTime;

// 函数列表
void initGame();				// 初始化游戏
void endGame();					// 结束游戏
void draw();					// 绘制函数
bool upDate();					// 数据更新函数
void absDelay(int delay);		// 绝对延迟

bool canMove(POINT pos);		// 判断某个位置是否可以移动
void computeCameraPos();		// 计算摄像机在地图上的位置
void rePaintMap();				// 重绘地图

void drawWall(POINT pos);		// 绘制墙壁图块的函数
void drawGround(POINT pos);		// 绘制地面图块的函数
void drawFillState(POINT pos);	// 绘制油灯图块的函数
void drawEndPos(POINT pos);		// 绘制终点
void drawPlayer();				// 绘制人物的函数
void drawView();				// 绘制视野


void initGame()
{
	g_BlockSize = 32;
	srand(GetTickCount());

	// 地图生成逻辑保持不变...

	// 初始化玩家位置（新增玩家2）
	g_Player1Pos = { g_BlockSize * 3 / 2, g_BlockSize * 3 / 2 };
	g_Player2Pos = { g_BlockSize * 5 / 2, g_BlockSize * 5 / 2 }; // 不同初始位置
	computeCameraPos();
	initgraph(WIN_WIDTH, WIN_HEIGHT);
	setbkmode(TRANSPARENT);
	BeginBatchDraw();
}

bool upDate()
{
	// 处理玩家1移动（方向键）
	POINT nextPos1 = g_Player1Pos;
	if (GetKeyState(VK_UP) & 0x8000)	nextPos1.y -= 2;
	if (GetKeyState(VK_DOWN) & 0x8000)	nextPos1.y += 2;
	if (GetKeyState(VK_LEFT) & 0x8000)	nextPos1.x -= 2;
	if (GetKeyState(VK_RIGHT) & 0x8000)	nextPos1.x += 2;

	// 处理玩家2移动（WASD键）
	POINT nextPos2 = g_Player2Pos;
	if (GetKeyState('W') & 0x8000)		nextPos2.y -= 2;
	if (GetKeyState('S') & 0x8000)		nextPos2.y += 2;
	if (GetKeyState('A') & 0x8000)		nextPos2.x -= 2;
	if (GetKeyState('D') & 0x8000)		nextPos2.x += 2;

	// 移动合法性检测（分别检测）
	if (!canMove(nextPos1)) nextPos1 = adjustPosition(nextPos1, g_Player1Pos);
	if (!canMove(nextPos2)) nextPos2 = adjustPosition(nextPos2, g_Player2Pos);

	// 更新玩家位置
	g_Player1Pos = nextPos1;
	g_Player2Pos = nextPos2;

	// 终点判定（任一玩家到达即通关）
	if (checkEndPos(g_Player1Pos) || checkEndPos(g_Player2Pos)) {
		showVictory();
		return false;
	}

	// 删除视野相关逻辑
	computeCameraPos();
	return true;
}

void draw()
{
	cleardevice();
	// 直接绘制完整地图（删除视野限制）
	putimage(0, 0, WIN_WIDTH, WIN_HEIGHT, &g_MapImage, g_CameraPos.x, g_CameraPos.y);

	// 绘制双玩家
	drawPlayer(g_Player1Pos, RGB(255, 0, 0));	// 红色玩家1
	drawPlayer(g_Player2Pos, RGB(0, 0, 255));	// 蓝色玩家2

	// 时间显示保持不变...
	FlushBatchDraw();
}

// 新增辅助函数
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

// 其他函数保持原有逻辑，删除以下内容：
// 1. g_ViewArray相关变量
// 2. drawView()函数
// 3. 视野衰减计算逻辑
