#include "mainwindow.h"
#include <QApplication>
#include "ui_mainwindow.h"
#include <QDateTime>
#include "sendfile.h"
#include "p2p.h"

Mainwindow::Mainwindow(QWidget *parent) : QDialog(parent), ui(new Ui::Mainwindow) {
	ui->setupUi(this);
    socket = new QTcpSocket();//实例化了一个QTcpSocket对象用于处理与客户端的通信
    server = new QTcpServer();//实例化了一个QTcpServer对象用于创建和监听TCP服务器
    CIP = getIP();//返回本地主机的IP地址
    qDebug()<<CIP;
    QTime current_time = QTime::currentTime();//获取当前的系统时间
	int second = current_time.second(); //系统当前秒数
    qsrand(second);//使用当前时间的秒数来设置随机数生成器的种子
    Cport = QString::number(qrand() % 1024 + 6000);//生成一个位于6000和7013之间的随机数并将其转换为字符串
	CPort = Cport.toInt();//通过即时的秒来确定Port
    server->listen(QHostAddress::Any, CPort); //让服务器开始监听来自任意IP地址和指定端口CPort的连接
	qDebug() << CPort;

	connect(server, SIGNAL(newConnection()), this, SLOT(Connect())); //接受来自其他客户端的信号并连
	connect(socket, SIGNAL(readyRead()), this, SLOT(Read_Data())); //接受来自服务器的数据
	connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
	        this, SLOT(displayError(QAbstractSocket::SocketError)));  //发生错误时执行displayError函数
}
int Mainwindow::CPort = CPort;

Mainwindow::~Mainwindow() {
	delete ui;
	delete this->socket;
}

//用于处理与客户端的连接,在服务器接受到来自客户端的连接时被调用
void Mainwindow::Connect() { //连接客户端
    socket = server->nextPendingConnection();//接受来自客户端的连接请求,并将该连接分配给socket对象
	connect(socket, SIGNAL(readyRead()), this, SLOT(Read_Data())); //获取来自服务器的数据
}

//获取本机的IPv4地址
QString Mainwindow::getIP() {
	QString localHostName = QHostInfo::localHostName();//本机用户名
	QString IP;
    QHostInfo info = QHostInfo::fromName(localHostName); //获取与该用户名相关的网络信息,包括IP地址

    //遍历这些地址,找到第一个IPv4协议的地址
    foreach (QHostAddress address, info.addresses()) {
        if (address.protocol() == QAbstractSocket::IPv6Protocol) {
             if (address.toString().contains("fe")){
                 continue;
             }
			IP = address.toString();//本机ip地址
            qDebug()<<IP + "     IP地址";
			return IP;
		}
	}
	return NULL;
}

void Mainwindow::closeEvent(QCloseEvent *event) {//在关闭的时候自动退出登陆
	QString title = windowTitle();
	socket->abort();   //取消已有的连接
    socket->connectToHost(IP, port); //连接到指定IP地址和端口的服务器
    socket->waitForConnected();//等待连接完成
	QString ds = "d";
	QString data = ds + "#" + title;
    socket->write(data.toUtf8());//发送数据给服务器
    close();//关闭窗口
}

