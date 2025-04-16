#include <graphics.h>
#include <stack>
#include <vector>
#include <queue>
#include <cmath>
using std::stack;
using std::vector;
using std::queue;

// 游戏信息
#define WIN_WIDTH   800         // 窗口宽度(像素)
#define WIN_HEIGHT  600         // 窗口高度(像素)
// 注意：由于随机生成算法的原因，地图宽高只能为奇数
#define GAME_WIDTH  11          // 地图宽度(块)
#define GAME_HEIGHT 11          // 地图高度(块)

#define WALL        1           // 墙壁标记
#define GROUND      0           // 地面标记
#define FILLSTATE   2           // 油灯（加油站）标记
#define ENDPOS      3           // 终点标记

#define MAXVIEW     2.0         // 最大视野
#define MINVIEW     1           // 最小视野
#define FILLNUM     10          // 加油站数量
#define DARKTIME    12          // 每下降一图块视野所需时间(秒)

// 全局变量
int     g_BlockSize;                        // 图块尺寸
int     g_GameMap[GAME_HEIGHT][GAME_WIDTH]; // 地图数组（以块为单位）
POINT   g_EndPos;                           // 终点位置（网格坐标）
POINT   g_PlayerPos;                        // 玩家1在地图上的位置（像素坐标）
POINT   g_CameraPos;                        // 摄像机在地图上的位置（左上角，像素坐标）
IMAGE   g_MapImage;                         // 地图图片
double  g_ViewArray;                        // 当前视野范围（以图块为单位）
UINT    g_BeginTime;                        // 游戏开始时的时间
UINT    g_LastFillTime;                     // 最近一次加油的时间

// 自动寻路相关（仅对玩家1有效）
bool    g_autoMode = false;                 // 玩家1是否处于自动寻路模式，初始为 false
vector<POINT> g_path;                       // 自动寻路计算出的路径（玩家1），路径中的每个点为网格坐标

// 新增：双人模式相关
bool    g_twoPlayerMode = false;            // 是否处于双人模式，初始为 false
POINT   g_Player2Pos;                       // 玩家2在地图上的位置（像素坐标）

// 函数列表
void initGame();
void endGame();
void draw();
bool upDate();
void absDelay(int delay);

bool canMove(POINT pos);
void computeCameraPos();
void rePaintMap();

void drawWall(POINT pos);
void drawGround(POINT pos);
void drawFillState(POINT pos);
void drawEndPos(POINT pos);
void drawPlayer();      // 绘制玩家1
void drawPlayer2();     // 新增：绘制玩家2
void drawView();

// 自动寻路（玩家1）
vector<POINT> findPath();   // 使用 BFS 算法，在网格上计算从玩家1当前位置到终点的最短路径

int main()
{
    initGame();

    while (1)
    {
        if (!upDate()) break;   // 更新数据
        draw();                 // 绘制画面
        absDelay(16);           // 延迟 16 毫秒，控制帧率约 60 FPS
    }

    endGame();
    return 0;
}

