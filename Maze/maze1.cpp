#include <graphics.h>
#include <stack>
#include <vector>
#include <queue>
#include <cmath>
using std::stack;
using std::vector;
using std::queue;

// ��Ϸ��Ϣ
#define WIN_WIDTH   400         // ���ڿ��(����)
#define WIN_HEIGHT  300         // ���ڸ߶�(����)
// ע�⣺������������㷨��ԭ�򣬵�ͼ���ֻ��Ϊ����
#define GAME_WIDTH  11          // ��ͼ���(��)
#define GAME_HEIGHT 11          // ��ͼ�߶�(��)

#define WALL        1           // ǽ�ڱ��
#define GROUND      0           // ������
#define FILLSTATE   2           // �͵ƣ�����վ�����
#define ENDPOS      3           // �յ���

#define MAXVIEW     2.0         // �����Ұ
#define MINVIEW     1           // ��С��Ұ
#define FILLNUM     10          // ����վ����
#define DARKTIME    12          // ÿ�½�һͼ����Ұ����ʱ��(��)

// ȫ�ֱ���
int     g_BlockSize;                        // ͼ��ߴ�
int     g_GameMap[GAME_HEIGHT][GAME_WIDTH]; // ��ͼ���飨�Կ�Ϊ��λ��
POINT   g_EndPos;                           // �յ�λ�ã��������꣩
POINT   g_PlayerPos;                        // ����ڵ�ͼ�ϵ�λ�ã��������꣩
POINT   g_CameraPos;                        // ������ڵ�ͼ�ϵ�λ�ã����Ͻǣ��������꣩
IMAGE   g_MapImage;                         // ��ͼͼƬ
double  g_ViewArray;                        // ��ǰ��Ұ��Χ����ͼ��Ϊ��λ��
UINT    g_BeginTime;                        // ��Ϸ��ʼʱ��ʱ��
UINT    g_LastFillTime;                     // ���һ�μ��͵�ʱ��

// �����Զ�Ѱ·���ȫ�ֱ���
bool    g_autoMode = false;                 // �Ƿ����Զ�Ѱ·ģʽ����ʼΪ false
vector<POINT> g_path;                       // �Զ�Ѱ·�������·����·����ÿ����Ϊ��������

// �����б�
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

// �������Զ�Ѱ·
vector<POINT> findPath();   // ʹ�� BFS �㷨���������ϼ���ӵ�ǰ��Ԫ���յ�����·��

