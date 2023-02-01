#include <iostream>
#include <thread>
#include <chrono>
#include "GPSserial.hpp"

int main()
{
    setbuf(stdout, NULL);
    GPSserial gps("/dev/gps_ser", GPSserial::BR_9600);

    std::string gpsData;
    int counter = 0;
    while(true)
    {
        // if failed to init retry
        if(gps.initSerialInterface() < 0)
        {
            std::this_thread::sleep_for(std::chrono::seconds(2));
            std::cout << "retrying to get GPS data!\n";
            continue;
        }

        while(gps.receiveGPSdata(gpsData) == 0)
        {
            std::cout << (counter = (counter+1) % 9999) << "-->" << gpsData << '\n';
        }
    }
    return 0;
}