void initGame()
{
    g_BlockSize = 32;           // 初始图块大小为 32 像素
    srand(GetTickCount());      // 初始化随机数生成器

    // 初始化地图：奇数行奇数列为可走区域，其余设为墙壁
    for (int i = 0; i < GAME_HEIGHT; i++)
    {
        for (int j = 0; j < GAME_WIDTH; j++)
        {
            if (i % 2 == 0 || j % 2 == 0)
                g_GameMap[i][j] = WALL;
            else
                g_GameMap[i][j] = GROUND;
        }
    }

    // 随机生成迷宫（使用深度优先遍历算法）
    stack<POINT> stepStack;
    vector<POINT>  stepPoint;
    POINT nowPoint;
    stepStack.push({ 1, 1 });
    nowPoint = { 1, 1 };
    g_GameMap[1][1] = 0xFFFF;   // 标记起点
    while (!stepStack.empty())
    {
        // 获取四个方向相邻点（间隔为2）
        POINT tempPoint;
        for (int i = -1; i <= 1; i += 2)
        {
            tempPoint = { nowPoint.x, nowPoint.y + i * 2 };
            if (tempPoint.x >= 0 && tempPoint.x < GAME_WIDTH &&
                tempPoint.y >= 0 && tempPoint.y < GAME_HEIGHT &&
                g_GameMap[tempPoint.y][tempPoint.x] != 0xFFFF)
            {
                stepPoint.push_back(tempPoint);
            }
            tempPoint = { nowPoint.x + i * 2, nowPoint.y };
            if (tempPoint.x >= 0 && tempPoint.x < GAME_WIDTH &&
                tempPoint.y >= 0 && tempPoint.y < GAME_HEIGHT &&
                g_GameMap[tempPoint.y][tempPoint.x] != 0xFFFF)
            {
                stepPoint.push_back(tempPoint);
            }
        }

        if (stepPoint.empty())
        {
            stepStack.pop();
            if (!stepStack.empty())
                nowPoint = stepStack.top();
        }
        else
        {
            stepStack.push(stepPoint[rand() % stepPoint.size()]);
            g_GameMap[(nowPoint.y + stepStack.top().y) / 2][(nowPoint.x + stepStack.top().x) / 2] = 0;
            nowPoint = stepStack.top();
            g_GameMap[nowPoint.y][nowPoint.x] = 0xFFFF;
        }
        stepPoint.clear();
    }

    // 清除标记
    for (int i = 0; i < GAME_HEIGHT; i++)
    {
        for (int j = 0; j < GAME_WIDTH; j++)
        {
            if (g_GameMap[i][j] == 0xFFFF)
                g_GameMap[i][j] = 0;
        }
    }

    // 随机生成油灯（加油站）位置
    for (int i = 0; i < FILLNUM; i++)
    {
        POINT fillPoint = { rand() % GAME_WIDTH, rand() % GAME_HEIGHT };
        while (g_GameMap[fillPoint.y][fillPoint.x] != GROUND)
            fillPoint = { rand() % GAME_WIDTH, rand() % GAME_HEIGHT };
        g_GameMap[fillPoint.y][fillPoint.x] = FILLSTATE;
    }

    // 设置终点位置
    g_GameMap[GAME_HEIGHT - 2][GAME_WIDTH - 2] = ENDPOS;
    g_EndPos = { GAME_WIDTH - 2, GAME_HEIGHT - 2 };

    g_ViewArray = MAXVIEW;
    g_BeginTime = GetTickCount();
    g_LastFillTime = GetTickCount();
    rePaintMap();

    // 初始化玩家1位置（放在起点网格的中心）
    g_PlayerPos = { g_BlockSize * 3 / 2, g_BlockSize * 3 / 2 };
    // 初始化玩家2位置（双人模式下从另选一可走点，如在第一行右侧；这里取网格( GAME_WIDTH-2, 1 )）
    g_Player2Pos = { g_BlockSize * 3 / 2, g_BlockSize * 3 / 2 };

    computeCameraPos();
    initgraph(WIN_WIDTH, WIN_HEIGHT);
    setbkmode(TRANSPARENT);
    BeginBatchDraw();
}

void endGame()
{
    EndBatchDraw();
    closegraph();
}

void draw()
{
    cleardevice();
    drawView();
    drawPlayer();
    if (g_twoPlayerMode)
        drawPlayer2();

    // 绘制游戏时间
    TCHAR timeStr[256];
    int loseTime = GetTickCount() - g_BeginTime;
    _stprintf_s(timeStr, _T("游戏时间:%02d:%02d"), loseTime / 1000 / 60, loseTime / 1000 % 60);
    settextcolor(RGB(140, 140, 140));
    outtextxy((WIN_WIDTH - textwidth(timeStr)) / 2, 3, timeStr);

    FlushBatchDraw();
}

