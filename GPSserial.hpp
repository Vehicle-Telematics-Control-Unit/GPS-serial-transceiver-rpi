#include <atomic>
#include <memory>
#include <string>

//! \brief GPSserial class blueprint
class GPSserial{

private:
    std::string portName_m;
    uint32_t baudRate_m;
    int32_t fileDescriptor_m;


    int32_t openCharDeviceFile();
    int32_t initCommunicationAttributes();

public:
    //! \brief Contructor
    //! \param portName the tty port used ex: "/dev/ttyACM0"
    //! \param baudRate UART baud rate ex: BR_9600
    GPSserial(std::string portName, uint32_t baudRate);

    //! \brief opens the character device file
    //!        and initializes the serial communication
    int32_t initSerialInterface();

    //! \brief receive incoming data from serialport
    int32_t receiveGPSdata(std::pair<std::string, std::mutex>&gpsData);

    enum BaudRate{
        BR_50= 0000001,
        BR_75= 0000002,
        BR_110= 0000003,
        BR_134= 0000004,
        BR_150= 0000005,
        BR_200= 0000006,
        BR_300= 0000007,
        BR_600= 0000010,
        BR_1200= 0000011,
        BR_1800= 0000012,
        BR_2400= 0000013,
        BR_4800= 0000014,
        BR_9600= 0000015,
        BR_19200= 0000016,
        BR_38400= 0000017
    };
};
