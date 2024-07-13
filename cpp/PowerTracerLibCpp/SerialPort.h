//
// Created by oxymmiron on 13.07.2024.
// Minimal version of SerialPort class
//

#ifndef RISCURELIBCPP_SERIALPORT_H
#define RISCURELIBCPP_SERIALPORT_H


#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

class SerialPort {
private:
    int fd;
public:
    SerialPort(const char* portName);

    ~SerialPort();

    void writeData(const char* data, int length);

    void readData(char* buffer, int length);
};


#endif //RISCURELIBCPP_SERIALPORT_H
