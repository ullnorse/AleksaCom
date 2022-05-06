#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "macros.h"
#include "connectbutton.h"

#include <QLabel>
#include <QButtonGroup>
#include <QFont>
#include <QFontDialog>
#include <QTime>
#include <QDateTime>
#include <QStandardPaths>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    QCoreApplication::setOrganizationDomain(QStringLiteral("AleksaCom"));
    QCoreApplication::setApplicationName(QStringLiteral("AleksaCom6"));
    QCoreApplication::setApplicationVersion(QStringLiteral("0.0.1"));

    ui->setupUi(this);

    m_serialPort = new SerialPort(this);
    m_logger = new Logger(this);
    m_macros = new Macros();
    m_fileSender = new FileSender(this);
    m_statusBar = new StatusBar(this);

    serialPortNames(m_serialPort->getPortNames());



    connect(ui->cbSerialPortNames, &QComboBox::highlighted, m_serialPort, &SerialPort::serialPortsRequested);



    connect(ui->connectionButton, &ConnectButton::connectClicked, this, [this]()
    {
        auto settings = getSerialPortSettings();
        emit connectClicked(settings);
    });

    connect(ui->connectionButton, &ConnectButton::disconnectClicked, this, [this]()
    {
        ui->pbSendFile->setEnabled(false);
        m_statusBar->setConnectionStatus(false);
    });
    connect(ui->connectionButton, &ConnectButton::disconnectClicked, m_serialPort, &SerialPort::disconnect);



    connect(ui->actionASCII_table, &QAction::triggered,this, &MainWindow::onAsciiTableClicked);
    connect(ui->actionAbout_AleksaCom, &QAction::triggered, this, [this]()
    {
        QMessageBox::about(this, "About AleksaCom",
                           tr("This is AleksaCom %1<br>(c)2022-2022 Aleksa Jonic, &lt;ullnorse@gmail.com&gt;"
                              "<br>Licensed under the GNU GPL version 3 (or any later version).")
                               .arg("0.0.1"));
    });
    connect(ui->actionAbout_Qt, &QAction::triggered, qApp, &QApplication::aboutQt);
    connect(ui->pbReceiveClear, &QPushButton::clicked, ui->dataDisplay, &DataDisplay::clear);

    connect(this, &MainWindow::connectClicked, m_serialPort, &SerialPort::connect);
    connect(m_serialPort, &SerialPort::serialPortData, this, &MainWindow::onSerialPortData);
    connect(m_serialPort, &SerialPort::connectionSuccessful, ui->connectionButton, &ConnectButton::changeToDisconnect);
    connect(m_serialPort, &SerialPort::connectionSuccessful, this, [this]() { ui->pbSendFile->setEnabled(true); });
    connect(m_serialPort, &SerialPort::connectionFailed, ui->connectionButton, &ConnectButton::changeToConnect);
    connect(m_serialPort, &SerialPort::serialPortNames, this, &MainWindow::serialPortNames);

    connect(ui->actionSet_Font, &QAction::triggered, this, [this]() { ui->dataDisplay->setFont(QFontDialog::getFont(0, ui->dataDisplay->font())); });

    connect(ui->pbSend, &QPushButton::clicked, this, &MainWindow::onSendClicked);
    connect(this, &MainWindow::dataForTransmit, m_serialPort, &SerialPort::send);

    connect(ui->pbSendFile, &QPushButton::clicked, this, &MainWindow::onSendFileClicked);

    // handle datadisplay signals and slots
    connect(this, &MainWindow::dataForDisplay, ui->dataDisplay, &DataDisplay::displayData);
    connect(ui->bgHexAscii, &QButtonGroup::buttonClicked, this, [this](QAbstractButton *button)
    {
        if (button->objectName() == "rbAscii")
        {
            ui->dataDisplay->setMode(DataDisplay::DisplayMode::ASCII);
        }
        else if (button->objectName() == "rbHex")
        {
            ui->dataDisplay->setMode(DataDisplay::DisplayMode::HEX);
        }
    });


    // handle logging signals and slots
    connect(ui->pbStartLog, &QPushButton::clicked, this, [this]()
    {
        if (ui->cbAppend->isChecked())
        {
            m_logger->startLogging(true);
        }
        else
        {
            m_logger->startLogging(false);
        }
    });

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

    connect(ui->pbSetMacros, &QPushButton::clicked, this, [this]()
    {
        m_macros->show();
        m_macros->activateWindow();
    });

    connect(m_macros, &Macros::macroText, this, [this](const QString &text)
    {
        m_serialPort->send(QByteArray(text.toUtf8()));
    });


    auto macroButtons = ui->wMacroButtons->findChildren<QPushButton*>(nullptr);
    for (auto &button : macroButtons)
    {
        connect(button, &QPushButton::clicked, m_macros, &Macros::onMacroButtonClicked);
    }

    ui->leSend->installEventFilter(this);

    connect(ui->cbStayOnTop, &QCheckBox::clicked, ui->dataDisplay, &DataDisplay::setScrolling);

    connect(m_macros, &Macros::macroLabelTextChanged, this, [this](const QString &buttonName, const QString &text)
    {
        auto button = ui->wMacroButtons->findChild<QPushButton*>(buttonName);
        button->setText(text);
    });

    connect(this, &MainWindow::sendFile, m_fileSender, &FileSender::sendFile);
    connect(m_fileSender, &FileSender::fileData, m_serialPort, &SerialPort::send);

    // Handle status bar
    setStatusBar(m_statusBar);
    connect(this, &MainWindow::dataForTransmit, this, [this](const QByteArray &data){ m_statusBar->incrementTxCount(data.count()); });
    connect(m_serialPort, &SerialPort::serialPortData, this, [this](const QByteArray &data) { m_statusBar->incrementRxCount(data.size()); });
    connect(m_serialPort, &SerialPort::connectionSuccessful, this, [this]() { m_statusBar->setConnectionStatus(true); });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::serialPortNames(const QStringList &portNames)
{
    ui->cbSerialPortNames->clear();
    ui->cbSerialPortNames->addItems(portNames);
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

    if (ui->cbTime->isChecked())
    {
        text.append(QTime::currentTime().toString("hh:mm:ss.zzz> ").toUtf8());
    }

    text.append(data);

    emit dataForDisplay(text);
}

