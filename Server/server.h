#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include <QTcpSocket>
#include <QTcpServer>
#include <QtDebug>
#include <QMessageBox>
#include <QListWidgetItem>      //列表框空间头文件
#include <QFileDialog>          //文件对话框控件
#include <QStringList>          //字符串容器
#include <QDir>                 //目录类头文件
#include <QString>
#include <QList>
#include <QTimer>

#include "mysql.h"

namespace Ui {
class Server;
}

class Server : public QMainWindow
{
    Q_OBJECT

public:
    explicit Server(QWidget *parent = 0);
    ~Server();

    int checkLogIn(QString Name,QString Password);
    bool checkSignUp(QString Name, QString Password, QString Mail);
    int findpwd(QString name,QString Mail);

private slots:
    void Connect();   //连接客户端和服务端
    void ReadMSG();  //读取来自客户端的信息
    void on_pushButton_Flash_clicked();

private:
    Ui::Server *ui;
    QTcpServer* server;
    QTcpSocket* socket;
    QTimer *timer;
    QSqlQuery *query;
};

#endif // MAINWINDOW_H
