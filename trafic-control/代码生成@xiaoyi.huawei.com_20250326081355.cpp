#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <queue>
#include <random>
#include <unordered_map>

using namespace std;

const int LANE_COUNT = 3;  // 每方向3个车道
const int ROAD_LENGTH = 50; // 道路长度
const int STOP_PROB = 5;   // 停止概率5%

class Vehicle {
private:
    int id;
    int x;
    int lane;
    bool isStopped;
    chrono::steady_clock::time_point stopEndTime;
    static int nextID;

public:
    Vehicle(int startX, int startLane) :
        id(nextID++), x(startX), lane(startLane),
        isStopped(false) {
    }

    void moveForward() {
        if (!isStopped) x = (x + 1) % ROAD_LENGTH;
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
    int getID() const { return id; }
    bool stopped() const { return isStopped; }
};

int Vehicle::nextID = 1;

class LaneSystem {
private:
    vector<vector<bool>> lanes;  // 车道占用状态 [车道][位置]
    queue<Vehicle> northbound;   // 北向车辆队列（右到左）
    queue<Vehicle> southbound;   // 南向车辆队列（左到右）

public:
    LaneSystem() : lanes(LANE_COUNT * 2, vector<bool>(ROAD_LENGTH, false)) {}

    void addVehicle(bool isNorthbound) {
        if (isNorthbound) {
            northbound.emplace(0, rand() % LANE_COUNT);
        }
        else {
            southbound.emplace(ROAD_LENGTH - 1, LANE_COUNT + rand() % LANE_COUNT);
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

    void updateVehiclePosition(Vehicle& v, queue<Vehicle>& q) {
        // 检查变道可能性
        if (!v.stopped() && rand() % 10 < 3) { // 30%变道概率
            attemptLaneChange(v);
        }

        // 检查目标位置是否可用
        if (!lanes[v.getLane()][v.getX()]) {
            lanes[v.getLane()][v.getX()] = true;
            q.push(v);
        }
    }

    void attemptLaneChange(Vehicle& v) {
        int currentLane = v.getLane();
        int direction = (currentLane < LANE_COUNT) ? 1 : -1;
        vector<int> possibleLanes;

        // 检查相邻车道是否在允许范围内
        if (currentLane > 0 && (currentLane != LANE_COUNT))
            possibleLanes.push_back(currentLane - 1);
        if (currentLane < LANE_COUNT * 2 - 1 && (currentLane != LANE_COUNT - 1))
            possibleLanes.push_back(currentLane + 1);

        for (int newLane : possibleLanes) {
            if (!lanes[newLane][v.getX()]) {
                v = Vehicle(v.getX(), newLane);
                return;
            }
        }
    }
    
    void printRoad() {
        system("cls"); // Windows清屏，Linux/Mac用system("clear")
        for (int lane = 0; lane < LANE_COUNT * 2; ++lane) {
            for (int x = 0; x < ROAD_LENGTH; ++x) {
                if (lanes[lane][x]) cout << "■";
                else if (x % 5 == 0) cout << "—";
                else cout << " ";
            }
            cout << endl;
        }
    }
};

int main() {
    LaneSystem system;
    random_device rd;
    mt19937 gen(rd());
    poisson_distribution<> vehicleGen(3); // 车辆生成间隔

    int timeCounter = 0;
    while (true) {
        if (timeCounter % 10 == 0) {
            system.addVehicle(rand() % 2); // 随机生成方向
        }

        system.updateLanes();
        system.printRoad();

        this_thread::sleep_for(chrono::milliseconds(200));
        timeCounter++;
    }
}
