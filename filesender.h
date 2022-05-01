#ifndef FILESENDER_H
#define FILESENDER_H

#include <QObject>
#include <QByteArray>
#include <QString>

class FileSender : public QObject
{
    Q_OBJECT
public:
    explicit FileSender(QObject *parent = nullptr);

    enum class Protocol
    {
        Plain,
        Script,
        XModem,
        YModem,
        ZModem
    };

signals:
    void fileData(const QByteArray &data);

public slots:
    void sendFile(FileSender::Protocol protocol);

private:
    void sendFilePlain(const QString &fileName);
};

#endif // FILESENDER_H
