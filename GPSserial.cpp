#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <iostream>
#include <iomanip>
#include "GPSserial.hpp"
#include "TinyGPSPlus_/src/TinyGPS++.h"


GPSserial::GPSserial(std::string portName, uint32_t baudRate=BR_9600) : portName_m(portName),baudRate_m(baudRate)
{
}

bool GPSserial::openCharDeviceFile()
{
    const char *portname = portName_m.c_str();

    fileDescriptor_m = open(portname, O_RDONLY | O_NOCTTY);
    if (fileDescriptor_m < 0)
    {
        std::cout << "Error opening " <<  portname << " " << strerror(errno) << '\n';
        return false;
    }

    return true;
}

bool GPSserial::initCommunicationAttributes()
{
    struct termios tty;

    if (tcgetattr(fileDescriptor_m, &tty) < 0)
    {
        std::cout << "Error from tcgetattr: " <<  strerror(errno) << '\n';
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

    if(openCharDeviceFile() < 0)
        return false;

    if(initCommunicationAttributes() < 0)
        return false;

    return true;
}

void GPSserial::receiveGPSdata()
{
    static TinyGPSPlus gps;
    char data[1];
    while(read(fileDescriptor_m, &data[0],sizeof(char)) != -1)
    {
        // if(data[0] == '\n')
        //     std::cout << "\n>>>>>>>>>>>>>>lol<<<<<<<<<<";
        // std::cout << data[0];
        std::cout << std::fixed << std::setprecision(10);
        if (gps.encode(data[0])) 
        {
            if (gps.location.isValid()) 
            {
                std::cout << "Latitude: ";
                std::cout << gps.location.lat(); // Prints latitude with 6 decimal places

                std::cout << "Longitude: ";
                std::cout << gps.location.lng(); // Prints longitude with 6 decimal places
                std::cout << "\n\n\n";
            }
        }
    }
}