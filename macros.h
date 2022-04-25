#ifndef MACROS_H
#define MACROS_H

#include <QWidget>
#include <QAbstractButton>

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
    void buttonClicked(QAbstractButton *button);
    void macroLabelTextChanged(const QString &text);

private:
    Ui::Macros *ui;
};

#endif // MACROS_H
