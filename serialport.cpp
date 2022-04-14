#include "serialport.h"

#include <QDebug>


SerialPort::SerialPort(QObject *parent)
    : QObject{parent}
{
    m_serialPort = new QSerialPort(this);


    connect(m_serialPort, &QSerialPort::readyRead, this, &SerialPort::handleReadyRead);
}

SerialPort::~SerialPort()
{
    m_serialPort->close();
}

void SerialPort::serialPortsRequested()
{
    emit serialPortNames(getPortNames());
}

void SerialPort::onPbConnectClicked(SerialPort::Settings &settings)
{
    qDebug("From serial port pb connect clicked");

    m_serialPort->setPortName(settings.portName);
    m_serialPort->setBaudRate(settings.baudRate);
    m_serialPort->setDataBits(settings.dataBits);
    m_serialPort->setFlowControl(settings.flowControl);
    m_serialPort->setStopBits(settings.stopBits);

//    if (m_serialPort->open(QIODevice::ReadOnly) == true)
//    {
//        qDebug() << "Device successfully opened";
//    }
//    else
//    {
//        qDebug() << "Can't open serial port";
//    }
}

void SerialPort::handleReadyRead()
{
    emit serialPortData(QString(m_serialPort->readAll()));
}

QStringList SerialPort::getPortNames()
{
    auto ports = QSerialPortInfo::availablePorts();
    auto list = QStringList();

    for (auto &port : ports)
    {
#ifdef Q_OS_WIN
        list << port.portName();
#elif defined Q_OS_LINUX
        list << port.systemLocation();
#endif
    }

    return list;
}
