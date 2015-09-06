#include "showip.h"
#include "ui_showip.h"
#include <QDebug>
showIp::showIp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::showIp)
{
    flag = 0;
    /*
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
    //~~~~~
    */
    QHostAddress result;
       QString hostname = QHostInfo::localHostName(), ipAddress;
       QHostInfo hostInfo = QHostInfo::fromName(hostname);
       QTcpSocket* trial = new QTcpSocket(this);
       trial->connectToHost("baidu.com", 80);
       if (trial->waitForConnected(5000))
       {
           result = trial->localAddress();
           trial->abort();
           delete trial;
           trial = 0;
       }
       else
       {
           QList<QHostAddress> listAddress = hostInfo.addresses();
           if (! listAddress.isEmpty())
           {
               for (int i = 0; i < listAddress.count(); i++)
               {
                   QHostAddress address = listAddress.at(i);
                   if (address.protocol() == QAbstractSocket::IPv4Protocol
                       && address != QHostAddress(QHostAddress::LocalHost))
                   {
                       result = address;
                       break;
                   }
               }
           }
       }
       ip = result.toString();
       
    //~~~~~
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
