#include "serialport.h"

SerialPort::SerialPort(QObject *parent)
    : QObject{parent}
{

}

void SerialPort::serialPortsRequested()
{
    emit serialPortNames(getPortNames());
}

QStringList SerialPort::getPortNames()
{
    auto ports = QSerialPortInfo::availablePorts();
    auto list = QStringList();

    for (auto &port : ports)
    {
        list << port.portName();
    }

    return list;
}