void initGame()
{
    g_BlockSize = 32;           // ��ʼͼ���СΪ 32 ����
    srand(GetTickCount());      // ��ʼ�������������

    // ��ʼ����ͼ�����������С�������Ϊ������������Ϊǽ��
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

    // ��������Թ���ʹ��������ȱ����㷨��
    stack<POINT> stepStack;
    vector<POINT>  stepPoint;
    POINT nowPoint;
    stepStack.push({ 1,1 });
    nowPoint = { 1,1 };
    g_GameMap[1][1] = 0xFFFF;   // ������
    while (!stepStack.empty())
    {
        // ��ȡ�ĸ���������ڵ㣨���Ϊ2��
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

    // ������
    for (int i = 0; i < GAME_HEIGHT; i++)
    {
        for (int j = 0; j < GAME_WIDTH; j++)
        {
            if (g_GameMap[i][j] == 0xFFFF)
                g_GameMap[i][j] = 0;
        }
    }

    // ��������͵ƣ�����վ��λ��
    for (int i = 0; i < FILLNUM; i++)
    {
        POINT fillPoint = { rand() % GAME_WIDTH, rand() % GAME_HEIGHT };
        while (g_GameMap[fillPoint.y][fillPoint.x] != GROUND)
            fillPoint = { rand() % GAME_WIDTH, rand() % GAME_HEIGHT };
        g_GameMap[fillPoint.y][fillPoint.x] = FILLSTATE;
    }

    // �����յ�
    g_GameMap[GAME_HEIGHT - 2][GAME_WIDTH - 2] = ENDPOS;
    g_EndPos = { GAME_WIDTH - 2, GAME_HEIGHT - 2 };

    g_ViewArray = MAXVIEW;
    g_BeginTime = GetTickCount();
    g_LastFillTime = GetTickCount();
    rePaintMap();

    // ��ʼ�����λ�ã���ʼ�� (1,1) ��������ĸ�����
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

    // ������Ϸʱ��
    TCHAR timeStr[256];
    int loseTime = GetTickCount() - g_BeginTime;
    _stprintf_s(timeStr, _T("��Ϸʱ��:%02d:%02d"), loseTime / 1000 / 60, loseTime / 1000 % 60);
    settextcolor(RGB(140, 140, 140));
    outtextxy((WIN_WIDTH - textwidth(timeStr)) / 2, 3, timeStr);

    FlushBatchDraw();
}

bool upDate()
{
    // ��� P ���������л��Զ�Ѱ·ģʽ��������رգ�
    // ʹ�� GetAsyncKeyState ��ⰴ��������ʱ���ء�
    if (GetAsyncKeyState('P') & 1)
    {
        if (g_autoMode)
        {
            // �ر��Զ�Ѱ·ģʽ
            g_autoMode = false;
            g_path.clear();
        }
        else
        {
            // �����Զ�Ѱ·ģʽ��������ֶ����룬�������
            g_path = findPath();
            if (!g_path.empty())
                g_autoMode = true;
        }
    }

    // ��������Զ�Ѱ·ģʽ��һ���з�������ֶ��������˳��Զ�Ѱ·ģʽ
    if ((GetKeyState(VK_UP) & 0x8000) || (GetKeyState(VK_DOWN) & 0x8000) ||
        (GetKeyState(VK_LEFT) & 0x8000) || (GetKeyState(VK_RIGHT) & 0x8000))
    {
        if (g_autoMode)
        {
            g_autoMode = false;
            g_path.clear();
        }
    }

    // ����Զ�Ѱ·ģʽ���ã�����ݼ������·���Զ��ƶ����
    if (g_autoMode)
    {
        if (!g_path.empty())
        {
            // Ŀ������Ԫ��·����һ�㣩��������������
            POINT target;
            target.x = g_path.front().x * g_BlockSize + g_BlockSize / 2;
            target.y = g_path.front().y * g_BlockSize + g_BlockSize / 2;

            // ���㵱ǰλ����Ŀ��λ�õľ��뼰����
            double dx = target.x - g_PlayerPos.x;
            double dy = target.y - g_PlayerPos.y;
            double dist = sqrt(dx * dx + dy * dy);
            int speed = 2;  // �Զ��ƶ��ٶ����ֶ��ƶ��ٶȱ���һ��

            if (dist <= speed)
            {
                // �ﵽĿ�굥Ԫ��ֱ����ΪĿ�꣬�����õ�
                g_PlayerPos = target;
                g_path.erase(g_path.begin());
            }
            else
            {
                // ���ٶȷ����ƶ�
                g_PlayerPos.x += int(speed * dx / dist);
                g_PlayerPos.y += int(speed * dy / dist);
            }
        }
        else
        {
            // ·��������Զ�ģʽ����
            g_autoMode = false;
        }
    }
    else
    {
        // ��������Զ�ģʽ�������ֶ�����
        POINT nextPos = g_PlayerPos;
        if (GetKeyState(VK_UP) & 0x8000)    nextPos.y -= 2;
        if (GetKeyState(VK_DOWN) & 0x8000)  nextPos.y += 2;
        if (GetKeyState(VK_LEFT) & 0x8000)  nextPos.x -= 2;
        if (GetKeyState(VK_RIGHT) & 0x8000) nextPos.x += 2;

        // �ж��Ƿ�����ƶ�
        if (!canMove(nextPos))
        {
            if (canMove({ g_PlayerPos.x, nextPos.y }))
                nextPos = { g_PlayerPos.x, nextPos.y };
            else if (canMove({ nextPos.x, g_PlayerPos.y }))
                nextPos = { nextPos.x, g_PlayerPos.y };
            else
                nextPos = g_PlayerPos;
        }
        // ����ߵ��͵Ƹ��ӣ�����ʱ��
        int cellX = nextPos.x / g_BlockSize;
        int cellY = nextPos.y / g_BlockSize;
        if (g_GameMap[cellY][cellX] == FILLSTATE)
            g_LastFillTime = GetTickCount();
        // ����ߵ��յ���ʤ��
        else if (g_GameMap[cellY][cellX] == ENDPOS)
        {
            outtextxy(WIN_WIDTH / 2 - 40, WIN_HEIGHT / 2 - 12, _T("��ϲ���أ�"));
            FlushBatchDraw();
            Sleep(1000);
            return false;
        }
        g_PlayerPos = nextPos;
    }

    computeCameraPos();

    // ����ʱ��������Ұ
    static unsigned int lastTime = GetTickCount();
    int loseTime = GetTickCount() - g_LastFillTime;
    g_ViewArray = MAXVIEW - loseTime / 1000.0 / DARKTIME;
    if (g_ViewArray < MINVIEW) g_ViewArray = MINVIEW;

    // ��������������
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

// ������ʹ�� BFS ��������Ѱ�Ҵ���ҵ�ǰλ�õ��յ�����·��
vector<POINT> findPath()
{
    vector<POINT> pathResult;
    // ����ҵ�ǰ���ڵĵ�ͼ��Ԫ������Ϊ���
    int startX = g_PlayerPos.x / g_BlockSize;
    int startY = g_PlayerPos.y / g_BlockSize;
    int endX = g_EndPos.x;
    int endY = g_EndPos.y;

    // ���ڱ�Ƿ���״̬
    bool visited[GAME_HEIGHT][GAME_WIDTH] = { false };
    // ��¼���ڵ㣬���ڻ���·��
    POINT parent[GAME_HEIGHT][GAME_WIDTH];
    for (int i = 0; i < GAME_HEIGHT; i++)
        for (int j = 0; j < GAME_WIDTH; j++)
            parent[i][j] = { -1, -1 };

    queue<POINT> q;
    q.push({ startX, startY });
    visited[startY][startX] = true;
    bool found = false;

    // �ĸ������ϡ��¡�����
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
            // �жϱ߽���Ƿ�Ϊǽ��ǽΪ����������
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
        return pathResult; // �޷��ҵ�·��

    // �������·�������յ㵽���
    vector<POINT> revPath;
    POINT cur = { endX, endY };
    while (!(cur.x == startX && cur.y == startY))
    {
        revPath.push_back(cur);
        cur = parent[cur.y][cur.x];
    }
    revPath.push_back({ startX, startY });
    // ��·���������У�����㵽�յ㣩
    for (int i = revPath.size() - 1; i >= 0; i--)
        pathResult.push_back(revPath[i]);

    return pathResult;
}