bool upDate()
{
    // 检测 O 键切换双人模式（开关），按 O 切换后，系统会在下次更新时激活或关闭双人模式
    if (GetAsyncKeyState('O') & 1)
    {
        g_twoPlayerMode = !g_twoPlayerMode;
        // 如开启双人模式，可重新设置玩家2的初始位置（此处选取网格 (GAME_WIDTH-2, 1) 的中心）
        if (g_twoPlayerMode)
            g_Player2Pos = g_PlayerPos;
    }

    // 玩家1的自动寻路开关（P 键，保持原有玩法）
    if (GetAsyncKeyState('P') & 1)
    {
        if (g_autoMode)
        {
            g_autoMode = false;
            g_path.clear();
        }
        else
        {
            g_path = findPath();
            if (!g_path.empty())
                g_autoMode = true;
        }
    }

    // 如果玩家1处于自动寻路状态且检测到方向键按下，则取消自动寻路
    if ((GetKeyState(VK_UP) & 0x8000) || (GetKeyState(VK_DOWN) & 0x8000) ||
        (GetKeyState(VK_LEFT) & 0x8000) || (GetKeyState(VK_RIGHT) & 0x8000))
    {
        if (g_autoMode)
        {
            g_autoMode = false;
            g_path.clear();
        }
    }

    // 更新玩家1的位置
    if (g_autoMode)
    {
        if (!g_path.empty())
        {
            // 目标网格单元（路径第一点）的中心像素坐标
            POINT target;
            target.x = g_path.front().x * g_BlockSize + g_BlockSize / 2;
            target.y = g_path.front().y * g_BlockSize + g_BlockSize / 2;

            // 计算当前与目标的距离及方向
            double dx = target.x - g_PlayerPos.x;
            double dy = target.y - g_PlayerPos.y;
            double dist = sqrt(dx * dx + dy * dy);
            int speed = 2;  // 移动速度

            if (dist <= speed)
            {
                g_PlayerPos = target;
                g_path.erase(g_path.begin());
            }
            else
            {
                g_PlayerPos.x += int(speed * dx / dist);
                g_PlayerPos.y += int(speed * dy / dist);
            }
        }
        else
        {
            g_autoMode = false;
        }
    }
    else
    {
        // 玩家1手动控制（方向键）
        POINT nextPos = g_PlayerPos;
        if (GetKeyState(VK_UP) & 0x8000)    nextPos.y -= 2;
        if (GetKeyState(VK_DOWN) & 0x8000)  nextPos.y += 2;
        if (GetKeyState(VK_LEFT) & 0x8000)  nextPos.x -= 2;
        if (GetKeyState(VK_RIGHT) & 0x8000) nextPos.x += 2;

        if (!canMove(nextPos))
        {
            if (canMove({ g_PlayerPos.x, nextPos.y }))
                nextPos = { g_PlayerPos.x, nextPos.y };
            else if (canMove({ nextPos.x, g_PlayerPos.y }))
                nextPos = { nextPos.x, g_PlayerPos.y };
            else
                nextPos = g_PlayerPos;
        }
        // 如果走到油灯格子，更新时间
        int cellX = nextPos.x / g_BlockSize;
        int cellY = nextPos.y / g_BlockSize;
        if (g_GameMap[cellY][cellX] == FILLSTATE)
            g_LastFillTime = GetTickCount();
        // 检查胜利条件（玩家1）
        else if (g_GameMap[cellY][cellX] == ENDPOS)
        {
            outtextxy(WIN_WIDTH / 2 - 40, WIN_HEIGHT / 2 - 12, _T("玩家1胜利！"));
            FlushBatchDraw();
            Sleep(1000);
            return false;
        }
        g_PlayerPos = nextPos;
    }

    // 玩家2更新（仅在双人模式下处理，使用 WASD 键控制）
    if (g_twoPlayerMode)
    {
        POINT nextPos2 = g_Player2Pos;
        if (GetKeyState('W') & 0x8000)    nextPos2.y -= 2;
        if (GetKeyState('S') & 0x8000)    nextPos2.y += 2;
        if (GetKeyState('A') & 0x8000)    nextPos2.x -= 2;
        if (GetKeyState('D') & 0x8000)    nextPos2.x += 2;

        if (!canMove(nextPos2))
        {
            if (canMove({ g_Player2Pos.x, nextPos2.y }))
                nextPos2 = { g_Player2Pos.x, nextPos2.y };
            else if (canMove({ nextPos2.x, g_Player2Pos.y }))
                nextPos2 = { nextPos2.x, g_Player2Pos.y };
            else
                nextPos2 = g_Player2Pos;
        }
        // 同样检测油灯补给（共享时间）
        int cellX2 = nextPos2.x / g_BlockSize;
        int cellY2 = nextPos2.y / g_BlockSize;
        if (g_GameMap[cellY2][cellX2] == FILLSTATE)
            g_LastFillTime = GetTickCount();
        // 检查胜利条件（玩家2）
        else if (g_GameMap[cellY2][cellX2] == ENDPOS)
        {
            outtextxy(WIN_WIDTH / 2 - 40, WIN_HEIGHT / 2 - 12, _T("玩家2胜利！"));
            FlushBatchDraw();
            Sleep(1000);
            return false;
        }
        g_Player2Pos = nextPos2;
    }

    // 更新摄像机位置（双人模式下取两玩家中心的平均值，否则以玩家1为中心）
    computeCameraPos();

    // 根据时间缩减视野
    static unsigned int lastTime = GetTickCount();
    int loseTime = GetTickCount() - g_LastFillTime;
    g_ViewArray = MAXVIEW - loseTime / 1000.0 / DARKTIME;
    if (g_ViewArray < MINVIEW) g_ViewArray = MINVIEW;

    // 处理鼠标滚轮缩放
    MOUSEMSG mouseMsg;
    int lastBlockSize = g_BlockSize;
    while (MouseHit())
    {
        mouseMsg = GetMouseMsg();
        if (mouseMsg.uMsg == WM_MOUSEWHEEL)
            g_BlockSize += mouseMsg.wheel / 120;
    }
    if (lastBlockSize != g_BlockSize)
    {
        if (g_BlockSize >= 10 && g_BlockSize <= 50)
        {
            if (GAME_WIDTH * g_BlockSize < WIN_WIDTH ||
                GAME_HEIGHT * g_BlockSize < WIN_HEIGHT)
                g_BlockSize = lastBlockSize;
            rePaintMap();
            POINT mapPos = { g_PlayerPos.x / lastBlockSize, g_PlayerPos.y / lastBlockSize };
            g_PlayerPos.x = mapPos.x * g_BlockSize + g_BlockSize / 2;
            g_PlayerPos.y = mapPos.y * g_BlockSize + g_BlockSize / 2;
            // 对于玩家2也重新映射位置（双人模式下）
            if (g_twoPlayerMode)
            {
                POINT mapPos2 = { g_Player2Pos.x / lastBlockSize, g_Player2Pos.y / lastBlockSize };
                g_Player2Pos.x = mapPos2.x * g_BlockSize + g_BlockSize / 2;
                g_Player2Pos.y = mapPos2.y * g_BlockSize + g_BlockSize / 2;
            }
            computeCameraPos();
        }
        if (g_BlockSize < 10) g_BlockSize = 10;
        if (g_BlockSize > 50) g_BlockSize = 50;
    }
    return true;
}

