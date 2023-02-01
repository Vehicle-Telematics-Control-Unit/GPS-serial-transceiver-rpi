#include <iostream>
#include <mutex>
#include <utility>
#include "GPSserial.hpp"

int main()
{
    std::cout << "heyyy";
    GPSserial gps("/dev/ttyACM1", GPSserial::BR_9600);

    if(gps.initSerialInterface() < 0)
        return -1;

    std::pair<std::string, std::mutex> gpsData;
    gpsData.first = "";
    while(1)
    {
        gps.receiveGPSdata(gpsData);
        gpsData.second.lock();
        std::cout << gpsData.first;
        gpsData.second.unlock();
    }
    return 0;
}