#include "connectbutton.h"

ConnectButton::ConnectButton(QWidget *parent) : QPushButton(parent)
{
    connect(this, &QPushButton::clicked, this, [this]()
    {
        if (m_state == State::Connect)
        {
            emit connectClicked();
        }
        else if (m_state == State::Disconnect)
        {
            emit disconnectClicked();
            changeToConnect();
        }
    });
}

void ConnectButton::changeToConnect()
{
    m_state = State::Connect;
    setText("Connected");
}

void ConnectButton::changeToDisconnect()
{
    m_state = State::Disconnect;
    setText("Disconnected");
}
