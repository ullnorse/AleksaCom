#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <QStatusBar>
#include <QLabel>

class StatusBar : public QStatusBar
{
    Q_OBJECT
public:
    StatusBar(QWidget *parent = nullptr);

    void setConnectionStatus(bool connected);
    void incrementRxCount(int num);
    void incrementTxCount(int num);

private:
    QLabel *m_connectionStatus;
    QLabel *m_rxCount;
    QLabel *m_txCount;
};

#endif // STATUSBAR_H
