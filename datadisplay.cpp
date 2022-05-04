#include "datadisplay.h"
#include <QScrollBar>
#include <QDebug>

DataDisplay::DataDisplay(QWidget *parent)
    : QPlainTextEdit(parent)
{
}

void DataDisplay::displayData(const QByteArray &data)
{
    auto old_position = verticalScrollBar()->value();

    moveCursor(QTextCursor::End);

    if (m_displayMode == DisplayMode::ASCII)
    {
        auto d = data;
        d.removeIf([](auto &c){return c == '\r';});
        insertPlainText(d);
    }
    else if (m_displayMode == DisplayMode::HEX)
    {
        insertPlainText(data.toHex(' ').append(" "));
    }

    moveCursor(QTextCursor::End);

    if (!m_scrollingEnabled)
    {
        verticalScrollBar()->setValue(old_position);
    }

}

void DataDisplay::setScrolling(bool scrollingEnabled)
{
    if (scrollingEnabled)
    {
        m_scrollingEnabled = true;
        verticalScrollBar()->setValue(verticalScrollBar()->maximum());
    }
    else
    {
        m_scrollingEnabled = false;
    }
}

void DataDisplay::setMode(DisplayMode mode)
{
    m_displayMode = mode;
}
