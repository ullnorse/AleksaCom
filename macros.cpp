#include "macros.h"
#include "ui_macros.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QSpinBox>
#include <QTimer>
#include <QRegularExpression>
#include <QFileDialog>
#include <QStandardPaths>
#include <QTextStream>
#include <QStringList>

Macros::Macros(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Macros)
{
    ui->setupUi(this);

    setWindowTitle("Macro Settings");

    auto macroButtons = ui->gbTransmitMacros->findChildren<QPushButton*>(nullptr);
    for (auto &button : macroButtons)
    {
        connect(button, &QPushButton::clicked, this, &Macros::onMacroButtonClicked);
    }

    auto macroButtonNameLineEdits = ui->gbTransmitMacros->findChildren<QLineEdit*>(QRegularExpression("leM"));
    for (auto &lineEdit : macroButtonNameLineEdits)
    {
        connect(lineEdit, &QLineEdit::textEdited, this, &Macros::onMacroButtonNameEdited);
    }

    for (int i = 0; i < static_cast<int>(m_timers.size()); i++)
    {
        auto timer = &m_timers[i];

        timer->setInterval(1000);
        timer->setObjectName("timer" + QString::number(i + 1));

        connect(timer, &QTimer::timeout, this, [this]()
        {
            auto lineEdit = ui->gbTransmitMacros->findChild<QLineEdit*>("le" + this->sender()->objectName().remove(0, 5));

            emit macroText(lineEdit->text().append('\n'));
        });
    }

    auto checkBoxes = ui->gbTransmitMacros->findChildren<QCheckBox*>(nullptr);
    for (auto &checkBox : checkBoxes)
    {
        connect(checkBox, &QCheckBox::clicked, this, [this](int checked)
        {
            int num = this->sender()->objectName().remove(0, 2).toInt();

            auto timer = &m_timers[num - 1];
            auto spinBox = ui->gbTransmitMacros->findChild<QSpinBox*>("sb" + QString::number(num));

            if (checked)
            {
                timer->setInterval(spinBox->value());
                timer->start();
            }
            else
            {
                timer->stop();
            }
        });
    }

    connect(this, &Macros::macroLabelTextChanged, this, [this](const QString &buttonName, const QString &text)
    {
        auto button = ui->gbTransmitMacros->findChild<QPushButton*>(buttonName);
        button->setText(text);
    });

    connect(ui->pbSave, &QPushButton::clicked, this, &Macros::onSaveButtonClicked);
    connect(ui->pbLoad, &QPushButton::clicked, this, &Macros::onLoadButtonClicked);
}

Macros::~Macros()
{
    delete ui;
}

void Macros::onMacroButtonClicked()
{
    auto lineEdit = ui->gbTransmitMacros->findChild<QLineEdit*>("le" + this->sender()->objectName().remove(0, 3));

    emit macroText(lineEdit->text().append('\n')); //TODO: remove new line and replace with parsing macro correctly
}

void Macros::onMacroButtonNameEdited(const QString &text)
{
    auto lineEdit = qobject_cast<QLineEdit*>(sender());
    auto buttonName = "pbM" + lineEdit->objectName().remove(0, 3);

    emit macroLabelTextChanged(buttonName, text);
}

void Macros::onSaveButtonClicked()
{
#ifdef Q_OS_WIN
    auto dir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
#elif defined Q_OS_LINUX
        auto dir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
#endif

    auto fileName = QFileDialog::getSaveFileName(nullptr,
                                                 "Select log file",
                                                 dir,
                                                 "Terminal macro files (*.tmf)",
                                                 0,
                                                 QFileDialog::Option::DontConfirmOverwrite);

    if (!fileName.isEmpty())
    {
        auto file = QFile(fileName);

        if (file.open(QIODeviceBase::OpenModeFlag::WriteOnly))
        {
            saveMacroFile(&file);
        }
    }
}

void Macros::onLoadButtonClicked()
{
#ifdef Q_OS_WIN
    auto dir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
#elif defined Q_OS_LINUX
        auto dir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
#endif

    auto fileName = QFileDialog::getOpenFileName(nullptr,
                                                 "Select log file",
                                                 dir,
                                                 "Terminal macro files (*.tmf)",
                                                 0,
                                                 QFileDialog::Option::DontConfirmOverwrite);

    if (!fileName.isEmpty())
    {
        auto file = QFile(fileName);

        if (file.open(QIODeviceBase::OpenModeFlag::ReadOnly))
        {
            loadMacroFile(&file);
        }
    }
}

void Macros::saveMacroFile(QFile *file)
{
    auto stream = QTextStream(file);

    stream << "# Terminal macro file v2\n";

    for (int i = 0; i < 24; i++)
    {
        auto button = ui->gbTransmitMacros->findChild<QPushButton*>("pbM" + QString::number(i + 1));
        auto macroName = button->text();

        auto macro = ui->gbTransmitMacros->findChild<QLineEdit*>("le" + QString::number(i + 1));
        auto command = macro->text();

        stream << macroName << "\n" << command << "\n";
    }
}

void Macros::loadMacroFile(QFile *file)
{
    auto stream = QTextStream(file);

    // skip first line
    auto firstLine = stream.readLine();
    QString buttonText;
    QString macroName;
    QStringList buttonTexts;
    QStringList macroNames;

    while (stream.readLineInto(&buttonText) && stream.readLineInto(&macroName))
    {
        buttonTexts << buttonText;
        macroNames << macroName;
    }

    for (int i = 0; i < buttonTexts.size(); i++)
    {
        auto button = ui->gbTransmitMacros->findChild<QPushButton*>("pbM" + QString::number(i + 1));
        auto macroName = ui->gbTransmitMacros->findChild<QLineEdit*>("leM" + QString::number(i + 1));
        button->setText(buttonTexts[i]);
        macroName->setText(buttonTexts[i]);
    }

    for (int i = 0; i < macroNames.size(); i++)
    {
        auto macro = ui->gbTransmitMacros->findChild<QLineEdit*>("le" + QString::number(i + 1));
        macro->setText(macroNames[i]);
    }
}
