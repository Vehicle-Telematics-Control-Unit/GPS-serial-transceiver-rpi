#include <json.hpp>
#include "GPSserial.hpp"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <iostream>
#include <iomanip>
#include <memory>
#include <TinyGPS++.h>
#include <conf.hpp>

GPSserial::GPSserial(std::string portName, uint32_t baudRate, std::shared_ptr<ServiceManagerAdapter> serviceManager) : portName_m(portName), baudRate_m(baudRate), serviceManager_m(serviceManager)
{
}

bool GPSserial::openCharDeviceFile()
{
    const char *portname = portName_m.c_str();

    fileDescriptor_m = open(portname, O_RDONLY | O_NOCTTY);
    if (fileDescriptor_m < 0)
    {
        std::cout << "Error opening " << portname << " " << strerror(errno) << '\n';
        return false;
    }

    return true;
}

bool GPSserial::exec(std::string command)
{
    FILE *pipe = popen(command.c_str(), "r");
    if (!pipe)
    {
        return false;
    }

    pclose(pipe);
    return true;
}

bool GPSserial::initCommunicationAttributes()
{
    struct termios tty;

    if (tcgetattr(fileDescriptor_m, &tty) < 0)
    {
        std::cout << "Error from tcgetattr: " << strerror(errno) << '\n';
        return false;
    }
    cfsetospeed(&tty, (speed_t)baudRate_m);
    cfsetispeed(&tty, (speed_t)baudRate_m);

    tty.c_cflag |= (CLOCAL | CREAD); /* ignore modem controls */
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;      /* 8-bit characters */
    tty.c_cflag &= ~PARENB;  /* no parity bit */
    tty.c_cflag &= ~CSTOPB;  /* only need 1 stop bit */
    tty.c_cflag &= ~CRTSCTS; /* no hardware flowcontrol */

    /* setup for non-canonical mode */
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;

    /* fetch bytes as they become available */
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;

    if (tcsetattr(fileDescriptor_m, TCSANOW, &tty) != 0)
    {
        std::cout << "Error from tcsetattr: " << strerror(errno) << '\n';
        return false;
    }
    return true;
}

bool GPSserial::initSerialInterface()
{

    if (openCharDeviceFile() < 0)
        return false;

    if (initCommunicationAttributes() < 0)
        return false;

    return true;
}

void GPSserial::receiveGPSdata()
{
    static TinyGPSPlus gps;
    char data[1];
    while (read(fileDescriptor_m, &data[0], sizeof(char)) != -1)
    {
        std::cout << std::fixed << std::setprecision(10);
        if (gps.encode(data[0]))
        {
            if (gps.location.isValid())
            {
                std::cout << "Latitude, Long: ";
                std::cout << gps.location.lat(); // Prints latitude with 6 decimal places
                std::cout << ", ";
                std::cout << gps.location.lng() << "\n\n"; // Prints longitude with 6 decimal places

                using json = nlohmann::json;
                json jsonMessage = {
                    {"lat", ""},
                    {"lng", ""},
                };

                jsonMessage["lat"] = gps.location.lat();
                jsonMessage["lng"] = gps.location.lng();
                std::string message = jsonMessage.dump(2);
                serviceManager_m->updateEvent(GPS_EVENT_ID, std::vector<uint8_t>(message.begin(), message.end()));
                // std::cout << " ; Time: ";
                // std::cout << gps.time.getTime();

                // fetching the time
                std::time_t systemTime = std::time(nullptr);
                std::tm *localTime = std::localtime(&systemTime);
                if (!(localTime->tm_hour == gps.time.hour() && localTime->tm_min == gps.time.minute() && localTime->tm_sec == gps.time.second()))
                {
                    // updating the time with gps timing
                    localTime->tm_hour = gps.time.hour();
                    localTime->tm_min = gps.time.minute();
                    localTime->tm_sec = gps.time.second();
                    std::time_t newSystemTime = std::mktime(localTime);
                    exec("date -s @" + std::string(std::to_string(newSystemTime)));
                }
                // std::cout << (int)gps.time.hour();
                // std::cout << ":";
                // std::cout << (int)gps.time.minute();
                // std::cout << ":";
                // std::cout << (int)gps.time.second();
                // std::cout << ".";
                // std::cout << (int)gps.time.centisecond();
                // std::cout << "\n\n\n";
            }
        }
    }
}