void absDelay(int delay)
{
    static int curtime = GetTickCount();
    static int pretime = GetTickCount();
    while (curtime - pretime < delay)
    {
        curtime = GetTickCount();
        Sleep(1);
    }
    pretime = curtime;
}

bool canMove(POINT pos)
{
    return  g_GameMap[(pos.y - 3) / g_BlockSize][(pos.x - 3) / g_BlockSize] != WALL &&
        g_GameMap[(pos.y + 3) / g_BlockSize][(pos.x + 3) / g_BlockSize] != WALL &&
        g_GameMap[(pos.y - 3) / g_BlockSize][(pos.x + 3) / g_BlockSize] != WALL &&
        g_GameMap[(pos.y + 3) / g_BlockSize][(pos.x - 3) / g_BlockSize] != WALL;
}

void computeCameraPos()
{
    // 双人模式：以两玩家坐标的平均值作为摄像机中心
    int centerX, centerY;
    if (g_twoPlayerMode)
    {
        centerX = (g_PlayerPos.x + g_Player2Pos.x) / 2;
        centerY = (g_PlayerPos.y + g_Player2Pos.y) / 2;
    }
    else
    {
        centerX = g_PlayerPos.x;
        centerY = g_PlayerPos.y;
    }
    g_CameraPos.x = centerX - WIN_WIDTH / 2;
    g_CameraPos.y = centerY - WIN_HEIGHT / 2;
    if (g_CameraPos.x < 0) g_CameraPos.x = 0;
    if (g_CameraPos.y < 0) g_CameraPos.y = 0;
    if (g_CameraPos.x > GAME_WIDTH * g_BlockSize - WIN_WIDTH)
        g_CameraPos.x = GAME_WIDTH * g_BlockSize - WIN_WIDTH;
    if (g_CameraPos.y > GAME_HEIGHT * g_BlockSize - WIN_HEIGHT)
        g_CameraPos.y = GAME_HEIGHT * g_BlockSize - WIN_HEIGHT;
}

void rePaintMap()
{
    g_MapImage.Resize(GAME_WIDTH * g_BlockSize, GAME_HEIGHT * g_BlockSize);
    SetWorkingImage(&g_MapImage);
    for (int i = 0; i < GAME_HEIGHT; i++)
    {
        for (int j = 0; j < GAME_WIDTH; j++)
        {
            switch (g_GameMap[i][j])
            {
            case WALL:
                drawWall({ j * g_BlockSize, i * g_BlockSize });
                break;
            case FILLSTATE:
                drawFillState({ j * g_BlockSize, i * g_BlockSize });
                break;
            case GROUND:
                drawGround({ j * g_BlockSize, i * g_BlockSize });
                break;
            case ENDPOS:
                drawEndPos({ j * g_BlockSize, i * g_BlockSize });
                break;
            }
        }
    }
    SetWorkingImage();
}

