#include <iostream>
#include <queue>
#include <vector>
#include <thread>
#include <chrono>
#include <mutex>
#include <random>
#include <conio.h>
#include <windows.h>
#include <sstream> // �����������ַ���ת��

using namespace std;

// �Զ���to_string���������ݾɰ汾C++������
template <typename T>
string custom_to_string(T value) {
    ostringstream os;
    os << value;
    return os.str();
}

// ����̨��������
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

// ���巽��
enum Direction {
    NORTH = 0,
    EAST = 1,
    SOUTH = 2,
    WEST = 3
};

// ���彻ͨ��״̬
enum LightStatus {
    RED,
    GREEN
};

// ���峵��
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

// ��ͨ����
class TrafficLight {
private:
    LightStatus status[4];
    int timeCounter[4];
    const int greenDuration = 60;
    const int redDuration = 180;
    mutex mtx;

public:
    TrafficLight() {
        // ��ʼ����ͨ��״̬������Ϊ�����̵�ʱ��
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
        // ��������̵�
        gotoxy(40, 5);
        cout << (status[NORTH] == GREEN ? "G" : "R");

        // ��������̵�
        gotoxy(55, 15);
        cout << (status[EAST] == GREEN ? "G" : "R");

        // �Ϸ�����̵�
        gotoxy(40, 25);
        cout << (status[SOUTH] == GREEN ? "G" : "R");

        // ��������̵�
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

// ʮ��·����
class Intersection {
private:
    TrafficLight trafficLight;
    vector<queue<Car*>> carQueues;
    vector<Car*> movingCars;
    mutex queueMtx;
    mutex movingCarsMtx;
    bool running;

    // ����·�ڱ߽�
    const int northBoundary = 7;
    const int southBoundary = 23;
    const int westBoundary = 32;
    const int eastBoundary = 48;

    // ���峵��λ��
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
        // ����
        system("cls");

        // ��ˮƽ����
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

        // ����ֱ����
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

        // ������·��
        for (int y = northBoundary; y <= southBoundary; y++) {
            for (int x = westBoundary; x <= eastBoundary; x++) {
                gotoxy(x, y);
                cout << " ";
            }
        }

        // ��ʾ�����ǩ
        gotoxy(40, 2);
        cout << "��";
        gotoxy(65, 15);
        cout << "��";
        gotoxy(40, 28);
        cout << "��";
        gotoxy(15, 15);
        cout << "��";

        // ��ʾ��ʱ����ǩ
        gotoxy(45, 5);
        cout << "������:";
        gotoxy(55, 10);
        cout << "������:";
        gotoxy(45, 25);
        cout << "�Ϸ���:";
        gotoxy(25, 10);
        cout << "������:";
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
        // ����Ƿ��Ѿ�ͨ��·��
        if (car->crossedIntersection) {
            return true;
        }

        // ����Ƿ񵽴�·���Һ��
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

        // ���ǰ���Ƿ��г�
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

        // ���ͨ��·�ڣ����Ϊ��ͨ��
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

                // ���ƶ����еĵ�һ����
                car->draw();

                // ��������ƶ��������Ӷ����Ƶ��������
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

            // ����Ƿ񵽴��յ�
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

            // ��������ƶ������ƶ�����
            if (canMove(car)) {
                car->move();
            }
        }

        // �Ӻ���ǰɾ�������յ�ĳ���
        for (int i = (int)carsToRemove.size() - 1; i >= 0; i--) {
            delete movingCars[carsToRemove[i]];
            movingCars.erase(movingCars.begin() + carsToRemove[i]);
        }
    }

    void updateTrafficLights() {
        trafficLight.update();
        trafficLight.draw();

        // ��ʾ������ʣ��ʱ��
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

            // ��������³�
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
    cout << "proximaʮ��·��ģ��ϵͳ" << endl;
    Intersection intersection;
    intersection.run();
    system("cls");
    cout << "ģ�����" << endl;
}