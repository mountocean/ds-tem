#include <iostream>
#include <cstring>
#include <time.h>
#include <cstdlib>
#include <windows.h>
#include <ctime>  // 包含计时所需的库
using namespace std;

clock_t start_time, current_time, end_time;
int a[15][15], ans[15][15], b[15][15];
bool square[10][10], lie[10][10], hang[10][10], flag[15][15];
bool check;
bool f[15];
int c[15];
int difficult;

void SetColor(unsigned short ForeColor = 7, unsigned short BackGroundColor = 0)
{
    HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hCon, ForeColor | BackGroundColor);
}

int ge(int x, int y)
{
    return (((x - 1) / 3) * 3 + (y - 1) / 3 + 1);
}

// -------- 插入：带计时的打印函数 --------
void print() {
    // 计算并显示已用时间
    current_time = clock();
    double elapsed = double(current_time - start_time) / CLOCKS_PER_SEC;
    SetColor(14);
    printf("Elapsed Time: %.1f s\n", elapsed);
    SetColor(15);

    // 原有网格绘制
    SetColor(15);
    printf("╔━━━┳━━━┳━━━");
    SetColor(9); printf("┳"); SetColor(15);
    printf("━━━┳━━━┳━━━");
    SetColor(9); printf("┳"); SetColor(15);
    printf("━━━┳━━━┳━━━┓\n");
    for (int i = 1; i <= 9; i++) {
        for (int j = 1; j <= 9; j++) {
            if (a[i][j] == 0) {
                if (j == 4 || j == 7) SetColor(9);
                cout << "┃   ";
                SetColor(15);
                if (j == 9) cout << "┃";
            }
            else {
                if (j == 4 || j == 7) SetColor(9);
                cout << "┃ ";
                SetColor(15);
                if (b[i][j] == 0) SetColor(4);
                cout << a[i][j] << " ";
                SetColor(15);
                if (j == 9) cout << "┃";
            }
        }
        cout << endl;
        if (i == 3 || i == 6) {
            SetColor(9);
            cout << "┣━━━╋━━━╋━━━╋━━━╋━━━╋━━━╋━━━╋━━━┫" << endl;
            SetColor(15);
            continue;
        }
        if (i != 9) {
            cout << "┣━━━╋━━━╋━━━";
            SetColor(9); cout << "╋"; SetColor(15);
            cout << "━━━╋━━━╋━━━";
            SetColor(9); cout << "╋"; SetColor(15);
            cout << "━━━╋━━━╋━━━┫" << endl;
        }
        else {
            SetColor(15);
            printf("┗━━━┻━━━┻━━━");
            SetColor(9); printf("┻"); SetColor(15);
            printf("━━━┻━━━┻━━━");
            SetColor(9); printf("┻"); SetColor(15);
            printf("━━━┻━━━┻━━━┛\n");
        }
        SetColor(15);
    }
}

void print_y() {
    // 结束时专用，先显示总时长
    end_time = clock();
    double total = double(end_time - start_time) / CLOCKS_PER_SEC;
    SetColor(10);
    printf("恭喜你，答案完全正确！总用时：%.1f 秒\n\n", total);
    SetColor(15);

    // 原有答案网格
    SetColor(15);
    printf("╔━━━┳━━━┳━━━");
    SetColor(9); printf("┳"); SetColor(15);
    printf("━━━┳━━━┳━━━");
    SetColor(9); printf("┳"); SetColor(15);
    printf("━━━┳━━━┳━━━┓\n");
    for (int i = 1; i <= 9; i++) {
        for (int j = 1; j <= 9; j++) {
            if (ans[i][j] == 0) {
                if (j == 4 || j == 7) SetColor(9);
                cout << "┃   ";
                SetColor(15);
                if (j == 9) cout << "┃";
            }
            else {
                if (j == 4 || j == 7) SetColor(9);
                cout << "┃ ";
                SetColor(15);
                if (b[i][j] == 0) SetColor(4);
                cout << ans[i][j] << " ";
                SetColor(15);
                if (j == 9) cout << "┃";
            }
        }
        cout << endl;
        if (i == 3 || i == 6) {
            SetColor(9);
            cout << "┣━━━╋━━━╋━━━╋━━━╋━━━╋━━━╋━━━╋━━━┫" << endl;
            SetColor(15);
            continue;
        }
        if (i != 9) {
            cout << "┣━━━╋━━━╋━━━";
            SetColor(9); cout << "╋"; SetColor(15);
            cout << "━━━╋━━━╋━━━";
            SetColor(9); cout << "╋"; SetColor(15);
            cout << "━━━╋━━━╋━━━┫" << endl;
        }
        else {
            SetColor(15);
            printf("┗━━━┻━━━┻━━━");
            SetColor(9); printf("┻"); SetColor(15);
            printf("━━━┻━━━┻━━━");
            SetColor(9); printf("┻"); SetColor(15);
            printf("━━━┻━━━┻━━━┛\n");
        }
        SetColor(15);
    }
}

