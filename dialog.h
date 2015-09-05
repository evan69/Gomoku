#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QTimer>
#include <QPainter>
#include <QPoint>
#include <QKeyEvent>
#include <vector>
#include "showip.h"
#include "choosemode.h"
#include "inputip.h"
using std::vector;

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

    void initServer();
    void judgeWin();
    bool check(vector<QPoint> v,int i,int j);

signals:
    void getReturn(int,bool);
    
public slots:
    void paintEvent(QPaintEvent*);
    void mousePressEvent(QMouseEvent*);
    void createRoom();
    void connectRoom();
    void acceptConnection();
    void recvMessage();
    void getReady();
    void adjustTime();
    void saveGame();
    void loadGame();
    void regretMove();
    void processReturn(int,bool);
    void exitGame();
    void otherDisconnect();

private:
    void drawChessboard();//画出棋盘
    void drawChessman();//画出棋子
    void startGame();//开始游戏
    QString encodeData();//把vec转化为字符串
    void decodeData(QString);//把字符串解码为vec
    Ui::Dialog *ui;
    QTcpServer* server;
    QTcpSocket* socket;
    QPainter* painter;
    QTimer *timer;
    showIp* dia;
    chooseMode* choose;
    inputIp* input;
    vector<QPoint> selfdata;//己方的数据
    vector<QPoint> otherdata;//对手的数据
    QPoint points[15][15];
    QString ip;
    int mode;//1为主机
    int start;//游戏是否开始，1为自己开始，2为对方开始，3为都开始
    int myTurn;//标记是否是自己的回合，1为是
    int regret;//已悔棋次数
    int accept;//请求是否被接受 -1不接受 1接受
};

#endif // DIALOG_H
