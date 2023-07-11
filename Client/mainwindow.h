#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDialog>
#include "login.h"
#include <QCloseEvent>
#include <QtNetwork>
#include <QTcpSocket>
#include <QTcpServer>
#include <QtGlobal>     //用于生成随机数
#include <QListWidgetItem>      //列表框空间头文件


namespace Ui {
	class Mainwindow;
}

class Mainwindow : public QDialog {
		Q_OBJECT

	public:
		explicit Mainwindow(QWidget *parent = 0);
		~Mainwindow();
		QString getIP(); //获取本机IP
		QString Cport;
		QString CIP; //自己的IP
		QString YIP; //对方的IP
		static int CPort; //自己的端口号
		int cport; //对方的端口号
		QString IP = Login::IP;
		int port = Login::Port;
		int p;
		QString Name;
	private slots:
		void Read_Data();
		void Connect();
		void on_listWidget_itemDoubleClicked(QListWidgetItem *item);
		void on_pushButton_Send_clicked();
		void on_pushButton_Connect_clicked();

	private:
		void Connect_for_userlist();//连接，得到用户列表
		void closeEvent(QCloseEvent *event); //窗口关闭函数
		Ui::Mainwindow *ui;
		QTcpSocket *socket;
		QTcpServer *server;
};

#endif // MAINWINDOW_H
