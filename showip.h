#ifndef SHOWIP_H
#define SHOWIP_H

#include <QDialog>
#include <QtNetwork>

namespace Ui {
class showIp;
}

class showIp : public QDialog
{
    Q_OBJECT

public:
    explicit showIp(QWidget *parent = 0);
    ~showIp();

private:
    Ui::showIp *ui;

public:
    QString ip;
    int flag;

public slots:
    void ok();
    void cancel();
};

#endif // SHOWIP_H
