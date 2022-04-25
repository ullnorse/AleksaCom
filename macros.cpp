#include "macros.h"
#include "ui_macros.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QSpinBox>
#include <QTimer>

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

    for (auto &timer : m_timers)
    {
        timer.setInterval(1000);
        connect(&timer, &QTimer::timeout, this, []()
        {
            qDebug() << "timer timeout";
        });
    }

    auto checkBoxes = ui->gbTransmitMacros->findChildren<QCheckBox*>(nullptr);
    for (auto &checkBox : checkBoxes)
    {
        connect(checkBox, &QCheckBox::clicked, this, [this](int checked)
        {
            int num = this->sender()->objectName().remove(0, 2).toInt();

            auto timeoutCheckBoxes = ui->bgMacroButtons->findChildren<QCheckBox*>(nullptr);


            qDebug() << "checkbox " << num << " with value " << checked;

            if (checked)
            {
                m_timers[num - 1].start();
            }
            else
            {
                m_timers[num - 1].stop();
            }
        });
    }
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
