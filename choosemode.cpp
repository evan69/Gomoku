#include "choosemode.h"
#include "ui_choosemode.h"

chooseMode::chooseMode(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::chooseMode)
{
    ui->setupUi(this);
    mode = -1;
    connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(ok()));
}

chooseMode::~chooseMode()
{
    delete ui;
}

void chooseMode::ok()
{
    mode = (int)(ui->radioButton->isChecked());
    close();
}

void chooseMode::closeEvent(QCloseEvent*)
{
    if(mode == -1) exit(0);
}
