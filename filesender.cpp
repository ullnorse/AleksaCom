#include "filesender.h"

#include <QDebug>
#include <QStandardPaths>
#include <QFileDialog>

FileSender::FileSender(QObject *parent)
    : QObject{parent}
{

}

void FileSender::sendFile(Protocol protocol)
{
#ifdef Q_OS_WIN
    auto dir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
#elif defined Q_OS_LINUX
        auto dir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
#endif
    auto fileName = QFileDialog::getOpenFileName(nullptr, "File to send", dir);

    if (fileName.isEmpty())
    {
        return;
    }

    switch (protocol)
    {
        case Protocol::Plain:
        {
            sendFilePlain(fileName);
            break;
        }
        case Protocol::Script:
        {
            qDebug() << "Sending file Plain";
            break;
        }
        case Protocol::XModem:
        {
            qDebug() << "Sending file XModem";
            break;
        }
        case Protocol::YModem:
        {
            qDebug() << "Sending file YModem";
            break;
        }
        case Protocol::ZModem:
        {
            qDebug() << "Sending file ZModem";
            break;
        }
    }
}

void FileSender::sendFilePlain(const QString &fileName)
{
    auto data = QByteArray();

    QFile file{fileName};

    if (file.open(QIODevice::OpenModeFlag::ReadOnly))
    {
        data = file.readAll();
    }

    data.replace("\r\n", "\n");

    emit fileData(data);
}
