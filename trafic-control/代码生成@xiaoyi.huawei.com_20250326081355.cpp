#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <queue>
#include <random>
#include <unordered_map>

using namespace std;

const int LANE_COUNT = 3;  // ÿ����3������
const int ROAD_LENGTH = 50; // ��·����
const int STOP_PROB = 5;   // ֹͣ����5%

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
    vector<vector<bool>> lanes;  // ����ռ��״̬ [����][λ��]
    queue<Vehicle> northbound;   // ���������У��ҵ���
    queue<Vehicle> southbound;   // ���������У����ң�

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
        // �����״̬
        for (auto& lane : lanes) fill(lane.begin(), lane.end(), false);

        // ���±�����
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

        // ����������
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
        // �����������
        if (!v.stopped() && rand() % 10 < 3) { // 30%�������
            attemptLaneChange(v);
        }

        // ���Ŀ��λ���Ƿ����
        if (!lanes[v.getLane()][v.getX()]) {
            lanes[v.getLane()][v.getX()] = true;
            q.push(v);
        }
    }

    void attemptLaneChange(Vehicle& v) {
        int currentLane = v.getLane();
        int direction = (currentLane < LANE_COUNT) ? 1 : -1;
        vector<int> possibleLanes;

        // ������ڳ����Ƿ�������Χ��
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
        system("cls"); // Windows������Linux/Mac��system("clear")
        for (int lane = 0; lane < LANE_COUNT * 2; ++lane) {
            for (int x = 0; x < ROAD_LENGTH; ++x) {
                if (lanes[lane][x]) cout << "��";
                else if (x % 5 == 0) cout << "��";
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
    poisson_distribution<> vehicleGen(3); // �������ɼ��

    int timeCounter = 0;
    while (true) {
        if (timeCounter % 10 == 0) {
            system.addVehicle(rand() % 2); // ������ɷ���
        }

        system.updateLanes();
        system.printRoad();

        this_thread::sleep_for(chrono::milliseconds(200));
        timeCounter++;
    }
}
