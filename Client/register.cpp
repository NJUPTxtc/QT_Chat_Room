#include "register.h"
#include "ui_register.h"


Register::Register(QWidget * parent): QDialog(parent), ui(new Ui::Register) {
	ui->setupUi(this);
	tcpSocket = new QTcpSocket(this);
	//连接信号和槽
	connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readMSG()));
	connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
	        this, SLOT(displayError(QAbstractSocket::SocketError)));  //发生错误时执行displayError函数
}


Register::~Register() {
	delete this->tcpSocket;
	delete ui;
}

void Register::readMSG() {
    QString data = tcpSocket->readAll();//读取所有数据
	qDebug() << "1!" << data << endl;
    QStringList list = data.split("#");//将data按照"#"进行分割并保存到list中
    if (list[0] == "a" && list[1] == "true") { //注册成功
		QMessageBox::information(this, "信息提示", "注册成功!", QMessageBox::Ok);
        Register::close();//关闭Register窗口
    } else if (list[0] == "a" && list[1] == "false") //注册失败
		QMessageBox::information(this, "信息提示", "注册失败,用户名已经被注册!", QMessageBox::Ok);
}

void Register::on_pushButton_Register_clicked() {
	tcpSocket->abort();  //取消已有的连接
	tcpSocket->connectToHost(IP, Port);//连接服务器

	//等待连接成功
	if (!tcpSocket->waitForConnected(30000)) {
		qDebug() << "连接失败!666666!";
		return;
	}
	qDebug() << "连接成功!";

	QString string1 = ui->lineEdit_Name->text();//账号
	QString string2 = ui->lineEdit_Password->text();//密码
	QString string3 = ui->lineEdit_Repassword->text();//确认密码
	QString string4 = ui->lineEdit_Mail->text();//电话

	int num = string4.length();
	if (string1 == NULL)
		QMessageBox::information(this, tr("错误"), tr("账号不能为空!"));
	else if (string2 == NULL)
		QMessageBox::information(this, tr("错误"), tr("密码不能为空!"));
	else if (string3 == NULL)
		QMessageBox::information(this, tr("错误"), tr("请确认密码!"));
	else if (string2 != string3) {
		QMessageBox::information(this, tr("错误"), tr("两次密码不一致!"));
		ui->lineEdit_Password->clear();  //清空密码栏
		ui->lineEdit_Password->setFocus();  //将鼠标重新定位到账号栏
	} else if (string4 == NULL)
		QMessageBox::information(this, tr("错误"), tr("请输入邮箱!"));
	QString as = "a";
	QString data = as + "#" + string1 + "#" + string2 + "#" + string4;
//    qDebug()<<"2!"<<data<<endl;
	tcpSocket->write(data.toUtf8());
}

void Register::on_pushButton_Back_clicked() {
	//this->hide();
	Register::close();
}
