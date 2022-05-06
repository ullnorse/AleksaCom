#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QStringListModel>
#include <QStringList>
#include <QString>
#include <QByteArray>
#include <QLabel>
#include <QFile>

#include "serialport.h"
#include "logger.h"
#include "macros.h"
#include "filesender.h"
#include "statusbar.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void connectClicked(SerialPort::Settings &settings);
    void dataForTransmit(const QByteArray &data);
    void dataForDisplay(const QByteArray &data);
    void sendFile(FileSender::Protocol protocol);

private slots:
    void serialPortNames(const QStringList &portNames);
    void onSerialPortData(const QByteArray &data);
    void onSendClicked();
    void onAsciiTableClicked();
    void onSendFileClicked();

private:
    SerialPort::Settings getSerialPortSettings() const;
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::MainWindow *ui;

private:
    SerialPort *m_serialPort;
    Logger *m_logger;
    Macros *m_macros;
    FileSender *m_fileSender;
    StatusBar *m_statusBar;

    // QObject interface
public:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
};
#endif // MAINWINDOW_H
