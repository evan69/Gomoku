#include "showip.h"
#include "ui_showip.h"
#include <QDebug>
showIp::showIp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::showIp)
{
    flag = 0;
    QString localhostName = QHostInfo::localHostName();
    QList<QHostAddress> addr = QHostInfo::fromName(localhostName).addresses();    
    for(QList<QHostAddress>::iterator it = addr.begin();it != addr.end();++it)
    {
        if(it->protocol() == QAbstractSocket::IPv6Protocol || it->toString() == "127.0.0.1") continue;
        qDebug() << it->toString();
        ip = it->toString();
        break;
    }
    //ip = "127.0.0.1";
    ui->setupUi(this);
    ui->lineEdit->setText(ip);
    connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(ok()));
    connect(ui->pushButton_2,SIGNAL(clicked(bool)),this,SLOT(cancel()));
}

showIp::~showIp()
{
    delete ui;
}

void showIp::ok()
{
    flag = 1;
    close();
}

void showIp::cancel()
{
    flag = 0;
    close();
}
