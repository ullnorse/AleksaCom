#ifndef DATADISPLAY_H
#define DATADISPLAY_H

#include <QTextEdit>
#include <QByteArray>

class DataDisplay : public QTextEdit
{
    Q_OBJECT
public:
    enum class DisplayMode
    {
        ASCII,
        HEX
    };

    DataDisplay(QWidget *parent = nullptr);

public slots:
    void displayData(const QByteArray &data);
    void setScrolling(bool scrollingEnabled);
    void setTimestamp(bool timestamp);

public:
    void setMode(DisplayMode mode);

private:
    DisplayMode m_displayMode = DisplayMode::ASCII;
    bool m_scrollingEnabled = true;
    bool m_timestamp = false;
};

#endif // DATADISPLAY_H
