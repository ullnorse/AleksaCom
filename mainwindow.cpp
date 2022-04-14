#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QLabel>
#include <QSerialPortInfo>
#include <QButtonGroup>
#include <QPair>

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

    connect(ui->pbConDiscon, &QPushButton::clicked, this, &MainWindow::onPbConnectClicked);
    connect(ui->pbAbout, &QPushButton::clicked, QApplication::instance(), &QApplication::aboutQt);

    connect(this, &MainWindow::pbConnectClicked, m_serialPort, &SerialPort::onPbConnectClicked);
    connect(m_serialPort, &SerialPort::serialPortData, this, &MainWindow::handleSerialPortData);
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

void MainWindow::onPbConnectClicked()
{
    auto settings = getSerialPortSettings();
    emit pbConnectClicked(settings);
}

void MainWindow::handleSerialPortData(const QString &data)
{
    ui->textEdit->append(data);
}

SerialPort::Settings MainWindow::getSerialPortSettings() const
{
    SerialPort::Settings settings;

    QButtonGroup buttonGroupBaudRate;
    QButtonGroup buttonGroupDataBits;
    QButtonGroup buttonGroupParity;
    QButtonGroup buttonGroupStopBits;
    QButtonGroup buttonGroupHandshaking;

    auto baudRateButtons      = ui->gbBaudRate->findChildren<QRadioButton*>();
    auto groupDataBitsButtons = ui->gbDataBits->findChildren<QRadioButton*>();
    auto parityButtons        = ui->gbParity->findChildren<QRadioButton*>();
    auto stopBitsButtons      = ui->gbStopBits->findChildren<QRadioButton*>();
    auto handshakingButtons   = ui->gbHandshaking->findChildren<QRadioButton*>();

    QList<QPair<QButtonGroup*, QList<QRadioButton*>>> buttonGroups;
    buttonGroups.append(qMakePair(&buttonGroupBaudRate, baudRateButtons));
    buttonGroups.append(qMakePair(&buttonGroupBaudRate, baudRateButtons));
    buttonGroups.append(qMakePair(&buttonGroupDataBits, groupDataBitsButtons));
    buttonGroups.append(qMakePair(&buttonGroupParity, parityButtons));
    buttonGroups.append(qMakePair(&buttonGroupStopBits, stopBitsButtons));
    buttonGroups.append(qMakePair(&buttonGroupHandshaking, handshakingButtons));

    for (auto & [buttonGroup, buttons] : buttonGroups)
    {
        for (int i = 0; i < buttons.size(); i++)
        {
            buttonGroup->addButton(buttons[i], i);
        }
    }

    settings.portName    = ui->comboBox->currentText();
    settings.baudRate    = static_cast<QSerialPort::BaudRate>(buttonGroupBaudRate.checkedButton()->objectName().remove(0, 2).toInt());
    settings.dataBits    = static_cast<QSerialPort::DataBits>(buttonGroupDataBits.checkedButton()->objectName().remove(0, 2).toInt());
    settings.parity      = static_cast<QSerialPort::Parity>(buttonGroupParity.checkedButton()->objectName().remove(0, 2).toInt());
    settings.stopBits    = static_cast<QSerialPort::StopBits>(buttonGroupStopBits.checkedButton()->objectName().remove(0, 2). toInt());
    settings.flowControl = QSerialPort::FlowControl::NoFlowControl;

    qDebug() << "Sending settings "
             << settings.portName << " "
             << settings.baudRate << " "
             << settings.dataBits << " "
             << settings.parity   << " "
             << settings.stopBits << " "
             << settings.flowControl;

    return settings;
}

