#include <graphics.h>
#include <stack>
#include <vector>
#include <queue>
#include <cmath>
using std::stack;
using std::vector;
using std::queue;

// 游戏信息
#define WIN_WIDTH   400         // 窗口宽度(像素)
#define WIN_HEIGHT  300         // 窗口高度(像素)
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
POINT   g_PlayerPos;                        // 玩家在地图上的位置（像素坐标）
POINT   g_CameraPos;                        // 摄像机在地图上的位置（左上角，像素坐标）
IMAGE   g_MapImage;                         // 地图图片
double  g_ViewArray;                        // 当前视野范围（以图块为单位）
UINT    g_BeginTime;                        // 游戏开始时的时间
UINT    g_LastFillTime;                     // 最近一次加油的时间

// 新增自动寻路相关全局变量
bool    g_autoMode = false;                 // 是否处于自动寻路模式，初始为 false
vector<POINT> g_path;                       // 自动寻路计算出的路径，路径中每个点为网格坐标

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
void drawPlayer();
void drawView();

// 新增：自动寻路
vector<POINT> findPath();   // 使用 BFS 算法，在网格上计算从当前单元格到终点的最短路径

void initGame()
{
    g_BlockSize = 32;           // 初始图块大小为 32 像素
    srand(GetTickCount());      // 初始化随机数生成器

    // 初始化地图：所有奇数行、奇数列为可走区域，其他为墙壁
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
    stepStack.push({ 1,1 });
    nowPoint = { 1,1 };
    g_GameMap[1][1] = 0xFFFF;   // 标记起点
    while (!stepStack.empty())
    {
        // 获取四个方向的相邻点（间隔为2）
        POINT tempPoint;
        for (int i = -1; i <= 1; i += 2)
        {
            tempPoint = { nowPoint.x, nowPoint.y + i * 2 };
            if (tempPoint.x >= 0 && tempPoint.x <= GAME_WIDTH - 1 &&
                tempPoint.y >= 0 && tempPoint.y <= GAME_HEIGHT - 1 &&
                g_GameMap[tempPoint.y][tempPoint.x] != 0xFFFF)
            {
                stepPoint.push_back(tempPoint);
            }
            tempPoint = { nowPoint.x + i * 2, nowPoint.y };
            if (tempPoint.x >= 0 && tempPoint.x <= GAME_WIDTH - 1 &&
                tempPoint.y >= 0 && tempPoint.y <= GAME_HEIGHT - 1 &&
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

    // 设置终点
    g_GameMap[GAME_HEIGHT - 2][GAME_WIDTH - 2] = ENDPOS;
    g_EndPos = { GAME_WIDTH - 2, GAME_HEIGHT - 2 };

    g_ViewArray = MAXVIEW;
    g_BeginTime = GetTickCount();
    g_LastFillTime = GetTickCount();
    rePaintMap();

    // 初始化玩家位置（起始于 (1,1) 网格的中心附近）
    g_PlayerPos = { g_BlockSize * 3 / 2, g_BlockSize * 3 / 2 };
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
    // 检测 P 键：用于切换自动寻路模式（开启或关闭）
    // 使用 GetAsyncKeyState 检测按键“按下时边沿”
    if (GetAsyncKeyState('P') & 1)
    {
        if (g_autoMode)
        {
            // 关闭自动寻路模式
            g_autoMode = false;
            g_path.clear();
        }
        else
        {
            // 开启自动寻路模式：先清除手动输入，以免干扰
            g_path = findPath();
            if (!g_path.empty())
                g_autoMode = true;
        }
    }

    // 如果处于自动寻路模式，一旦有方向键的手动输入则退出自动寻路模式
    if ((GetKeyState(VK_UP) & 0x8000) || (GetKeyState(VK_DOWN) & 0x8000) ||
        (GetKeyState(VK_LEFT) & 0x8000) || (GetKeyState(VK_RIGHT) & 0x8000))
    {
        if (g_autoMode)
        {
            g_autoMode = false;
            g_path.clear();
        }
    }

    // 如果自动寻路模式启用，则根据计算出的路径自动移动玩家
    if (g_autoMode)
    {
        if (!g_path.empty())
        {
            // 目标网格单元（路径第一点）的中心像素坐标
            POINT target;
            target.x = g_path.front().x * g_BlockSize + g_BlockSize / 2;
            target.y = g_path.front().y * g_BlockSize + g_BlockSize / 2;

            // 计算当前位置与目标位置的距离及方向
            double dx = target.x - g_PlayerPos.x;
            double dy = target.y - g_PlayerPos.y;
            double dist = sqrt(dx * dx + dy * dy);
            int speed = 2;  // 自动移动速度与手动移动速度保持一致

            if (dist <= speed)
            {
                // 达到目标单元，直接置为目标，弹出该点
                g_PlayerPos = target;
                g_path.erase(g_path.begin());
            }
            else
            {
                // 按速度方向移动
                g_PlayerPos.x += int(speed * dx / dist);
                g_PlayerPos.y += int(speed * dy / dist);
            }
        }
        else
        {
            // 路径走完后自动模式结束
            g_autoMode = false;
        }
    }
    else
    {
        // 如果不在自动模式，则处理手动控制
        POINT nextPos = g_PlayerPos;
        if (GetKeyState(VK_UP) & 0x8000)    nextPos.y -= 2;
        if (GetKeyState(VK_DOWN) & 0x8000)  nextPos.y += 2;
        if (GetKeyState(VK_LEFT) & 0x8000)  nextPos.x -= 2;
        if (GetKeyState(VK_RIGHT) & 0x8000) nextPos.x += 2;

        // 判断是否可以移动
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
        // 如果走到终点则胜利
        else if (g_GameMap[cellY][cellX] == ENDPOS)
        {
            outtextxy(WIN_WIDTH / 2 - 40, WIN_HEIGHT / 2 - 12, _T("恭喜过关！"));
            FlushBatchDraw();
            Sleep(1000);
            return false;
        }
        g_PlayerPos = nextPos;
    }

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
    g_CameraPos.x = g_PlayerPos.x - WIN_WIDTH / 2;
    g_CameraPos.y = g_PlayerPos.y - WIN_HEIGHT / 2;
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
    setfillcolor(RGB(252, 213, 11));
    solidcircle(g_PlayerPos.x - g_CameraPos.x, g_PlayerPos.y - g_CameraPos.y, 3);
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

// 新增：使用 BFS 在网格上寻找从玩家当前位置到终点的最短路径
vector<POINT> findPath()
{
    vector<POINT> pathResult;
    // 将玩家当前所在的地图单元坐标作为起点
    int startX = g_PlayerPos.x / g_BlockSize;
    int startY = g_PlayerPos.y / g_BlockSize;
    int endX = g_EndPos.x;
    int endY = g_EndPos.y;

    // 用于标记访问状态
    bool visited[GAME_HEIGHT][GAME_WIDTH] = { false };
    // 记录父节点，用于回溯路径
    POINT parent[GAME_HEIGHT][GAME_WIDTH];
    for (int i = 0; i < GAME_HEIGHT; i++)
        for (int j = 0; j < GAME_WIDTH; j++)
            parent[i][j] = { -1, -1 };

    queue<POINT> q;
    q.push({ startX, startY });
    visited[startY][startX] = true;
    bool found = false;

    // 四个方向：上、下、左、右
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
            // 判断边界和是否为墙（墙为不可走区域）
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
        return pathResult; // 无法找到路径

    // 反向回溯路径，从终点到起点
    vector<POINT> revPath;
    POINT cur = { endX, endY };
    while (!(cur.x == startX && cur.y == startY))
    {
        revPath.push_back(cur);
        cur = parent[cur.y][cur.x];
    }
    revPath.push_back({ startX, startY });
    // 将路径正向排列（从起点到终点）
    for (int i = revPath.size() - 1; i >= 0; i--)
        pathResult.push_back(revPath[i]);

    return pathResult;
}
