#ifndef CONNECTBUTTON_H
#define CONNECTBUTTON_H

#include <QPushButton>

class ConnectButton : public QPushButton
{
    Q_OBJECT

    enum class State
    {
        Connect,
        Disconnect
    };

public:
    ConnectButton(QWidget *parent = nullptr);

signals:
    void connectClicked();
    void disconnectClicked();

public slots:
    void changeToConnect();
    void changeToDisconnect();

private:
    State m_state = State::Connect;
};

#endif // CONNECTBUTTON_H
