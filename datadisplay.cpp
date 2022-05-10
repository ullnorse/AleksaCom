#include "datadisplay.h"
#include <QScrollBar>
#include <QDebug>
#include <QTime>

DataDisplay::DataDisplay(QWidget *parent)
    : QTextEdit(parent)
{
}

void DataDisplay::displayData(const QByteArray &data)
{
    auto old_position = verticalScrollBar()->value();
    auto text = data;

    moveCursor(QTextCursor::End);

    if (m_displayMode == DisplayMode::ASCII)
    {
        if (m_timestamp)
        {
            setTextColor(QColor(Qt::blue));

            insertPlainText(QTime::currentTime().toString("[hh:mm:ss.zzz] ").toUtf8());

            setTextColor(QColor(Qt::black));
        }

        text.removeIf([](auto &c){return c == '\r';});
        insertPlainText(text);
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

void DataDisplay::setTimestamp(bool timestamp)
{
    m_timestamp = timestamp;
}
