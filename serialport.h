#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QObject>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QStringList>
#include <QByteArray>

class SerialPort : public QObject
{
    Q_OBJECT
public:
    explicit SerialPort(QObject *parent = nullptr);
    ~SerialPort();
    QStringList getPortNames();

    struct Settings
    {
        QString portName;
        QSerialPort::BaudRate baudRate;
        QSerialPort::Parity parity;
        QSerialPort::DataBits dataBits;
        QSerialPort::StopBits stopBits;
        QSerialPort::FlowControl flowControl;
    };

signals:
    void serialPortNames(const QStringList &portNames);
    void serialPortData(const QByteArray &data);
    void connectionSuccessful();
    void connectionFailed();

public slots:
    void serialPortsRequested();
    void connect(SerialPort::Settings &settings);
    void disconnect();
    void send(const QByteArray &data);

private:
    QSerialPort *m_serialPort;
};

#endif // SERIALPORT_H
