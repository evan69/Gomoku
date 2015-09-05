#include "inputip.h"
#include "ui_inputip.h"
#include <QDebug>

inputIp::inputIp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::inputIp),
    mapper(new QSignalMapper(this))
{
    ui->setupUi(this);
    connect(ui->pushButton,SIGNAL(clicked(bool)),mapper,SLOT(map()));
    connect(ui->pushButton_2,SIGNAL(clicked(bool)),mapper,SLOT(map()));
    connect(ui->pushButton_3,SIGNAL(clicked(bool)),mapper,SLOT(map()));
    connect(ui->pushButton_4,SIGNAL(clicked(bool)),mapper,SLOT(map()));
    connect(ui->pushButton_5,SIGNAL(clicked(bool)),mapper,SLOT(map()));
    connect(ui->pushButton_6,SIGNAL(clicked(bool)),mapper,SLOT(map()));
    connect(ui->pushButton_7,SIGNAL(clicked(bool)),mapper,SLOT(map()));
    connect(ui->pushButton_8,SIGNAL(clicked(bool)),mapper,SLOT(map()));
    connect(ui->pushButton_9,SIGNAL(clicked(bool)),mapper,SLOT(map()));
    connect(ui->pushButton_10,SIGNAL(clicked(bool)),mapper,SLOT(map()));
    mapper->setMapping(ui->pushButton,1);
    mapper->setMapping(ui->pushButton_2,2);
    mapper->setMapping(ui->pushButton_3,3);
    mapper->setMapping(ui->pushButton_4,4);
    mapper->setMapping(ui->pushButton_5,5);
    mapper->setMapping(ui->pushButton_6,6);
    mapper->setMapping(ui->pushButton_7,7);
    mapper->setMapping(ui->pushButton_8,8);
    mapper->setMapping(ui->pushButton_9,9);
    mapper->setMapping(ui->pushButton_10,0);
    connect(mapper,SIGNAL(mapped(int)),this,SLOT(keyPressed(int)));
    
    connect(ui->pushButton_12,SIGNAL(clicked(bool)),this,SLOT(addDot()));
    connect(ui->pushButton_11,SIGNAL(clicked(bool)),this,SLOT(ok()));
    connect(ui->pushButton_13,SIGNAL(clicked(bool)),this,SLOT(cancel()));
}

inputIp::~inputIp()
{
    delete ui;
}

void inputIp::keyPressed(int x)
{
    ui->lineEdit->setText(ui->lineEdit->text() + QString::number(x));   
}

void inputIp::addDot()
{
    ui->lineEdit->setText(ui->lineEdit->text() + ".");
}

void inputIp::ok()
{
    ip = ui->lineEdit->text();
    qDebug() << ip;
    close();
}

void inputIp::cancel()
{
    ip = "";
    close();
}
