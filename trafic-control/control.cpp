#include <iostream>
#include <queue>
#include <vector>
#include <thread>
#include <chrono>
#include <mutex>
#include <random>
#include <conio.h>
#include <windows.h>
#include <sstream> // 添加这个用于字符串转换

using namespace std;

// 自定义to_string函数，兼容旧版本C++编译器
template <typename T>
string custom_to_string(T value) {
    ostringstream os;
    os << value;
    return os.str();
}

// 控制台操作函数
void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void hideCursor() {
    CONSOLE_CURSOR_INFO cursor_info = { 1, 0 };
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
}

// 定义方向
enum Direction {
    NORTH = 0,
    EAST = 1,
    SOUTH = 2,
    WEST = 3
};

// 定义交通灯状态
enum LightStatus {
    RED,
    GREEN
};

// 定义车辆
struct Car {
    int x, y;
    Direction dir;
    char symbol = '#';
    bool crossedIntersection = false;

    Car(int startX, int startY, Direction startDir) : x(startX), y(startY), dir(startDir) {}

    void draw() {
        gotoxy(x, y);
        cout << symbol;
    }

    void clear() {
        gotoxy(x, y);
        cout << " ";
    }

    void move() {
        clear();
        switch (dir) {
        case NORTH:
            y--;
            break;
        case EAST:
            x++;
            break;
        case SOUTH:
            y++;
            break;
        case WEST:
            x--;
            break;
        }
        draw();
    }
};

// 交通灯类
class TrafficLight {
private:
    LightStatus status[4];
    int timeCounter[4];
    const int greenDuration = 60;
    const int redDuration = 180;
    mutex mtx;

public:
    TrafficLight() {
        // 初始化交通灯状态，设置为错开的绿灯时间
        status[NORTH] = GREEN;
        status[EAST] = RED;
        status[SOUTH] = RED;
        status[WEST] = RED;

        timeCounter[NORTH] = greenDuration;
        timeCounter[EAST] = redDuration - greenDuration * 2;
        timeCounter[SOUTH] = redDuration - greenDuration * 1;
        timeCounter[WEST] = redDuration - greenDuration * 0;
    }

    void update() {
        lock_guard<mutex> lock(mtx);
        for (int i = 0; i < 4; i++) {
            timeCounter[i]--;
            if (timeCounter[i] <= 0) {
                if (status[i] == RED) {
                    status[i] = GREEN;
                    timeCounter[i] = greenDuration;
                }
                else {
                    status[i] = RED;
                    timeCounter[i] = redDuration;
                }
            }
        }
    }

    void draw() {
        // 北方向红绿灯
        gotoxy(40, 5);
        cout << (status[NORTH] == GREEN ? "G" : "R");

        // 东方向红绿灯
        gotoxy(55, 15);
        cout << (status[EAST] == GREEN ? "G" : "R");

        // 南方向红绿灯
        gotoxy(40, 25);
        cout << (status[SOUTH] == GREEN ? "G" : "R");

        // 西方向红绿灯
        gotoxy(25, 15);
        cout << (status[WEST] == GREEN ? "G" : "R");
    }

    bool isGreen(Direction dir) {
        lock_guard<mutex> lock(mtx);
        return status[dir] == GREEN;
    }

    string getTimeRemaining(Direction dir) {
        lock_guard<mutex> lock(mtx);
        string result = custom_to_string(timeCounter[dir]) + "s";
        if (status[dir] == GREEN) {
            result += " G";
        }
        else {
            result += " R";
        }
        return result;
    }
};

// 十字路口类
class Intersection {
private:
    TrafficLight trafficLight;
    vector<queue<Car*>> carQueues;
    vector<Car*> movingCars;
    mutex queueMtx;
    mutex movingCarsMtx;
    bool running;

    // 定义路口边界
    const int northBoundary = 7;
    const int southBoundary = 23;
    const int westBoundary = 32;
    const int eastBoundary = 48;

    // 定义车道位置
    const int northLaneX = 40;
    const int southLaneX = 38;
    const int eastLaneY = 15;
    const int westLaneY = 13;

public:
    Intersection() : running(true) {
        carQueues.resize(4);
    }

    ~Intersection() {
        for (auto& queue : carQueues) {
            while (!queue.empty()) {
                delete queue.front();
                queue.pop();
            }
        }

        for (size_t i = 0; i < movingCars.size(); i++) {
            delete movingCars[i];
        }
        movingCars.clear();
    }

    void drawRoads() {
        // 清屏
        system("cls");

        // 画水平车道
        for (int x = 10; x < 70; x++) {
            if (x < westBoundary || x > eastBoundary) {
                gotoxy(x, 12);
                cout << "-";
                gotoxy(x, 14);
                cout << "-";
                gotoxy(x, 16);
                cout << "-";
            }
        }

        // 画垂直车道
        for (int y = 1; y < 30; y++) {
            if (y < northBoundary || y > southBoundary) {
                gotoxy(37, y);
                cout << "|";
                gotoxy(39, y);
                cout << "|";
                gotoxy(41, y);
                cout << "|";
            }
        }

        // 画交叉路口
        for (int y = northBoundary; y <= southBoundary; y++) {
            for (int x = westBoundary; x <= eastBoundary; x++) {
                gotoxy(x, y);
                cout << " ";
            }
        }

        // 显示方向标签
        gotoxy(40, 2);
        cout << "北";
        gotoxy(65, 15);
        cout << "东";
        gotoxy(40, 28);
        cout << "南";
        gotoxy(15, 15);
        cout << "西";

        // 显示计时器标签
        gotoxy(45, 5);
        cout << "北方向:";
        gotoxy(55, 10);
        cout << "东方向:";
        gotoxy(45, 25);
        cout << "南方向:";
        gotoxy(25, 10);
        cout << "西方向:";
    }

