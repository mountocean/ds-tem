#include "head.h"
using namespace std;

const int LANE_COUNT = 3;   // ÿ������3������
const int ROAD_LENGTH = 50; // ��·����
const int STOP_PROB = 1;    // ֹͣ����5%

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
                x = (x + 1) % ROAD_LENGTH;  // ��������
            }
            else {
                x = (x - 1 + ROAD_LENGTH) % ROAD_LENGTH;  // ��������
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
    vector<vector<bool>> lanes;  // ����ռ��״̬ [����][λ��]
    queue<Vehicle> northbound;   // ����������
    queue<Vehicle> southbound;   // ����������

    void printMiddleLine() {
        for (int x = 0; x < ROAD_LENGTH*2; ++x) {
            cout << (x % 5 == 0 ? "��" : "="); // ����ָ���
        }
        cout << endl;
    }

    void updateVehiclePosition(Vehicle& v, queue<Vehicle>& q) {
        // 30%���ʳ��Ա��
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

        // �������������
        if (v.isNB()) {
            if (currentLane > 0) possibleLanes.push_back(currentLane - 1);
            if (currentLane < LANE_COUNT - 1) possibleLanes.push_back(currentLane + 1);
        }
        // �������������
        else {
            if (currentLane > LANE_COUNT) possibleLanes.push_back(currentLane - 1);
            if (currentLane < LANE_COUNT * 2 - 1) possibleLanes.push_back(currentLane + 1);
        }

        // ������ڳ����Ƿ����
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

    void printRoad() {
        system("cls"); // Windows������Linux/Mac��system("clear")

        // ��ӡ���򳵵������������ƶ���
        for (int lane = 0; lane < LANE_COUNT; ++lane) {
            for (int x = 0; x < ROAD_LENGTH; ++x) {
                if (lanes[lane][x]) cout << "����";
                else cout << (x % 5 == 0 ? "��" : "  ");
            }
            cout << endl;
        }

        printMiddleLine();

        // ��ӡ���򳵵������������ƶ���
        for (int lane = LANE_COUNT; lane < LANE_COUNT * 2; ++lane) {
            for (int x = 0; x < ROAD_LENGTH; ++x) {
                if (lanes[lane][x]) cout << "����";
                else cout << (x % 5 == 0 ? "��" : "  ");
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
        if (timeCounter % 8 == 0) {  // ������������Ƶ��
            system.addVehicle(rand() % 2);
        }

        system.updateLanes();
        system.printRoad();

        this_thread::sleep_for(chrono::milliseconds(200));
        timeCounter++;
    }
}

