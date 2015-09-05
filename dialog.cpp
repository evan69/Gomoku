#include "dialog.h"
#include "ui_dialog.h"
#include <QDebug>
#include <QMessageBox>
#include <QTime>
#include <QDate>
#include <QFile>
#include <QFileDialog>
#include <string>
#include <fstream>
using std::string;

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog),
    timer(new QTimer(this)),
    choose(new chooseMode(this))
{
    ui->setupUi(this);
    initServer();
    mode = -1;
    start = 0;
    myTurn = -1;
    regret = 0;
    accept = 0;
    ui->pushButton_2->setEnabled(0);
    ui->pushButton_3->setEnabled(0);
    ui->pushButton_4->setEnabled(0);
    ui->pushButton_5->setEnabled(0);
    ui->pushButton_6->setEnabled(0);
    ui->lcdNumber->display(20);
    for(int i = 0;i < 15;i++)
    {
        for(int j = 0;j < 15;j++)
        {
            points[i][j] = QPoint(24*j+20,24*i+20);
        }
    }
    
    this->hide();
    choose->show();
    choose->exec();
    choose->hide();
    this->show();
    mode = (int)(choose->mode);
    if(mode)
    {
        connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(createRoom()));
        ui->pushButton->setText("创建房间");
        this->setWindowTitle("游戏主机");
        myTurn = 1;
    }
    else
    {
        connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(connectRoom()));
        ui->pushButton->setText("连接房间");
        this->setWindowTitle("游戏客户机");
        myTurn = 0;
    }
    connect(timer,SIGNAL(timeout()),this,SLOT(adjustTime()));
    connect(ui->pushButton_2,SIGNAL(clicked(bool)),this,SLOT(getReady()));
    connect(ui->pushButton_3,SIGNAL(clicked(bool)),this,SLOT(regretMove()));
    connect(ui->pushButton_4,SIGNAL(clicked(bool)),this,SLOT(exitGame()));
    connect(ui->pushButton_5,SIGNAL(clicked(bool)),this,SLOT(saveGame()));
    connect(ui->pushButton_6,SIGNAL(clicked(bool)),this,SLOT(loadGame()));
    connect(ui->pushButton_7,SIGNAL(clicked(bool)),this,SLOT(close()));
    
    connect(this,SIGNAL(getReturn(int,bool)),this,SLOT(processReturn(int,bool)));
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::initServer()
{
    this->server =new QTcpServer;
    this->server->listen(QHostAddress::Any,20000);
    QObject::connect(this->server,SIGNAL(newConnection()),this,SLOT(acceptConnection()));
}

void Dialog::createRoom()
{
    dia = new showIp(this);
    this->hide();
    dia->show();
    dia->exec();
    dia->hide();
    ip = dia->ip;
    this->show();
    if(dia->flag == 1) ui->pushButton->setEnabled(0);
    ui->label_3->setText("创建房间成功，等待其他玩家进入");
}

void Dialog::connectRoom()
{
    this->socket = new QTcpSocket;
    input = new inputIp(this);
    this->hide();
    input->show();
    input->exec();
    input->hide();
    ip = input->ip;
    this->show();    
    this->socket->connectToHost(QHostAddress(ip),20000);//需要输入ip
    QObject::connect(this->socket,SIGNAL(readyRead()),this,SLOT(recvMessage()));
    QObject::connect(this->socket,SIGNAL(disconnected()),this,SLOT(otherDisconnect()));
    ui->pushButton->setEnabled(0);
    ui->pushButton_2->setEnabled(1);
    ui->label_3->setText("已经进入房间，请点击准备就绪");
}

void Dialog::acceptConnection()
{
    this->socket = this->server->nextPendingConnection();
    QObject::connect(this->socket,SIGNAL(readyRead()),this,SLOT(recvMessage()));
    if(mode) ui->label_3->setText("有玩家已经进入房间，请点击准备就绪");
    ui->pushButton_2->setEnabled(1);                                       
}

void Dialog::otherDisconnect()
{
    QMessageBox::information(this,"消息","连接断开，游戏结束",QMessageBox::Ok);
    exit(0);
}

