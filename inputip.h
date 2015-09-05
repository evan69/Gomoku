#ifndef INPUTIP_H
#define INPUTIP_H

#include <QDialog>
#include <QSignalMapper>

namespace Ui {
class inputIp;
}

class inputIp : public QDialog
{
    Q_OBJECT
    
public:
    explicit inputIp(QWidget *parent = 0);
    ~inputIp();
    QString ip;
public slots:
    void keyPressed(int);
    void addDot();
    void ok();
    void cancel();
private:
    Ui::inputIp *ui;
    QSignalMapper* mapper;
};

#endif // INPUTIP_H
