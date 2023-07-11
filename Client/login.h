#ifndef TCLIENT_H
#define TCLIENT_H

#include <QMainWindow>
#include <QMessageBox>
#include <QString>
#include <iostream>
#include <QtDebug>
#include <QTcpSocket>
#include <QtNetwork/QNetworkInterface>
#include <QtNetwork/QHostAddress>
#include <QList>

namespace Ui {
    class Login;
}

class Login : public QDialog {
        Q_OBJECT

    public:
        explicit Login(QWidget *parent = 0);
        ~Login();
        static QString IP;//全局IP
        static int Port;//全局port
    private:
        Ui::Login *ui;
        QTcpSocket *socket;
    private slots:
        void readMsg(); //接受来自服务器的信息
        void on_pushButton_Register_clicked();
        void on_pushButton_Forget_clicked();
        void on_pushButton_Login_clicked();
};

#endif // TCLIENT_H