// 以下函数（dfs, work, init, run）保持不变

int dfs(int x, int y)
{
    if (!check) return 0;
    if (x == 0 && y == 9) {
        check = false;
        return 0;
    }
    if (a[x][y] == 0)
        for (int j = 1; j <= 9; j++) {
            int i = c[j];
            if (square[ge(x, y)][i] && lie[y][i] && hang[x][i]) {
                square[ge(x, y)][i] = false;
                lie[y][i] = false;
                hang[x][i] = false;
                ans[x][y] = i;
                if (y == 1) dfs(x - 1, 9); else dfs(x, y - 1);
                square[ge(x, y)][i] = true;
                lie[y][i] = true;
                hang[x][i] = true;
                if (!check) return 0;
            }
        }
    if (!check) return 0;
    if (a[x][y] != 0) if (y == 1) dfs(x - 1, 9); else dfs(x, y - 1);
}

int work(int x) {
    if (x == 10) {
        return 0;
    }
    int i = 1;
    int y = rand() % 9 + 1;
    while (!f[y]) y = rand() % 9 + 1;
    f[y] = false;
    c[x] = y;
    work(x + 1);
}

void init() {
    srand(time(NULL));
    memset(a, 0, sizeof(a));
    memset(lie, true, sizeof(lie));
    memset(hang, true, sizeof(hang));
    memset(square, true, sizeof(square));
    memset(f, true, sizeof(f));
    check = true;
    work(1);
    memset(a, sizeof(a), 0);
    check = true;
    dfs(9, 9);
    memcpy(a, ans, sizeof(ans));
    printf("请选择难度，从 1-3\n");
    difficult = 0;
    while (difficult > 3 || difficult < 1) cin >> difficult;
}

void run() {
    int sum = 0;
    switch (difficult) {
    case 1: sum = 20; break;
    case 2: sum = 40; break;
    case 3: sum = 60; break;
    }
    int i = 1;
    while (i <= sum) {
        int x, y;
        x = rand() % 9 + 1;
        y = rand() % 9 + 1;
        while (a[x][y] == 0) {
            x = rand() % 9 + 1;
            y = rand() % 9 + 1;
        }
        a[x][y] = 0;
        i++;
    }
}

int main() {
    init();
    run();
    system("cls");
    memcpy(b, a, sizeof(a));

    start_time = clock();  // 记录开始时间

    int xx, yy, zz;
    while (true) {
        system("cls");
        print();  // 实时计时打印
        cout << "请输入 'x y z' 代表在横坐标x，纵坐标y写入数字z\n";
        cout << "输入 x y 0 可清除已填数字；输入 0 0 0 显示答案并结束\n\n";
        cin >> xx >> yy >> zz;
        if (xx == 0 && yy == 0 && zz == 0) {
            system("cls");
            print_y();
            break;
        }
        if (b[xx][yy] == 0) a[xx][yy] = zz;

        // 校验是否完成
        bool correct = true;
        for (int i = 1; i <= 9; i++)
            for (int j = 1; j <= 9; j++)
                if (a[i][j] != ans[i][j]) correct = false;
        if (correct) {
            system("cls");
            print_y();
            break;
        }
    }
    system("pause");
    return 0;
}
