#include <iostream>
#include <thread>
#include <chrono>
#include <GPSserial.hpp>
#include <conf.hpp>
#include <ServiceManagerAdapter.hpp>

int main()
{
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    std::shared_ptr<ServiceManagerAdapter> vsomeService_shared = std::make_shared<ServiceManagerAdapter>(SERVICE_ID, INSTANCE_ID, EVENTGROUP_ID, "GPS");

    if (!vsomeService_shared->init())
    {
        std::cerr << "Couldn't initialize vsomeip services" << std::endl;
        return -1;
    }
    vsomeService_shared->offer();
    vsomeService_shared->addEvent(GPS_EVENT_ID);
    vsomeService_shared->offerEvents();

    GPSserial gps("/dev/ttyACM0", GPSserial::BR_9600, vsomeService_shared);

    std::string gpsData;
    int counter = 0;
    while (true)
    {
        // if failed to init retry
        if (!gps.initSerialInterface())
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::cout << "retrying to get GPS data!\n";
            continue;
        }

        gps.receiveGPSdata();
    }
    return 0;
}