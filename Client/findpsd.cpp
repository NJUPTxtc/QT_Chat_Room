#include "findpsd.h"
#include "ui_findpsd.h"
#include "login.h"

FindPsd::FindPsd(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::FindPsd) {
	ui->setupUi(this);
    socket = new QTcpSocket(this);//创建socket并将其父对象设置为this.这样在FindPsd被销毁时socket也会被销毁
	connect(socket, SIGNAL(readyRead()), this, SLOT(readMsg()));
	connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
	        this, SLOT(displayError(QAbstractSocket::SocketError)));  //发生错误时执行displayError函数
    ui->lineEdit_Name->setFocus();  //将鼠标焦点定位在lineEdit_Name控件上,使用户可以直接在该控件上输入内容
}

FindPsd::~FindPsd() {
	delete this->socket;
	delete ui;
}


void FindPsd::readMsg() {//读取传输过来的消息
    QString data = socket->readAll();//读取所有的数据
    QStringList list = data.split("#");//将data按照"#"进行分割并保存到list中
    if (list[0] == "c" && list[1] == "true") { //找回密码成功
		QString str = list[2];
		qDebug() << list;
        //显示一个消息框标题为"找回密码"、内容为str、按钮为"Ok"
        QMessageBox::information(this, "找回密码", str, QMessageBox::Ok);
        FindPsd::close();//关闭FindPsd窗口
    } else if (list[0] == "c" && list[1] == "false" && list[2] == "0") { //手机号输入错误
		QMessageBox::information(this, "信息提示", "手机号错误,请重新输入!", QMessageBox::Ok);
		ui->lineEdit_Mail->clear();  //清空错误栏
		ui->lineEdit_Mail->setFocus();  //将鼠标重新定位
    } else if (list[0] == "c" && list[1] == "false" && list[2] == "4") {//用户不存在
		QMessageBox::information(this, "信息提示", "用户不存在，请重新输入!", QMessageBox::Ok);
        //清空lineEdit_Name和lineEdit_Mail控件的文本内容
		ui->lineEdit_Name->clear();
        ui->lineEdit_Mail->clear();
		ui->lineEdit_Name->setFocus();
	}

}

void FindPsd::on_pushButton__clicked() {
	socket->abort();   //取消已有的连接
	socket->connectToHost(IP, port); //连接服务器
	//等待连接成功
	if (!socket->waitForConnected(30000)) {
		qDebug() << "连接失败!";
		return;
	}
	qDebug() << "连接成功!";
	QString string1 = ui->lineEdit_Name->text();//用户名
	QString string2 = ui->lineEdit_Mail->text();//手机号
	QString cs = "c";
	QString data = cs + "#" + string1 + "#" + string2;
	socket->write(data.toUtf8());
}
