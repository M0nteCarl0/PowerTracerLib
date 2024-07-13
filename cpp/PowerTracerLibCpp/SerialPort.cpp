//
// Created by oxymmiron on 13.07.2024.
//

#include "SerialPort.h"


SerialPort::SerialPort(const char* portName) {
    fd = open(portName, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1) {
        std::cerr << "Failed to open the port\n";
    }

    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) {
        std::cerr << "Error getting attributes\n";
    }

    tty.c_cflag &= ~PARENB; // No parity
    tty.c_cflag &= ~CSTOPB; // 1 stop bit
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8; // 8 bits per byte
    tty.c_cflag |= CREAD | CLOCAL; // Enable receiver, ignore modem status lines

    cfsetospeed(&tty, B9600); // Set baud rate
    cfsetispeed(&tty, B9600);

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        std::cerr << "Error setting attributes\n";
    }
}

SerialPort::~SerialPort() {
    close(fd);
}

void SerialPort::writeData(const char* data, int length) {
    write(fd, data, length);
}

void SerialPort::readData(char* buffer, int length) {
    read(fd, buffer, length);
}