#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QLabel>
#include <QSerialPortInfo>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_serialPort = new SerialPort(this);

    connect(ui->pbRescan, &QPushButton::clicked, m_serialPort, &SerialPort::serialPortsRequested);
    connect(m_serialPort, &SerialPort::serialPortNames, this, &MainWindow::serialPortNames);

    serialPortNames(m_serialPort->getPortNames());

    ui->rb115200->setChecked(true);
    ui->rb8->setChecked(true);
    ui->rbNone->setChecked(true);
    ui->rb1->setChecked(true);
    ui->rbHandshakeNone->setChecked(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::serialPortNames(const QStringList &portNames)
{
    ui->comboBox->clear();
    ui->comboBox->addItems(portNames);
}

