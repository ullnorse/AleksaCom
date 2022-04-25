#include "macros.h"
#include "ui_macros.h"

#include <QDebug>

Macros::Macros(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Macros)
{
    ui->setupUi(this);

    setWindowTitle("Macro Settings");
    setAttribute(Qt::WA_DeleteOnClose);

    connect(ui->bgMacroButtons, &QButtonGroup::buttonClicked, this, [this](QAbstractButton *button)
    {
//        qDebug() << button->objectName();
        emit buttonClicked(button);
    });

//    connect(ui->bgMacroButtons, &QButtonGroup::idClicked, this, [](int id)
//    {
//        qDebug() << id;
//    });

    connect(ui->leM1, &QLineEdit::textEdited, this, [this](const QString &text)
    {
        ui->pbM1->setText(text);
        emit macroLabelTextChanged(text);
    });
}

Macros::~Macros()
{
    delete ui;
}
