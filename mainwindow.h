#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QStringListModel>
#include <QStringList>
#include <QString>

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
    void pbConnectClicked(SerialPort::Settings &settings);

private slots:
    void serialPortNames(const QStringList &portNames);
    void onPbConnectClicked();
    void handleSerialPortData(const QString &data);

private:
    SerialPort::Settings getSerialPortSettings() const;

private:
    Ui::MainWindow *ui;

private:
    SerialPort *m_serialPort;
};
#endif // MAINWINDOW_H