void MainWindow::onSendClicked()
{
    auto text = ui->leSend->text();

    auto lineEnd = ui->cbTransmitCRLF->currentText();

    if (lineEnd == "LF")
    {
        text.append("\n");
    }
    else if (lineEnd == "CR")
    {
        text.append("\r");
    }
    else if (lineEnd == "CR/LF")
    {
        text.append("\r\n");
    }
    else if (lineEnd == "Hex")
    {
        qDebug() << "Hex";
    }

    emit dataForTransmit(QByteArray(text.toUtf8()));
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
    auto protocolText = ui->cbProtocol->currentText();
    auto protocol = FileSender::Protocol::Plain;

    if (protocolText == "Plain")
    {
        protocol = FileSender::Protocol::Plain;
    }
    else if (protocolText == "Script")
    {
        protocol = FileSender::Protocol::Script;
    }
    else if (protocolText == "XModem")
    {
        protocol = FileSender::Protocol::XModem;
    }
    else if (protocolText == "YModem")
    {
        protocol = FileSender::Protocol::YModem;
    }
    else if (protocolText == "ZModem")
    {
        protocol = FileSender::Protocol::ZModem;
    }

    emit sendFile(protocol);
}

SerialPort::Settings MainWindow::getSerialPortSettings() const
{
    SerialPort::Settings settings;

    settings.portName    = ui->cbSerialPortNames->currentText();
    settings.baudRate    = static_cast<QSerialPort::BaudRate>(ui->bgBaudRate->checkedButton()->objectName().remove(0, 10).toInt());
    settings.dataBits    = static_cast<QSerialPort::DataBits>(ui->bgDataBits->checkedButton()->objectName().remove(0, 10).toInt());
    settings.parity      = static_cast<QSerialPort::Parity>(ui->bgParity->checkedButton()->objectName().remove(0, 8).toInt());
    settings.stopBits    = static_cast<QSerialPort::StopBits>(ui->bgStopBits->checkedButton()->objectName().remove(0, 10). toInt());
    settings.flowControl = static_cast<QSerialPort::FlowControl>(ui->bgFlowControl->checkedButton()->objectName().remove(0, 13).toInt());

    return settings;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_macros->isVisible())
    {
        m_macros->close();
    }

    event->accept();
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->leSend && event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

        if (keyEvent->key() == Qt::Key_Return)
        {
            onSendClicked();
            return true;
        }
    }

    return QWidget::eventFilter(watched, event);
}
