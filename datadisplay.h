#ifndef DATADISPLAY_H
#define DATADISPLAY_H

#include <QPlainTextEdit>
#include <QByteArray>

class DataDisplay : public QPlainTextEdit
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

public:
    void setMode(DisplayMode mode);

private:
    DisplayMode m_displayMode = DisplayMode::ASCII;
};

#endif // DATADISPLAY_H
