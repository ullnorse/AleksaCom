#include "datadisplay.h"

DataDisplay::DataDisplay(QWidget *parent)
    : QPlainTextEdit(parent)
{
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
        insertPlainText(data.toHex(' ').append(" "));
    }

    moveCursor(QTextCursor::End);
}

void DataDisplay::setMode(DisplayMode mode)
{
    m_displayMode = mode;
}