void Dialog::recvMessage()
{
    QString info;
    info += this->socket->readAll();
    if(info == "ready")
    {
        if(start == 0)
        {
            start = 2;
            ui->label_3->setText("对方已经准备就绪");
        }
        if(start == 1)
        {
            start = 3;
            socket->write("start");
            startGame();          
        }
        return;
    }
    if(info == "start")
    {
        start = 3;
        startGame();   
        return;
    }
    if(info == "timeout")
    {
        myTurn = 1;
        ui->label_3->setText("对方回合超时,现在是您的回合");
        timer->start(1000);
    }
    if(info == "regret")
    {
        timer->stop();
        int ret = QMessageBox::information(this,"消息","对方请求悔棋,是否同意?",QMessageBox::Yes|QMessageBox::No);
        if(ret == QMessageBox::No) socket->write("refuseRegret");
        else
        {
            selfdata.pop_back();
            otherdata.pop_back();
            socket->write("acceptRegret");
        }
        timer->start(1000);
    }
    if(info == "exit")
    {
        timer->stop();
        int ret = QMessageBox::information(this,"消息","对方请求退出游戏,是否同意?",QMessageBox::Yes|QMessageBox::No);
        if(ret == QMessageBox::No) socket->write("refuseExit");
        else
        {
            socket->write("acceptExit");
            otherDisconnect();
        }
        timer->start(1000);
    }
    if(info == "load")
    {
        timer->stop();
        int ret = QMessageBox::information(this,"消息","对方请求加载残局,是否同意?",QMessageBox::Yes|QMessageBox::No);
        if(ret == QMessageBox::No) socket->write("refuseLoad");
        else
        {
            socket->write("acceptLoad");
        }
        timer->start(1000);
    }
    if(info == "acceptRegret") emit getReturn(0,1);
    if(info == "refuseRegret") emit getReturn(0,0);
    if(info == "acceptExit") emit getReturn(1,1);
    if(info == "refuseExit") emit getReturn(1,0);
    if(info == "acceptLoad") emit getReturn(2,1);
    if(info == "refuseLoad") emit getReturn(2,0);
    if(info[0] >= '0' && info[0] <= '9')
    {
        int a = (int)(info.toInt() / 100);
        int b = info.toInt() - 100*a;
        otherdata.push_back(QPoint(a,b));
        myTurn = 1; 
        ui->pushButton_3->setEnabled(1);
        ui->pushButton_4->setEnabled(1);
        ui->pushButton_5->setEnabled(1);
        ui->pushButton_6->setEnabled(1);  
        ui->label_3->setText("您的回合");     
        update();
        judgeWin();
    }
    if(info.toStdString()[0] == 'd' && info.toStdString()[1] == 'a')
    {
        decodeData(info);
        judgeWin();
    }
    update();
}

void Dialog::getReady()
{
    ui->label_3->setText("您已经准备就绪");
    if(start == 0)
    {
        socket->write("ready");
        start = 1;
    }
    if(start == 2)
    {
        socket->write("start");
        start = 3;        
        startGame();
    }
    ui->pushButton_2->setEnabled(0);
}

void Dialog::startGame()
{
    ui->label_3->setText("游戏开始");
    timer->start(1000);
    if(mode)
    {
        ui->pushButton_3->setEnabled(1);
        ui->pushButton_4->setEnabled(1);
        ui->pushButton_5->setEnabled(1);
        ui->pushButton_6->setEnabled(1);         
    } 
}

void Dialog::adjustTime()
{
    if(myTurn)
    {
        ui->lcdNumber->display(ui->lcdNumber->intValue() - 1);
        ui->lcdNumber_2->display(ui->lcdNumber_2->intValue() + 1);
        if(ui->lcdNumber->intValue() <= 0)
        {
            socket->write("timeout");
            myTurn = 0;
            ui->lcdNumber->display(20);
            ui->label_3->setText("回合超时");
        }
    }
}

void Dialog::regretMove()
{
    timer->stop();
    if(regret >= 2)
    {
        ui->label_3->setText("无法悔棋,至多连续两次悔棋！");
        return;
    }
    if(selfdata.size() < 1 || otherdata.size() < 1)
    {
        ui->label_3->setText("棋子不够，无法悔棋！");
        return;
    }
    socket->write("regret");
    ui->label_3->setText("已经发出请求，正在等待对方同意");
}

