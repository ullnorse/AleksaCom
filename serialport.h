#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QObject>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QStringList>

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
    void serialPortData(const QString &data);

public slots:
    void serialPortsRequested();
    void onPbConnectClicked(SerialPort::Settings &settings);

private:
    void handleReadyRead();

private:
    QSerialPort *m_serialPort;
};

#endif // SERIALPORT_H
