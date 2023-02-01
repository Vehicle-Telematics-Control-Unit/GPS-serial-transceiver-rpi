#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <iostream>
#include "GPSserial.hpp"


GPSserial::GPSserial(std::string portName, uint32_t baudRate=BR_9600) : portName_m(portName),baudRate_m(baudRate)
{
}

int32_t GPSserial::openCharDeviceFile()
{
    const char *portname = portName_m.c_str();

    fileDescriptor_m = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (fileDescriptor_m < 0)
    {
        std::cout << "Error opening " <<  portname << " " << strerror(errno) << '\n';
        return -1;
    }

    return 0;
}

int32_t GPSserial::initCommunicationAttributes()
{
    struct termios tty;

    if (tcgetattr(fileDescriptor_m, &tty) < 0)
    {
        std::cout << "Error from tcgetattr: " <<  strerror(errno) << '\n';
        return -1;
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
        return -1;
    }
    return 0;
}

int32_t GPSserial::initSerialInterface()
{

    if(openCharDeviceFile() < 0)
        return -1;

    if(initCommunicationAttributes() < 0)
        return -1;

    return 0;
}

int32_t GPSserial::receiveGPSdata(std::string& gpsDataStr)
{
    tcdrain(fileDescriptor_m);

    char readBuffer[300];
    uint32_t readLineSize;

    while(true)
    {
        readLineSize = read(fileDescriptor_m, readBuffer, sizeof(readBuffer) - 1);
        char* start = strstr(readBuffer, "GPGGA");
        char* end = strstr(readBuffer, "\n");
        std::cout << "===>" << readLineSize << "\n";
        if(start - readBuffer == 1 && end - readBuffer > 0)
        {
            char temp[300];
            strncpy(temp, start, end - start);
            temp[end-start-1] = '\0';
            gpsDataStr = temp;
            break;
        }
        else if (readLineSize <= 0)
        {
            std::cout << "Error from read: " << readLineSize << " " << strerror(errno) << '\n';
            return -1;
        }
    }

    return 0;
}