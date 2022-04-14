#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QLabel>
#include <QSerialPortInfo>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_serialPortsModel = new QStringListModel(this);
    ui->comboBox->setModel(m_serialPortsModel);

    connect(ui->pbRescan, &QPushButton::clicked, this, &MainWindow::updateComboBox);

    updateComboBox();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateComboBox()
{
    auto ports = QSerialPortInfo::availablePorts();
    auto list = QStringList();

    for (auto &port : ports)
    {
        list << port.portName();
    }

    m_serialPortsModel->setStringList(list);
}