void Dialog::exitGame()
{
    timer->stop(); 
    int ret = QMessageBox::information(this,"消息","你真的要退出房间吗?",QMessageBox::Ok,QMessageBox::No);
    if(ret == QMessageBox::No) return;    
    socket->write("exit");
}

void Dialog::saveGame()
{  
    QDateTime tmp = QDateTime::currentDateTime();
    QString fileName = tmp.toString("yyyy-MM-dd_hhmmss")+".txt";
    QFile gameFile;
    gameFile.setFileName(fileName);
    if(gameFile.open(QIODevice::ReadWrite))
    {
        QTextStream out(&gameFile);
        out << encodeData();
        out.flush();
    }
    gameFile.close();
    ui->label_3->setText("保存残局成功");
}

void Dialog::loadGame()
{
    socket->write("load");
    timer->stop();
    ui->label_3->setText("已向对方发送加载本地残局的请求");
}

void Dialog::processReturn(int type,bool result)
{
    if(type == 0)
    {        
        if(result == 1)
        {
            selfdata.pop_back();
            otherdata.pop_back();
            regret++;
            ui->lcdNumber->display(20);                                
            ui->label_3->setText("对方已同意悔棋");
        }
        else
        {
            ui->label_3->setText("对方不同意悔棋");
        }
    }
    if(type == 1)
    {
        if(result == 1)
        {
            otherDisconnect();     
        }
        else
        {
            ui->label_3->setText("对方不同意结束游戏");
        }
    }
    if(type == 2)
    {
        if(result == 1)
        {
            QFile oldData(QFileDialog::getOpenFileName(this,"对方已经同意请求，请打开残局文件"));
            oldData.open(QIODevice::ReadWrite);
            decodeData(oldData.readAll());
            socket->write(encodeData().toStdString().c_str());
            myTurn = 1;                
        }
        else
        {
            ui->label_3->setText("对方不同意加载残局");          
        }    
    }
    timer->start(1000);
    update();
}

void Dialog::paintEvent(QPaintEvent *)
{
    painter = new QPainter(this);
    drawChessboard();
    drawChessman();
}

void Dialog::drawChessboard()
{
    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::yellow);
    painter->drawRect(QRect(8,8,24*15,24*15));
    //绘制棋盘背景
    painter->setPen(Qt::black);
    for(int i = 0;i < 15;i++)
    {
        painter->drawLine(points[i][0],points[i][14]);
        painter->drawLine(points[0][i],points[14][i]);
    }
    painter->setBrush(Qt::black);
    //绘制方格线
    painter->drawEllipse(points[3][3],3,3);
    painter->drawEllipse(points[3][11],3,3);
    painter->drawEllipse(points[11][3],3,3);
    painter->drawEllipse(points[11][11],3,3);
    painter->drawEllipse(points[7][7],3,3);
    //绘制五个特殊点
}

void Dialog::drawChessman()
{
    painter->setPen(Qt::NoPen);
    if(selfdata.size() > 0)
    {
        if(mode) painter->setBrush(Qt::black);
        else painter->setBrush(Qt::white);
        for(vector<QPoint>::iterator it = selfdata.begin();it != selfdata.end();++it)
        {
            painter->drawEllipse(points[(*it).x()][(*it).y()],11,11);
        }
    }
    if(otherdata.size() > 0)
    {
        if(mode) painter->setBrush(Qt::white);
        else painter->setBrush(Qt::black);
        for(vector<QPoint>::iterator it = otherdata.begin();it != otherdata.end();++it)
        {
            painter->drawEllipse(points[(*it).x()][(*it).y()],11,11);
        }
    }
}


