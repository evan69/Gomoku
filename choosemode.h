#ifndef CHOOSEMODE_H
#define CHOOSEMODE_H

#include <QDialog>
#include <QKeyEvent>

namespace Ui {
class chooseMode;
}

class chooseMode : public QDialog
{
    Q_OBJECT

public:
    explicit chooseMode(QWidget *parent = 0);
    ~chooseMode();

private:
    Ui::chooseMode *ui;
    void closeEvent(QCloseEvent*);
public:
    int mode;
public slots:
    void ok();

};

#endif // CHOOSEMODE_H
