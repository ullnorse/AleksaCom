#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QLabel>
#include <QSerialPortInfo>
#include <QButtonGroup>
#include <QPair>
#include <QFont>
#include <QFontDialog>
#include <QTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_serialPort = new SerialPort(this);

    connect(ui->pbRescan, &QPushButton::clicked, m_serialPort, &SerialPort::serialPortsRequested);
    connect(m_serialPort, &SerialPort::serialPortNames, this, &MainWindow::serialPortNames);

    serialPortNames(m_serialPort->getPortNames());

    connect(ui->pbConDiscon, &QPushButton::clicked, this, &MainWindow::onPbConnectClicked);
//    connect(ui->pbAboutQt, &QPushButton::clicked, QApplication::instance(), &QApplication::aboutQt);
    connect(ui->pbReceiveClear, &QPushButton::clicked, ui->teReceive, &QPlainTextEdit::clear);

    connect(this, &MainWindow::pbConnectClicked, m_serialPort, &SerialPort::onPbConnectClicked);
    connect(this, &MainWindow::pbDisconnectClicked, m_serialPort, &SerialPort::onPbDisconnectClicked);
    connect(m_serialPort, &SerialPort::serialPortData, this, &MainWindow::handleSerialPortData);

    connect(ui->pbSetFont, &QPushButton::clicked, this, [this]()
    {
        this->ui->teReceive->setFont(QFontDialog::getFont(0, this));
    });
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
    if (!ui->pbConDiscon->text().compare("Connect"))
    {
        ui->pbConDiscon->setText("Disconnect");
        auto settings = getSerialPortSettings();
        emit pbConnectClicked(settings);
    }
    else
    {
        ui->pbConDiscon->setText("Connect");
        emit pbDisconnectClicked();
    }

}

void MainWindow::handleSerialPortData(const QByteArray &data)
{
    static int cnt = 0;

    int c = data.count(ui->spinBox_2->text().toInt());

    if (c != 0)
    {
        cnt += c;
        ui->lCounter->setText(QString("Counter = ") + QString::number(cnt));
    }


    ui->teReceive->moveCursor(QTextCursor::End);

    if (ui->cbTime->isChecked())
    {
        ui->teReceive->insertPlainText(QTime::currentTime().toString("hh:mm:ss.zzz> "));
    }

    ui->teReceive->insertPlainText(QString(data));
    ui->teReceive->moveCursor(QTextCursor::End);
}

SerialPort::Settings MainWindow::getSerialPortSettings() const
{
    SerialPort::Settings settings;

    settings.portName    = ui->comboBox->currentText();
    settings.baudRate    = static_cast<QSerialPort::BaudRate>(ui->bgBaudRate->checkedButton()->objectName().remove(0, 10).toInt());
    settings.dataBits    = static_cast<QSerialPort::DataBits>(ui->bgDataBits->checkedButton()->objectName().remove(0, 10).toInt());
    settings.parity      = static_cast<QSerialPort::Parity>(ui->bgParity->checkedButton()->objectName().remove(0, 8).toInt());
    settings.stopBits    = static_cast<QSerialPort::StopBits>(ui->bgStopBits->checkedButton()->objectName().remove(0, 10). toInt());
    settings.flowControl = static_cast<QSerialPort::FlowControl>(ui->bgFlowControl->checkedButton()->objectName().remove(0, 13).toInt());

    qDebug() << "Sending settings "
             << settings.portName << " "
             << settings.baudRate << " "
             << settings.dataBits << " "
             << settings.parity   << " "
             << settings.stopBits << " "
             << settings.flowControl;

    return settings;
}

