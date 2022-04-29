#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "macros.h"

#include <QLabel>
#include <QButtonGroup>
#include <QFont>
#include <QFontDialog>
#include <QTime>
#include <QDateTime>
#include <QStandardPaths>
#include <QFileDialog>
#include <QFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    QCoreApplication::setApplicationName(QStringLiteral("AleksaCom"));

    ui->setupUi(this);

    m_serialPort = new SerialPort(this);
    m_logger = new Logger(this);
    m_macros = new Macros();



    connect(ui->pbRescan, &QPushButton::clicked, m_serialPort, &SerialPort::serialPortsRequested);
    connect(m_serialPort, &SerialPort::serialPortNames, this, &MainWindow::serialPortNames);

    serialPortNames(m_serialPort->getPortNames());

    connect(ui->pbConDiscon, &QPushButton::clicked, this, &MainWindow::onConnectDisconnectClicked);
    connect(ui->actionAbout_AleksaCom, &QAction::triggered, QApplication::instance(), &QApplication::aboutQt);
    connect(ui->actionAbout_Qt, &QAction::triggered, QApplication::instance(), &QApplication::aboutQt);
    connect(ui->pbReceiveClear, &QPushButton::clicked, ui->dataDisplay, &DataDisplay::clear);

    connect(this, &MainWindow::connectClicked, m_serialPort, &SerialPort::connect);
    connect(this, &MainWindow::disconnectClicked, m_serialPort, &SerialPort::disconnect);
    connect(m_serialPort, &SerialPort::serialPortData, this, &MainWindow::onSerialPortData);
    connect(m_serialPort, &SerialPort::connectionSuccessful, this, [this]()
    {
        ui->pbConDiscon->setText("Disconnect");
        ui->pbRescan->setEnabled(false);
    });
    connect(m_serialPort, &SerialPort::connectionFailed, this, [this](){this->ui->pbConDiscon->setText("Connect");});

    connect(ui->pbSetFont, &QPushButton::clicked, this, [this]()
    {
        ui->dataDisplay->setFont(QFontDialog::getFont(0, ui->dataDisplay->font()));
    });

    connect(ui->pbSend, &QPushButton::clicked, this, &MainWindow::onSendClicked);
    connect(this, &MainWindow::dataForTransmit, m_serialPort, &SerialPort::send);

    connect(ui->pbSendFile, &QPushButton::clicked, this, &MainWindow::onSendFileClicked);
    connect(ui->pbAsciiTable, &QPushButton::clicked, this, &MainWindow::onAsciiTableClicked);

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

    connect(ui->pbSetMacros, &QPushButton::clicked, this, [this]()
    {
        m_macros->show();
    });

    connect(m_macros, &Macros::macroLabelTextChanged, this, [this](const QString &text)
    {
        ui->pbM1->setText(text);
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
        ui->pbRescan->setEnabled(true);
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

void MainWindow::closeEvent(QCloseEvent *event)
{
    (void)event;

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
