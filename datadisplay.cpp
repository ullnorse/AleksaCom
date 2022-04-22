#include "datadisplay.h"

#include <QTime>
#include <QDebug>

DataDisplay::DataDisplay(QWidget *parent)
    : QPlainTextEdit(parent)
{
    qDebug() << parent->objectName();
}

void DataDisplay::displayData(const QByteArray &data)
{
    moveCursor(QTextCursor::End);

    if (m_displayMode == DisplayMode::ASCII)
    {
        insertPlainText(data);
    }
    else if (m_displayMode == DisplayMode::HEX)
    {
        insertPlainText(data.toHex(' '));
        insertPlainText(" ");
    }

    moveCursor(QTextCursor::End);
}

void DataDisplay::setMode(DisplayMode mode)
{
    m_displayMode = mode;
}
