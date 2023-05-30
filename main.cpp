#include <iostream>
#include <thread>
#include <chrono>
#include "GPSserial.hpp"

int main()
{
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);
    GPSserial gps("/dev/ttyACM0", GPSserial::BR_9600);

    std::string gpsData;
    int counter = 0;
    while(true)
    {
        // if failed to init retry
        if(!gps.initSerialInterface())
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::cout << "retrying to get GPS data!\n";
            continue;
        }

        gps.receiveGPSdata();
    }
    return 0;
}