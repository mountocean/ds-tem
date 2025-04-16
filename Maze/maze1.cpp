#include <graphics.h>
#include <stack>
#include <vector>
#include <queue>
#include <cmath>
using std::stack;
using std::vector;
using std::queue;

// ��Ϸ��Ϣ
#define WIN_WIDTH   800         // ���ڿ��(����)
#define WIN_HEIGHT  600         // ���ڸ߶�(����)
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
POINT   g_PlayerPos;                        // ���1�ڵ�ͼ�ϵ�λ�ã��������꣩
POINT   g_CameraPos;                        // ������ڵ�ͼ�ϵ�λ�ã����Ͻǣ��������꣩
IMAGE   g_MapImage;                         // ��ͼͼƬ
double  g_ViewArray;                        // ��ǰ��Ұ��Χ����ͼ��Ϊ��λ��
UINT    g_BeginTime;                        // ��Ϸ��ʼʱ��ʱ��
UINT    g_LastFillTime;                     // ���һ�μ��͵�ʱ��

// �Զ�Ѱ·��أ��������1��Ч��
bool    g_autoMode = false;                 // ���1�Ƿ����Զ�Ѱ·ģʽ����ʼΪ false
vector<POINT> g_path;                       // �Զ�Ѱ·�������·�������1����·���е�ÿ����Ϊ��������

// ������˫��ģʽ���
bool    g_twoPlayerMode = false;            // �Ƿ���˫��ģʽ����ʼΪ false
POINT   g_Player2Pos;                       // ���2�ڵ�ͼ�ϵ�λ�ã��������꣩

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
void drawPlayer();      // �������1
void drawPlayer2();     // �������������2
void drawView();

// �Զ�Ѱ·�����1��
vector<POINT> findPath();   // ʹ�� BFS �㷨���������ϼ�������1��ǰλ�õ��յ�����·��

int main()
{
    initGame();

    while (1)
    {
        if (!upDate()) break;   // ��������
        draw();                 // ���ƻ���
        absDelay(16);           // �ӳ� 16 ���룬����֡��Լ 60 FPS
    }

    endGame();
    return 0;
}

void initGame()
{
    g_BlockSize = 32;           // ��ʼͼ���СΪ 32 ����
    srand(GetTickCount());      // ��ʼ�������������

    // ��ʼ����ͼ��������������Ϊ��������������Ϊǽ��
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
    stepStack.push({ 1, 1 });
    nowPoint = { 1, 1 };
    g_GameMap[1][1] = 0xFFFF;   // ������
    while (!stepStack.empty())
    {
        // ��ȡ�ĸ��������ڵ㣨���Ϊ2��
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

    // �����յ�λ��
    g_GameMap[GAME_HEIGHT - 2][GAME_WIDTH - 2] = ENDPOS;
    g_EndPos = { GAME_WIDTH - 2, GAME_HEIGHT - 2 };

    g_ViewArray = MAXVIEW;
    g_BeginTime = GetTickCount();
    g_LastFillTime = GetTickCount();
    rePaintMap();

    // ��ʼ�����1λ�ã����������������ģ�
    g_PlayerPos = { g_BlockSize * 3 / 2, g_BlockSize * 3 / 2 };
    // ��ʼ�����2λ�ã�˫��ģʽ�´���ѡһ���ߵ㣬���ڵ�һ���Ҳࣻ����ȡ����( GAME_WIDTH-2, 1 )��
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
    // ��� O ���л�˫��ģʽ�����أ����� O �л���ϵͳ�����´θ���ʱ�����ر�˫��ģʽ
    if (GetAsyncKeyState('O') & 1)
    {
        g_twoPlayerMode = !g_twoPlayerMode;
        // �翪��˫��ģʽ���������������2�ĳ�ʼλ�ã��˴�ѡȡ���� (GAME_WIDTH-2, 1) �����ģ�
        if (g_twoPlayerMode)
            g_Player2Pos = g_PlayerPos;
    }

    // ���1���Զ�Ѱ·���أ�P ��������ԭ���淨��
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

    // ������1�����Զ�Ѱ·״̬�Ҽ�⵽��������£���ȡ���Զ�Ѱ·
    if ((GetKeyState(VK_UP) & 0x8000) || (GetKeyState(VK_DOWN) & 0x8000) ||
        (GetKeyState(VK_LEFT) & 0x8000) || (GetKeyState(VK_RIGHT) & 0x8000))
    {
        if (g_autoMode)
        {
            g_autoMode = false;
            g_path.clear();
        }
    }

    // �������1��λ��
    if (g_autoMode)
    {
        if (!g_path.empty())
        {
            // Ŀ������Ԫ��·����һ�㣩��������������
            POINT target;
            target.x = g_path.front().x * g_BlockSize + g_BlockSize / 2;
            target.y = g_path.front().y * g_BlockSize + g_BlockSize / 2;

            // ���㵱ǰ��Ŀ��ľ��뼰����
            double dx = target.x - g_PlayerPos.x;
            double dy = target.y - g_PlayerPos.y;
            double dist = sqrt(dx * dx + dy * dy);
            int speed = 2;  // �ƶ��ٶ�

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
        // ���1�ֶ����ƣ��������
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
        // ����ߵ��͵Ƹ��ӣ�����ʱ��
        int cellX = nextPos.x / g_BlockSize;
        int cellY = nextPos.y / g_BlockSize;
        if (g_GameMap[cellY][cellX] == FILLSTATE)
            g_LastFillTime = GetTickCount();
        // ���ʤ�����������1��
        else if (g_GameMap[cellY][cellX] == ENDPOS)
        {
            outtextxy(WIN_WIDTH / 2 - 40, WIN_HEIGHT / 2 - 12, _T("���1ʤ����"));
            FlushBatchDraw();
            Sleep(1000);
            return false;
        }
        g_PlayerPos = nextPos;
    }

    // ���2���£�����˫��ģʽ�´���ʹ�� WASD �����ƣ�
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
        // ͬ������͵Ʋ���������ʱ�䣩
        int cellX2 = nextPos2.x / g_BlockSize;
        int cellY2 = nextPos2.y / g_BlockSize;
        if (g_GameMap[cellY2][cellX2] == FILLSTATE)
            g_LastFillTime = GetTickCount();
        // ���ʤ�����������2��
        else if (g_GameMap[cellY2][cellX2] == ENDPOS)
        {
            outtextxy(WIN_WIDTH / 2 - 40, WIN_HEIGHT / 2 - 12, _T("���2ʤ����"));
            FlushBatchDraw();
            Sleep(1000);
            return false;
        }
        g_Player2Pos = nextPos2;
    }

    // ���������λ�ã�˫��ģʽ��ȡ��������ĵ�ƽ��ֵ�����������1Ϊ���ģ�
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
            // �������2Ҳ����ӳ��λ�ã�˫��ģʽ�£�
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
    // ˫��ģʽ��������������ƽ��ֵ��Ϊ���������
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
    setfillcolor(RGB(252, 213, 11)); // ���1��ɫ����ɫ��
    solidcircle(g_PlayerPos.x - g_CameraPos.x, g_PlayerPos.y - g_CameraPos.y, 3);
}

void drawPlayer2()
{
    setfillcolor(RGB(0, 255, 255));  // ���2��ɫ����ɫ��
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

// ʹ�� BFS �㷨��������Ѱ�Ҵ����1��ǰλ�õ��յ�����·��
vector<POINT> findPath()
{
    vector<POINT> pathResult;
    // ���1��ǰλ�ã��������꣩��Ϊ���
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
