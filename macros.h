#ifndef MACROS_H
#define MACROS_H

#include <QWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QTimer>

#include <array>

namespace Ui {
class Macros;
}

class Macros : public QWidget
{
    Q_OBJECT

public:
    explicit Macros(QWidget *parent = nullptr);
    ~Macros();

signals:
    void macroLabelTextChanged(const QString &text);
    void macroText(const QString &text);

public slots:
    void onMacroButtonClicked();

private:
    Ui::Macros *ui;

private:
    std::array<QTimer, 24> m_timers;
};

#endif // MACROS_H