//读取从服务器发送来的数据并根据不同的数据类型进行相应的处理
void Mainwindow::Read_Data() {
	qDebug() << "到达读取数据";
    QString data = socket->readAll();//读取从服务器发送来的所有数据并将结果保存到data中
    QStringList list = data.split("#");//将data以"#"进行分割,将分割后的结果保存到list中
    QString title = windowTitle();//获取当前窗口的标题
	qDebug() << data;
	if (list[0] == "e") { //加载用户列表
        ui->listWidget->clear();//清除原有的用户列表
		for (int i = 1; i < list.length(); i++) {
			if (list[i] != "end") {
                //将该元素添加到ui->listWidget中显示
				QString str = list[i];
				QListWidgetItem *item = new QListWidgetItem;
				item->setText(str);
				ui->listWidget->addItem(item);
            } else ui->textBrowser->append(list[++i]); //将下一个元素添加到ui->textBrowser中显示
		}
	} else if (list[0] == "f") { //群发消息功能实现
		qDebug() << "群聊      回到客户端";
		if (list[1] != title) {
            QDateTime time = QDateTime::currentDateTime();//获取当前的日期和时间
			QString Time = time.toString("yyyy-MM-dd ddd hh:mm:ss ");
			ui->textBrowser->append(Time);

            //将群发消息的发送者和消息内容添加到ui->textBrowser中显示
			ui->textBrowser->append(list[1] + " : " + list[2]);
		}
	} else if (list[0] == "g") { //私聊
		qDebug() << "私聊      客户端";
        if (list[2] == "1") { //私聊请求被接受
			P2P *p2 = new P2P();
            p2->setWindowTitle(Name);//设置窗口标题
            p2->show();//显示p2窗口
			socket->abort();   //取消已有的连接
			YIP = list[3];
			cport = list[4].toInt();
			socket->connectToHost(YIP, cport); //连接到客户端B
			socket->waitForConnected();//等待连接
			p = 1;

            //构建一条消息data,包括"z"和title,并将其转换为UTF-8编码后发送
			QString ms = "z";
			QString data = ms + "#" + title ;
			socket->write(data.toUtf8());
        } else if (list[2] == "0") { //私聊请求被拒绝
			p = 2;
			QMessageBox::information(this, "信息提示", "该用户不在线，您在输入框中直接输入就可以向他发送离线消息!", QMessageBox::Ok);
		}
	} else if (list[0] == 'z') {//开启P2P
		qDebug() << "到达开启P2P";
		P2P *p3 = new P2P();
		p3->setWindowTitle(list[1]);
		p3->show();
	} else return;
}

//在双击列表中的某一项时开启P2P功能
void Mainwindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item) {//双击开启P2P
	qDebug() << "双击666";
    Name = item->text();//将双击的项的文本内容保存到Name中
    socket->abort();//中断当前的连接
	//连接服务器
	socket->connectToHost(IP, port);
    socket->waitForConnected();//等待连接建立
    QString gs = "g";//表示私聊功能
    QString data = gs + "#" + Name;//创建一个QString对象data,将gs、"#"和Name拼接起来,用于向服务器发送私聊请求
    socket->write(data.toUtf8());//发送给服务器
}

void Mainwindow::on_pushButton_Send_clicked() {
	QString title = windowTitle();
	QString fs;
    QString msg = ui->textEdit->toPlainText();//获取文本编辑框的内容

	if (p != 2) { //群聊
		socket->abort();   //取消已有的连接
		//连接服务器
		socket->connectToHost(IP, port);
		//等待连接成功
        if (!socket->waitForConnected(30000)) { //连接失败
			qDebug() << "Connection failed!";
			return;
		} else
			qDebug() << "Connect successfully!     连接服务器";
		fs = "f"; //群聊
		QString data = fs + "#" + title + "#" + msg;
		socket->write(data.toUtf8());
	} else { //离线消息
		socket->abort();  //取消已有的连接
		//连接服务器
		socket->connectToHost(IP, port);
		//等待连接成功
		if (!socket->waitForConnected(30000)) {
			qDebug() << "Connection failed!";
			return;
		} else
			qDebug() << "Connect successfully!";
        fs = "i"; //表示离线消息
		QString data = fs + "#" + title + "#" + Name + "#" + msg;
		socket->write(data.toUtf8());
	}

    //获取当前日期和时间,并将其格式化为"yyyy-MM-dd ddd
	QDateTime time = QDateTime::currentDateTime();
	QString Time = time.toString("yyyy-MM-dd ddd hh:mm:ss ");

    //向文本浏览器添加日期时间和标题加消息的内容
    ui->textBrowser->append(Time);
	ui->textBrowser->append(title + " : " + msg);
    ui->textEdit->clear();//清空文本编辑框的内容
}

//槽函数,当用户点击名为"pushButton_Connect"的按钮时被调用
void Mainwindow::on_pushButton_Connect_clicked() {//得到用户列表，点击左上角，稍微偏右一些，多点几次就点到了
	qDebug() << this->windowTitle();
	QString title = windowTitle();
	socket->abort();   //取消已有的连接
	//连接服务器
    socket->connectToHost(IP, port);
	//等待连接成功
	if (!socket->waitForConnected(30000)) {
		qDebug() << "Connection failed!";
		return;
	} else {
		qDebug() << "Connect successfully!";
	}

	QString es = "e";
	QString data = es + "#" + title + "#" + CIP + "#" + Cport;
	socket->write(data.toUtf8());
}
