#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QFile>
#include <QByteArray>
#include <QString>

class Logger : public QObject
{
    Q_OBJECT
public:
    explicit Logger(QObject *parent = nullptr);
    ~Logger();

signals:
    void loggingStarted(const QString &fileName);
    void loggingStopped();

public slots:
    void startLogging(bool append = false);
    void stopLogging();
    void logData(const QByteArray &data);

private:
    QFile *m_logFile;
};

#endif // LOGGER_H
