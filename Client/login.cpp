#include "login.h"
#include "ui_login.h"
#include "register.h"
#include "findpsd.h"
#include "mainwindow.h"
#include <QMovie>
#include <QPixmap>

//开始的源头,登陆完成之后跳转到主界面，
//注意,这里有一个全局IP和全局端口(port),除了文件传输和私人聊天是要重新连接的,其他的都是通过这个去连接数据库的
Login::Login(QWidget *parent) : QDialog(parent), ui(new Ui::Login) {
    ui->setupUi(this);//设置当前窗口的UI
	qDebug() << IP << endl;
    QMovie *movie = new QMovie("F:/1.gif");//创建一个QMovie对象用于显示一个gif动画
    socket = new QTcpSocket(this);//创建一个QTcpSocket对象,用于建立TCP连接

    //当socket接收到数据发送readyRead信号,将这个信号与readMsg槽函数连接,readMsg函数会在接收到数据时被调用
	connect(socket, SIGNAL(readyRead()), this, SLOT(readMsg()));
	connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
	        this, SLOT(displayError(QAbstractSocket::SocketError)));  //发生错误时执行displayError函数
}

//用于建立TCP连接的IP和端口
QString Login::IP = "2409:8938:c33:f6b:b1cf:7754:a07:acc";
int Login::Port = 6666;

Login::~Login() {
	delete this->socket;
	delete ui;
}


//这是一个槽函数,用于读取socket接收到的数据
void Login::readMsg() {//读取信息
    //将接收到的数据存储在data字符串中
	QString data = socket->readAll();
    QStringList list = data.split("#");//使用"#"分割字符串,将结果存储在list列表中
	qDebug() << data;
	if (list[0] == "b" && list[2] == "true") {
        //创建一个Mainwindow对象并显示
		Mainwindow *w = new Mainwindow;
		w->setWindowTitle(list[1]);
		w->show();
	} else if (list[0] == "b" && list[1] == "false" && list[2] == '0')
		QMessageBox::information(this, "信息提示", "登录失败,密码错误!", QMessageBox::Ok);
	else if (list[0] == "b" && list[1] == "false" && list[2] == '2')
		QMessageBox::information(this, "信息提示", "用户已在线，拒绝重复登录!", QMessageBox::Ok);
	else if (list[0] == "b" && list[1] == "false" && list[2] == '3')
		QMessageBox::information(this, "信息提示", "用户不存在，请先注册!", QMessageBox::Ok);
	else
		return;
}

//槽函数,点击"注册"按钮时触发的
void Login::on_pushButton_Register_clicked() {
	Register *m = new Register;
    m->setWindowTitle("Regist");//设置了窗口标题
    m->show(); //当点击注册账户时,弹出注册窗口
}
//槽函数,点击"忘记密码"按钮时触发的
void Login::on_pushButton_Forget_clicked() {
	FindPsd *n = new FindPsd;
    n->setWindowTitle("FindPsd");//设置了窗口标题
    n->show();//当点击忘记密码时,显示找回密码界面
}

void Login::on_pushButton_Login_clicked() {
	socket->abort();   //取消已有的连接
    socket->connectToHost(IP, Port); //使用给定的IP地址和端口号连接服务器
	qDebug() << IP << endl;

    //连接服务器超时
	if (!socket->waitForConnected(30000)) {
        //输出错误信息并弹出一个消息框显示错误信息
		qDebug() << "Connection failed!";
		QMessageBox::information(this, tr("错误"), tr("服务器未开启，请先开启服务器！"));

		ui->lineEdit_Name->clear();  //清空账号栏
		ui->lineEdit_Password->clear();  //清空密码栏
		ui->lineEdit_Name->setFocus();  //将鼠标重新定位到账号栏
		return;
	}

    //连接成功
	qDebug() << "Connect successfully!666666!";

    //:从用户界面获取账号栏的文本内容和密码栏的文本内容并存储到变量userName和passward中
	QString userName = ui->lineEdit_Name->text();
	QString passward = ui->lineEdit_Password->text();

	QString bs = "b";

    //将bs、用户名、密码以特定格式拼接为一个整体字符串存储到变量data中
    QString data = bs + "#" + userName + "#" + passward;\

    //将字符串data转换为UTF-8编码并通过socket对象发送给服务器
	socket->write(data.toUtf8());

	if (userName == NULL) { // 输入为空的提示
		QMessageBox::information(this, tr("错误"), tr("账号不能为空！"));
		ui->lineEdit_Name->setFocus();  //将鼠标重新定位到账号栏
	} else if (passward == NULL) {
		QMessageBox::information(this, tr("错误"), tr("密码不能为空！"));
		ui->lineEdit_Name->setFocus();
	}
	ui->lineEdit_Name->clear();  //清空账号栏
	ui->lineEdit_Password->clear();  //清空密码栏
	ui->lineEdit_Name->setFocus();  //将鼠标重新定位到账号栏
}
