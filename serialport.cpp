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
    m_serialPort->setPortName(settings.portName);
    m_serialPort->setBaudRate(settings.baudRate);
    m_serialPort->setDataBits(settings.dataBits);
    m_serialPort->setFlowControl(settings.flowControl);
    m_serialPort->setStopBits(settings.stopBits);

    if (m_serialPort->open(QIODevice::ReadWrite))
    {
        m_serialPort->clear(QSerialPort::AllDirections);
        m_serialPort->setReadBufferSize(1000);
    }
    else
    {
        qDebug() << "Can't open serial port";
    }
}

void SerialPort::onPbDisconnectClicked()
{
    if (m_serialPort->isOpen())
    {
        m_serialPort->close();
    }
}

void SerialPort::handleReadyRead()
{
    if (m_serialPort->canReadLine())
    {
        emit serialPortData(m_serialPort->readLine());
    }

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
