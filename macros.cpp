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

            auto timeoutCheckBoxes = ui->bgMacroButtons->findChildren<QCheckBox*>(nullptr);
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
