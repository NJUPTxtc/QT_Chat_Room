#ifndef P2P_H
#define P2P_H

#include "login.h"
#include <QDialog>
#include <QtNetwork>
#include <QTcpSocket>
#include <QTcpServer>
#include "mainwindow.h"
namespace Ui {
	class P2P;
}

class P2P : public QDialog {
		Q_OBJECT

	public:
		explicit P2P(QWidget *parent = nullptr);
		~P2P();
		QString getIP(); //获取本机IP
		QString Cport;
		QString CIP; //自己的IP
		QString YIP; //对方的IP
		int CPort = Mainwindow::CPort + 1; //自己的端口号
		int cport; //对方的端口号
		QString IP = Login::IP;
		int port = Login::Port;
		QString name;
		void Port(int number);

	private slots:
		void on_pushButton_Send_clicked();
		void on_pushButton_Sendfile_clicked();
		void Connect();
		void Read_Data();
		void on_pushButton_Connect_clicked();
		void on_comboBox_currentIndexChanged(int num);//变换壁纸
	private:
		Ui::P2P *ui;
		QTcpServer *server;
		QTcpSocket *socket;
		QPixmap map;
};

#endif // P2P_H
