#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QObject>
#include <QSerialPortInfo>
#include <QStringList>

class SerialPort : public QObject
{
    Q_OBJECT
public:
    explicit SerialPort(QObject *parent = nullptr);
    QStringList getPortNames();

signals:
    void serialPortNames(const QStringList &portNames);

public slots:
    void serialPortsRequested();

private:
};

#endif // SERIALPORT_H
