#ifndef FINDPSD_H
#define FINDPSD_H

#include <QDialog>
#include "login.h"

namespace Ui {
	class FindPsd;
}

class FindPsd : public QDialog {
		Q_OBJECT
	public:
		explicit FindPsd(QWidget *parent = 0);
		~FindPsd();
		QString IP = Login::IP;//全局IP
		int port = Login::Port;//全局端口
	private slots:
		void readMsg(); //接受来自服务器的信息
		void on_pushButton__clicked();
	private:
		Ui::FindPsd *ui;
		QTcpSocket *socket;
};

#endif // FINDPSD_H