void drawWall(POINT pos)
{
    setfillcolor(RGB(254, 109, 19));
    solidrectangle(pos.x, pos.y, pos.x + g_BlockSize, pos.y + g_BlockSize);
}

void drawGround(POINT pos)
{
    setfillcolor(RGB(255, 255, 255));
    solidrectangle(pos.x, pos.y, pos.x + g_BlockSize, pos.y + g_BlockSize);
}

void drawFillState(POINT pos)
{
    drawGround(pos);
    pos.x += g_BlockSize / 5;
    pos.y += g_BlockSize / 5;
    setfillcolor(RGB(252, 213, 11));
    solidroundrect(pos.x, pos.y, pos.x + g_BlockSize * 3 / 5, pos.y + g_BlockSize * 3 / 5, g_BlockSize / 8, g_BlockSize / 8);
}

void drawEndPos(POINT pos)
{
    drawGround(pos);
    pos.x += g_BlockSize / 5;
    pos.y += g_BlockSize / 5;
    setfillcolor(RGB(87, 116, 48));
    solidroundrect(pos.x, pos.y, pos.x + g_BlockSize * 3 / 5, pos.y + g_BlockSize * 3 / 5, g_BlockSize / 8, g_BlockSize / 8);
}

void drawPlayer()
{
    setfillcolor(RGB(252, 213, 11)); // 玩家1颜色（黄色）
    solidcircle(g_PlayerPos.x - g_CameraPos.x, g_PlayerPos.y - g_CameraPos.y, 3);
}

void drawPlayer2()
{
    setfillcolor(RGB(0, 255, 255));  // 玩家2颜色（青色）
    solidcircle(g_Player2Pos.x - g_CameraPos.x, g_Player2Pos.y - g_CameraPos.y, 3);
}

void drawView()
{
    HRGN viewArr;
    int r = int(g_BlockSize * g_ViewArray + 0.5);
    POINT orgin = g_PlayerPos;
    orgin.x -= g_CameraPos.x;
    orgin.y -= g_CameraPos.y;
    viewArr = CreateEllipticRgn(orgin.x - r, orgin.y - r, orgin.x + r, orgin.y + r);
    setcliprgn(viewArr);
    putimage(0, 0, WIN_WIDTH, WIN_HEIGHT, &g_MapImage, g_CameraPos.x, g_CameraPos.y);
    DeleteObject(viewArr);
    setcliprgn(NULL);
}

// 使用 BFS 算法在网格上寻找从玩家1当前位置到终点的最短路径
vector<POINT> findPath()
{
    vector<POINT> pathResult;
    // 玩家1当前位置（网格坐标）作为起点
    int startX = g_PlayerPos.x / g_BlockSize;
    int startY = g_PlayerPos.y / g_BlockSize;
    int endX = g_EndPos.x;
    int endY = g_EndPos.y;

    bool visited[GAME_HEIGHT][GAME_WIDTH] = { false };
    POINT parent[GAME_HEIGHT][GAME_WIDTH];
    for (int i = 0; i < GAME_HEIGHT; i++)
        for (int j = 0; j < GAME_WIDTH; j++)
            parent[i][j] = { -1, -1 };

    queue<POINT> q;
    q.push({ startX, startY });
    visited[startY][startX] = true;
    bool found = false;

    int dx[4] = { 0,  0, -1, 1 };
    int dy[4] = { -1, 1,  0, 0 };

    while (!q.empty())
    {
        POINT cur = q.front();
        q.pop();
        if (cur.x == endX && cur.y == endY)
        {
            found = true;
            break;
        }
        for (int i = 0; i < 4; i++)
        {
            int nx = cur.x + dx[i];
            int ny = cur.y + dy[i];
            if (nx >= 0 && nx < GAME_WIDTH && ny >= 0 && ny < GAME_HEIGHT &&
                !visited[ny][nx] && g_GameMap[ny][nx] != WALL)
            {
                visited[ny][nx] = true;
                parent[ny][nx] = cur;
                q.push({ nx, ny });
            }
        }
    }

    if (!found)
        return pathResult;

    vector<POINT> revPath;
    POINT cur = { endX, endY };
    while (!(cur.x == startX && cur.y == startY))
    {
        revPath.push_back(cur);
        cur = parent[cur.y][cur.x];
    }
    revPath.push_back({ startX, startY });
    for (int i = revPath.size() - 1; i >= 0; i--)
        pathResult.push_back(revPath[i]);

    return pathResult;
}
