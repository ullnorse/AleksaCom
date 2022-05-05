#include "logger.h"

#include <QDebug>
#include <QStandardPaths>
#include <QDateTime>
#include <QFileDialog>
#include <QMessageBox>

Logger::Logger(QObject *parent)
    : QObject{parent}, m_logFile(new QFile(this))
{

}

Logger::~Logger()
{
    m_logFile->close();
}

void Logger::startLogging(bool append)
{
#ifdef Q_OS_WIN
    auto dir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
#elif defined Q_OS_LINUX
        auto dir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
#endif

    auto name = QDateTime::currentDateTime().toString("/yyyyMMddhhmmss.log");
    QString fileName;

    if (append)
    {
        fileName = QFileDialog::getOpenFileName(nullptr,
                                                "Select log file",
                                                dir + name,
                                                "Log files (*.log)");
    }
    else
    {
        fileName = QFileDialog::getSaveFileName(nullptr,
                                                "Select log file",
                                                dir + name,
                                                "Log files (*.log)");
    }


    if (fileName.isEmpty())
    {
        QMessageBox::information(nullptr, "Opening file failed", "Could not open file for logging");
        return;
    }

    m_logFile->setFileName(fileName);

    if (m_logFile->open(QIODevice::Append))
    {
        emit loggingStarted(m_logFile->fileName());
    }
    else
    {
        QMessageBox::information(nullptr, "Opening file failed", QString("Could not open file %1 for writting").arg(m_logFile->fileName()));
    }
}

void Logger::stopLogging()
{
    m_logFile->close();

    emit loggingStopped();
}

void Logger::logData(const QByteArray &data)
{
    if (m_logFile->isOpen())
    {
        m_logFile->write(data);
    }
}
