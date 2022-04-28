#include "serialport.h"

#include <QDebug>

#include <algorithm>
#include <iterator>


SerialPort::SerialPort(QObject *parent)
    : QObject{parent}
{
    m_serialPort = new QSerialPort(this);

    QObject::connect(m_serialPort, &QSerialPort::readyRead, this, [this]()
    {
        emit serialPortData(m_serialPort->readAll());
    });
}

SerialPort::~SerialPort()
{
    disconnect();
}

void SerialPort::serialPortsRequested()
{
    emit serialPortNames(getPortNames());
}

void SerialPort::connect(SerialPort::Settings &settings)
{
    m_serialPort->setPortName(settings.portName);
    m_serialPort->setBaudRate(settings.baudRate);
    m_serialPort->setDataBits(settings.dataBits);
    m_serialPort->setFlowControl(settings.flowControl);
    m_serialPort->setStopBits(settings.stopBits);

    if (m_serialPort->open(QIODevice::ReadWrite))
    {
        m_serialPort->clear(QSerialPort::AllDirections);
        emit connectionSuccessful();
    }
    else
    {
        qDebug() << "Can't open serial port";
        emit connectionFailed();
    }
}

void SerialPort::disconnect()
{
    if (m_serialPort->isOpen())
    {
        m_serialPort->close();
    }
}

void SerialPort::send(const QByteArray &data)
{
    if (m_serialPort->isOpen())
    {
        m_serialPort->write(data);
    }
}

QStringList SerialPort::getPortNames()
{
    auto list = QStringList();
    auto ports = QSerialPortInfo::availablePorts();

#ifdef Q_OS_WIN
    std::transform(ports.begin(), ports.end(),
                   std::back_inserter(list), [](QSerialPortInfo info) {return info.portName();});
#elif defined Q_OS_LINUX
    std::transform(ports.begin(), ports.end(),
                   std::back_inserter(list), [](QSerialPortInfo info) {return info.systemLocation();});
#endif

    return list;
}
