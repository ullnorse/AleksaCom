#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QFile>
#include <QByteArray>

class Logger : public QObject
{
    Q_OBJECT
public:
    explicit Logger(QObject *parent = nullptr);
    ~Logger();

signals:
    void loggingStarted();
    void loggingStopped();

public slots:
    void startLogging();
    void stopLogging();
    void logData(const QByteArray &data);

private:
    QFile *m_logFile = nullptr;
};

#endif // LOGGER_H
