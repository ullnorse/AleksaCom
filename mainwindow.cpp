#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->statusbar->showMessage("Aleksa");
    ui->statusbar->setStyleSheet("QStatusBar::item { border: 1px solid red; border-radius: 3px; } ");

    auto l1 = new QLabel(this);
    l1->setText("Aleksa");
    l1->setStyleSheet("border: 0; color: blue;");
    auto l2 = new QLabel(this);
    l2->setText("Pavle");
    l2->setStyleSheet("border: 0; color: red;");


//    ui->statusbar->addPermanentWidget();
    ui->statusbar->addPermanentWidget(l1);
    ui->statusbar->addPermanentWidget(l2);


}

MainWindow::~MainWindow()
{
    delete ui;
}