void Dialog::mousePressEvent(QMouseEvent *e)
{
    if(start < 3) return;
    if(myTurn == 0) return;
    int x = e->x();
    int y = e->y();
    for(int i = 0;i < 15;i++)
    {
        for(int j = 0;j < 15;j++)
        {
            if(abs(x - points[i][j].x()) < 12 && abs(y - points[i][j].y()) < 12)
            {
                if(std::find(otherdata.begin(),otherdata.end(),QPoint(i,j))!= otherdata.end())
                    return;
                if(selfdata.size() == 0 ||
                        std::find(selfdata.begin(),selfdata.end(),QPoint(i,j)) == selfdata.end())
                {
                    selfdata.push_back(QPoint(i,j));
                    ui->lcdNumber->display(20);
                    ui->pushButton_3->setEnabled(0);
                    ui->pushButton_4->setEnabled(0);
                    ui->pushButton_5->setEnabled(0);
                    ui->pushButton_6->setEnabled(0);     
                    ui->label_3->setText("对方的回合"); 
                    QByteArray array;
                    array.append(QString::number(100*i+j));
                    socket->write(array);
                    myTurn = 0;
                    regret = 0;               
                    update();
                    judgeWin();
                }
            }
        }
    }
}

void Dialog::judgeWin()
{
    for(int i = 2;i < 13;i++)
    {
        for(int j = 2;j < 13;j++)
        {
            if(check(selfdata,i,j))
            {                
                myTurn = 0;
                timer->stop();
                QMessageBox::information(this,"胜利",
                "您获胜了!共用时" + QString::number(ui->lcdNumber_2->intValue()) + "秒",QMessageBox::Ok);
                return;
            }
            if(check(otherdata,i,j))
            {                
                myTurn = 0;
                timer->stop();
                QMessageBox::information(this,"失败",
                "您失败了!共用时" + QString::number(ui->lcdNumber_2->intValue()) + "秒",QMessageBox::Ok);
                return;
            }
        }
    }
}

bool Dialog::check(vector<QPoint> vec,int i,int j)
{
    QPoint tmp[4] = {QPoint(1,0),QPoint(0,1),QPoint(1,1),QPoint(1,-1)};
    for(int r = 0;r < 4;r++)
    {
        int g = 1;
        for(int s = -2;s < 3;s++)
        {
            QPoint e = QPoint(tmp[r].x() * s,tmp[r].y() * s);
            if(std::find(vec.begin(),vec.end(),QPoint(i,j)+e)== vec.end())
            {
                g = 0;
                break;
            }           
        }
        if(g == 1) return 1;
    }
    return 0;
}

QString Dialog::encodeData()
{
    string gameData = "data*";
    for(vector<QPoint>::iterator it = selfdata.begin();it != selfdata.end();++it)
    {
        if(it->x() < 10) gameData += (char)(it->x() + '0');
        else
        {
            gameData += (char)(it->x() + 'A' - 10);
        }
        if(it->y() < 10) gameData += (char)(it->y() + '0');
        else
        {
            gameData += (char)(it->y() + 'A' - 10);
        }
    }
    gameData += '*';
    for(vector<QPoint>::iterator it = otherdata.begin();it != otherdata.end();++it)
    {
        if(it->x() < 10) gameData += (char)(it->x() + '0');
        else
        {
            gameData += (char)(it->x() + 'A' - 10);
        }
        if(it->y() < 10) gameData += (char)(it->y() + '0');
        else
        {
            gameData += (char)(it->y() + 'A' - 10);
        }
    }
    gameData += '*';
    return QString::fromStdString(gameData);    
}

void Dialog::decodeData(QString s)
{
    string str = s.toStdString();
    int i = 0;
    selfdata.clear();
    otherdata.clear();
    while(1)
    {
        if(str[i] == '*') break;
        i++;
    }
    i++;
    while(str[i] != '*')
    {
        int x = 0,y = 0;
        if(str[i] >= '0' && str[i] <= '9') x = str[i] - '0';
        else x = str[i] - 'A' + 10;
        i++;
        if(str[i] >= '0' && str[i] <= '9') y = str[i] - '0';
        else y = str[i] - 'A' + 10;        
        i++;
        otherdata.push_back(QPoint(x,y));
    }
    i++;
    while(str[i] != '*')
    {
        int x = 0,y = 0;
        if(str[i] >= '0' && str[i] <= '9') x = str[i] - '0';
        else x = str[i] - 'A' + 10;
        i++;
        if(str[i] >= '0' && str[i] <= '9') y = str[i] - '0';
        else y = str[i] - 'A' + 10;        
        i++;
        selfdata.push_back(QPoint(x,y));
    }
}
