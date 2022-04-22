#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QLabel>
#include <QSerialPortInfo>
#include <QButtonGroup>
#include <QPair>
#include <QFont>
#include <QFontDialog>
#include <QTime>
#include <QDateTime>
#include <QStandardPaths>
#include <QFileDialog>
#include <QFile>
#include <QDataStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_serialPort = new SerialPort(this);
    m_logger = new Logger(this);




    connect(ui->pbRescan, &QPushButton::clicked, m_serialPort, &SerialPort::serialPortsRequested);
    connect(m_serialPort, &SerialPort::serialPortNames, this, &MainWindow::serialPortNames);

    serialPortNames(m_serialPort->getPortNames());

    connect(ui->pbConDiscon, &QPushButton::clicked, this, &MainWindow::onConnectDisconnectClicked);
//    connect(ui->pbAboutQt, &QPushButton::clicked, QApplication::instance(), &QApplication::aboutQt);
    connect(ui->actionAbout_AleksaCom, &QAction::triggered, QApplication::instance(), &QApplication::aboutQt);
    connect(ui->actionAbout_Qt, &QAction::triggered, QApplication::instance(), &QApplication::aboutQt);
    connect(ui->pbReceiveClear, &QPushButton::clicked, ui->teReceive, &QPlainTextEdit::clear);

    connect(this, &MainWindow::connectClicked, m_serialPort, &SerialPort::connect);
    connect(this, &MainWindow::disconnectClicked, m_serialPort, &SerialPort::disconnect);
    connect(m_serialPort, &SerialPort::serialPortData, this, &MainWindow::onSerialPortData);
    connect(m_serialPort, &SerialPort::connectionSuccessful, this, [this](){this->ui->pbConDiscon->setText("Disconnect");});
    connect(m_serialPort, &SerialPort::connectionFailed, this, [this](){this->ui->pbConDiscon->setText("Connect");});

    connect(ui->pbSetFont, &QPushButton::clicked, this, [this]()
    {
        this->ui->teReceive->setFont(QFontDialog::getFont(0, this));
    });

    connect(ui->pbSend, &QPushButton::clicked, this, &MainWindow::onSendClicked);
    connect(this, &MainWindow::dataForTransmit, m_serialPort, &SerialPort::send);

    connect(ui->pbSendFile, &QPushButton::clicked, this, &MainWindow::onSendFileClicked);
    connect(ui->pbAsciiTable, &QPushButton::clicked, this, &MainWindow::onAsciiTableClicked);


    // handle logging signals and slots
    connect(ui->pbStartLog, &QPushButton::clicked, m_logger, &Logger::startLogging);
    connect(ui->pbStopLog, &QPushButton::clicked, m_logger, &Logger::stopLogging);

    connect(m_logger, &Logger::loggingStarted, this, [this]()
    {
        ui->pbStartLog->setEnabled(false);
        ui->pbStopLog->setEnabled(true);
        connect(this, &MainWindow::dataForDisplay, m_logger, &Logger::logData);
    });

    connect(m_logger, &Logger::loggingStopped, this, [this]()
    {
        ui->pbStartLog->setEnabled(true);
        ui->pbStopLog->setEnabled(false);
        disconnect(this, &MainWindow::dataForDisplay, m_logger, &Logger::logData);
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

void MainWindow::onConnectDisconnectClicked()
{
    if (!ui->pbConDiscon->text().compare("Connect"))
    {
        auto settings = getSerialPortSettings();
        emit connectClicked(settings);
    }
    else if (!ui->pbConDiscon->text().compare("Disconnect"))
    {
        emit disconnectClicked();
        ui->pbConDiscon->setText("Connect");
    }
}

void MainWindow::onSerialPortData(const QByteArray &data)
{
    static int cnt = 0;

    int c = data.count(ui->spinBox_2->text().toInt());

    if (c != 0)
    {
        cnt += c;
        ui->lCounter->setText(QString("Counter = ") + QString::number(cnt));
    }

    QByteArray text;

    ui->teReceive->moveCursor(QTextCursor::End);

    if (ui->cbTime->isChecked())
    {
        text.append(QTime::currentTime().toString("hh:mm:ss.zzz> ").toUtf8());
    }

    text.append(data);
    ui->teReceive->insertPlainText(text);
    ui->teReceive->moveCursor(QTextCursor::End);

    emit dataForDisplay(text);
}

void MainWindow::onSendClicked()
{
    auto text = ui->leSend->text();

    if (ui->cbCR->isChecked())
    {
        if (ui->cbCRLF->isChecked())
        {
            text.append("\r\n");
        }
        else
        {
            text.append("\n");
        }
    }

    qDebug() << text;

    emit dataForTransmit(QByteArray(text.toUtf8()));

    ui->leSend->clear();
}

void MainWindow::onAsciiTableClicked()
{
    auto label = new QLabel();
    label->setPixmap(QPixmap::fromImage(QImage(":/img/ascii_table.png")));
    label->setFocus();
    label->setAttribute(Qt::WA_DeleteOnClose);
    label->show();
}

void MainWindow::onSendFileClicked()
{
    auto dir = QStandardPaths::standardLocations(QStandardPaths::StandardLocation::HomeLocation).at(0);
    auto fileName = QFileDialog::getOpenFileName(this, "File to send", dir);
    auto text = QByteArray();

    if (fileName.isEmpty())
    {
        return;
    }
    else
    {
        QFile file{fileName};

        if (file.open(QIODevice::OpenModeFlag::ReadOnly))
        {
            text = file.readAll();
        }
    }

//        qDebug() << text;

    emit dataForTransmit(text);
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

    return settings;
}

