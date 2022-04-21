#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QStringListModel>
#include <QStringList>
#include <QString>
#include <QByteArray>
#include <QLabel>

#include "serialport.h"

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
    void disconnectClicked();
    void dataForTransmit(const QByteArray &data);

private slots:
    void serialPortNames(const QStringList &portNames);
    void onConnectDisconnectClicked();
    void onSerialPortData(const QByteArray &data);
    void onSendClicked();
    void onAsciiTableClicked();
    void onSendFileClicked();

private:
    SerialPort::Settings getSerialPortSettings() const;

private:
    Ui::MainWindow *ui;

private:
    SerialPort *m_serialPort;

    QLabel *label;
};
#endif // MAINWINDOW_H
