#include "head.h"
using namespace std;

const int LANE_COUNT = 3;   // 每个方向3个车道
const int ROAD_LENGTH = 50; // 道路长度
const int STOP_PROB = 1;    // 停止概率5%

class Vehicle {
private:
    int id;
    int x;
    int lane;
    bool isNorthbound;
    bool isStopped;
    chrono::steady_clock::time_point stopEndTime;
    static int nextID;

public:
    Vehicle(int startX, int startLane, bool isNB) :
        id(nextID++), x(startX), lane(startLane),
        isNorthbound(isNB), isStopped(false) {
    }

    void moveForward() {
        if (!isStopped) {
            if (isNorthbound) {
                x = (x + 1) % ROAD_LENGTH;  // 北向右移
            }
            else {
                x = (x - 1 + ROAD_LENGTH) % ROAD_LENGTH;  // 南向左移
            }
        }
    }

    void maybeStop() {
        if (rand() % 100 < STOP_PROB) {
            isStopped = true;
            stopEndTime = chrono::steady_clock::now() + chrono::seconds(3);
        }
    }

    void checkStopStatus() {
        if (isStopped && chrono::steady_clock::now() > stopEndTime) {
            isStopped = false;
        }
    }

    int getX() const { return x; }
    int getLane() const { return lane; }
    bool isNB() const { return isNorthbound; }
    int getID() const { return id; }
    bool stopped() const { return isStopped; }
};

int Vehicle::nextID = 1;

class LaneSystem {
private:
    vector<vector<bool>> lanes;  // 车道占用状态 [车道][位置]
    queue<Vehicle> northbound;   // 北向车辆队列
    queue<Vehicle> southbound;   // 南向车辆队列

    void printMiddleLine() {
        for (int x = 0; x < ROAD_LENGTH*2; ++x) {
            cout << (x % 5 == 0 ? "≡" : "="); // 中央分隔线
        }
        cout << endl;
    }

    void updateVehiclePosition(Vehicle& v, queue<Vehicle>& q) {
        // 30%概率尝试变道
        if (!v.stopped() && rand() % 10 < 3) {
            attemptLaneChange(v);
        }

        if (!lanes[v.getLane()][v.getX()]) {
            lanes[v.getLane()][v.getX()] = true;
            q.push(v);
        }
    }

    void attemptLaneChange(Vehicle& v) {
        int currentLane = v.getLane();
        vector<int> possibleLanes;

        // 北向车辆变道限制
        if (v.isNB()) {
            if (currentLane > 0) possibleLanes.push_back(currentLane - 1);
            if (currentLane < LANE_COUNT - 1) possibleLanes.push_back(currentLane + 1);
        }
        // 南向车辆变道限制
        else {
            if (currentLane > LANE_COUNT) possibleLanes.push_back(currentLane - 1);
            if (currentLane < LANE_COUNT * 2 - 1) possibleLanes.push_back(currentLane + 1);
        }

        // 检查相邻车道是否可用
        for (int newLane : possibleLanes) {
            if (!lanes[newLane][v.getX()]) {
                v = Vehicle(v.getX(), newLane, v.isNB());
                return;
            }
        }
    }

public:
    LaneSystem() : lanes(LANE_COUNT * 2, vector<bool>(ROAD_LENGTH, false)) {}

    void addVehicle(bool isNorthbound) {
        if (isNorthbound) {
            northbound.emplace(0, rand() % LANE_COUNT, true);
        }
        else {
            southbound.emplace(ROAD_LENGTH - 1, LANE_COUNT + rand() % LANE_COUNT, false);
        }
    }

    void updateLanes() {
        // 清除旧状态
        for (auto& lane : lanes) fill(lane.begin(), lane.end(), false);

        // 更新北向车辆
        queue<Vehicle> tempNB;
        while (!northbound.empty()) {
            auto v = northbound.front();
            v.checkStopStatus();
            if (!v.stopped()) {
                v.maybeStop();
                v.moveForward();
            }
            updateVehiclePosition(v, tempNB);
            northbound.pop();
        }
        northbound = tempNB;

        // 更新南向车辆
        queue<Vehicle> tempSB;
        while (!southbound.empty()) {
            auto v = southbound.front();
            v.checkStopStatus();
            if (!v.stopped()) {
                v.maybeStop();
                v.moveForward();
            }
            updateVehiclePosition(v, tempSB);
            southbound.pop();
        }
        southbound = tempSB;
    }

    void printRoad() {
        system("cls"); // Windows清屏，Linux/Mac用system("clear")

        // 打印北向车道（从右向左移动）
        for (int lane = 0; lane < LANE_COUNT; ++lane) {
            for (int x = 0; x < ROAD_LENGTH; ++x) {
                if (lanes[lane][x]) cout << "→■";
                else cout << (x % 5 == 0 ? "—" : "  ");
            }
            cout << endl;
        }

        printMiddleLine();

        // 打印南向车道（从左向右移动）
        for (int lane = LANE_COUNT; lane < LANE_COUNT * 2; ++lane) {
            for (int x = 0; x < ROAD_LENGTH; ++x) {
                if (lanes[lane][x]) cout << "←■";
                else cout << (x % 5 == 0 ? "—" : "  ");
            }
            cout << endl;
        }
    }
};

void main1() {
    srand(time(nullptr));
    LaneSystem system;
    int timeCounter = 0;

    while (true) {
        if (timeCounter % 8 == 0) {  // 调整车辆生成频率
            system.addVehicle(rand() % 2);
        }

        system.updateLanes();
        system.printRoad();

        this_thread::sleep_for(chrono::milliseconds(200));
        timeCounter++;
    }
}

