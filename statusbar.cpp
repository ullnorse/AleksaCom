#include "statusbar.h"

#include <QLabel>

StatusBar::StatusBar(QWidget *parent) :
    QStatusBar(parent),
    m_connectionStatus(new QLabel("Disconnected", this)),
    m_rxCount(new QLabel("Rx: 0", this)),
    m_txCount(new QLabel("Tx: 0", this))
{
    m_connectionStatus->setMinimumWidth(150);
    m_rxCount->setMinimumWidth(100);
    m_txCount->setMinimumWidth(100);

    addWidget(m_connectionStatus);
    addWidget(m_rxCount);
    addWidget(m_txCount);
}

void StatusBar::setConnectionStatus(bool connected)
{
    if (connected)
    {
        m_connectionStatus->setText("Connected");
    }
    else
    {
        m_connectionStatus->setText("Disconnected");
    }
}

void StatusBar::incrementRxCount(int num)
{
    int currentCount = m_rxCount->text().remove(0, 4).toInt();
    currentCount += num;
    m_rxCount->setText("Rx: " + QString::number(currentCount));
}

void StatusBar::incrementTxCount(int num)
{
    int currentCount = m_txCount->text().remove(0, 4).toInt();
    currentCount += num;
    m_txCount->setText("Tx: " + QString::number(currentCount));
}
