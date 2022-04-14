#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QLabel>
#include <QSerialPortInfo>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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

    ui->comboBox->clear();

    for (auto &port : ports)
    {
        ui->comboBox->addItem(port.portName());
    }
}

