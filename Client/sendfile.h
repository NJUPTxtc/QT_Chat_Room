#ifndef SENDFILE_H
#define SENDFILE_H

#include <QDialog>
#include <QtNetwork>
#include <QFile>
#include <QFileDialog>
#include <QTimer>
#include "mainwindow.h"

namespace Ui {
	class Sendfile;
}

class Sendfile : public QDialog {
		Q_OBJECT


	public:
		explicit Sendfile(QWidget *parent = 0);
		~Sendfile();
		void send_init();
		void openFile();
		void startTransfer();
		void send();
		void receive_init();
		void updateServerProgress();
		int flag = 0;
        QString YouIP = "2408:844c:700:16f0:39fe:96d0:3f2f:12ba";

		QByteArray line;
		QFile *send_localFile;
		QFile *receive_localFile;
        qint64 send_totalBytes;//发送的总字节数
		qint64 receive_totalBytes;
        qint64 send_bytesWritten;//已经发送的字节数
        qint64 send_bytesToWrite;//还剩下多少字节要发送
		qint64 receive_bytesReceived;
		qint64 receive_fileNameSize;
        qint64 send_loadSize;//每次发送的数据块的大小
		QString send_fileName;
		QString receive_fileName;
		QByteArray send_outBlock;
		QByteArray receive_inBlock;
		QUdpSocket *send_udpSocket;
		QUdpSocket *receive_udpSocket;
		QHostAddress send_ip;
		QHostAddress receive_ip;
		QTimer *m_pTimer;
	private slots:
		void handletimeout();
		void send_readmessage();
		void processPendingDatagram();
		void on_pushButton_Open_clicked();
		void on_pushButton_Send_clicked();
	private:
		Ui::Sendfile *ui;
};

#endif // SENDFILE_H