    void addCar(Direction dir) {
        Car* car = nullptr;

        switch (dir) {
        case NORTH:
            car = new Car(northLaneX, 29, NORTH);
            break;
        case EAST:
            car = new Car(10, eastLaneY, EAST);
            break;
        case SOUTH:
            car = new Car(southLaneX, 1, SOUTH);
            break;
        case WEST:
            car = new Car(69, westLaneY, WEST);
            break;
        }

        if (car) {
            lock_guard<mutex> lock(queueMtx);
            carQueues[dir].push(car);
        }
    }

    bool canMove(Car* car) {
        // 检查是否已经通过路口
        if (car->crossedIntersection) {
            return true;
        }

        // 检查是否到达路口且红灯
        bool atIntersection = false;
        switch (car->dir) {
        case NORTH:
            atIntersection = car->y == southBoundary;
            break;
        case EAST:
            atIntersection = car->x == westBoundary;
            break;
        case SOUTH:
            atIntersection = car->y == northBoundary;
            break;
        case WEST:
            atIntersection = car->x == eastBoundary;
            break;
        }

        if (atIntersection && !trafficLight.isGreen(car->dir)) {
            return false;
        }

        // 检查前方是否有车
        for (size_t i = 0; i < movingCars.size(); i++) {
            Car* otherCar = movingCars[i];
            if (otherCar != car) {
                switch (car->dir) {
                case NORTH:
                    if (otherCar->x == car->x && otherCar->y == car->y - 1) {
                        return false;
                    }
                    break;
                case EAST:
                    if (otherCar->y == car->y && otherCar->x == car->x + 1) {
                        return false;
                    }
                    break;
                case SOUTH:
                    if (otherCar->x == car->x && otherCar->y == car->y + 1) {
                        return false;
                    }
                    break;
                case WEST:
                    if (otherCar->y == car->y && otherCar->x == car->x - 1) {
                        return false;
                    }
                    break;
                }
            }
        }

        // 如果通过路口，标记为已通过
        if (atIntersection && trafficLight.isGreen(car->dir)) {
            car->crossedIntersection = true;
        }

        return true;
    }


    void updateCarQueues() {
        lock_guard<mutex> lock(queueMtx);
        for (int dir = 0; dir < 4; dir++) {
            if (!carQueues[dir].empty()) {
                Car* car = carQueues[dir].front();

                // 绘制队列中的第一辆车
                car->draw();

                // 如果可以移动，将车从队列移到活动车辆中
                {
                    lock_guard<mutex> movingLock(movingCarsMtx);
                    if (canMove(car)) {
                        carQueues[dir].pop();
                        movingCars.push_back(car);
                    }
                }
            }
        }
    }


    void updateMovingCars() {
        lock_guard<mutex> lock(movingCarsMtx);
        vector<int> carsToRemove;

        for (size_t i = 0; i < movingCars.size(); i++) {
            Car* car = movingCars[i];

            // 检查是否到达终点
            bool reachedEnd = false;
            switch (car->dir) {
            case NORTH:
                reachedEnd = car->y <= 1;
                break;
            case EAST:
                reachedEnd = car->x >= 69;
                break;
            case SOUTH:
                reachedEnd = car->y >= 29;
                break;
            case WEST:
                reachedEnd = car->x <= 10;
                break;
            }

            if (reachedEnd) {
                car->clear();
                carsToRemove.push_back(i);
                continue;
            }

            // 如果可以移动，则移动车辆
            if (canMove(car)) {
                car->move();
            }
        }

        // 从后向前删除到达终点的车辆
        for (int i = (int)carsToRemove.size() - 1; i >= 0; i--) {
            delete movingCars[carsToRemove[i]];
            movingCars.erase(movingCars.begin() + carsToRemove[i]);
        }
    }

    void updateTrafficLights() {
        trafficLight.update();
        trafficLight.draw();

        // 显示各方向剩余时间
        gotoxy(55, 5);
        cout << trafficLight.getTimeRemaining(NORTH) << "   ";
        gotoxy(65, 10);
        cout << trafficLight.getTimeRemaining(EAST) << "   ";
        gotoxy(55, 25);
        cout << trafficLight.getTimeRemaining(SOUTH) << "   ";
        gotoxy(35, 10);
        cout << trafficLight.getTimeRemaining(WEST) << "   ";
    }

    void run() {
        drawRoads();
        hideCursor();

        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dirDist(0, 3);
        uniform_int_distribution<> timeDist(1, 5);

        int carTimer = 0;

        while (running) {
            updateTrafficLights();
            updateCarQueues();
            updateMovingCars();

            // 随机生成新车
            carTimer++;
            if (carTimer >= timeDist(gen)) {
                Direction randomDir = static_cast<Direction>(dirDist(gen));
                addCar(randomDir);
                carTimer = 0;
            }

            this_thread::sleep_for(chrono::milliseconds(100));
        }
    }
};

void main2() {
    system("cls");
    cout << "proxima十字路口模拟系统" << endl;
    Intersection intersection;
    intersection.run();
    system("cls");
    cout << "模拟结束" << endl;
}