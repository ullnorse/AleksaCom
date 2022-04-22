#include "logger.h"

#include <QDebug>
#include <QStandardPaths>
#include <QDateTime>
#include <QFileDialog>
#include <QDataStream>

Logger::Logger(QObject *parent)
    : QObject{parent}
{

}

Logger::~Logger()
{
    if (m_logFile->isOpen())
    {
        m_logFile->close();
    }
}

void Logger::startLogging()
{
    auto dir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    auto name = QDateTime::currentDateTime().toString("/yyyyMMddhhmmss.log");

    auto fileName = QFileDialog::getSaveFileName(nullptr,
                                                 "Select log file",
                                                 dir + name,
                                                 "Log files (*.log)",
                                                 0,
                                                 QFileDialog::Option::DontConfirmOverwrite);

    m_logFile = new QFile(fileName);

    if (m_logFile->open(QIODeviceBase::OpenModeFlag::Append))
    {
        emit loggingStarted();
    }
    else
    {
        qDebug() << "Can't open log file " + fileName;
    }
}

void Logger::stopLogging()
{
    if (m_logFile->isOpen())
    {
        m_logFile->close();
    }

    emit loggingStopped();
}

void Logger::logData(const QByteArray &data)
{
    m_logFile->write(data);
}
