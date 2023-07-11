#include "p2p.h"
#include "ui_p2p.h"
#include "sendfile.h"

P2P::P2P(QWidget *parent) : QDialog(parent), ui(new Ui::P2P) {
	qDebug() << "打开P2P";
	ui->setupUi(this);

    map.load(":/icon/0C89F7B185A1EA66CE5B1148061C5622.jpg");//加载图片将其设置为背景
	map = map.scaled(QSize(221, 381));
	ui->label_pic->setPixmap(map);

	socket = new QTcpSocket();
	server = new QTcpServer();

    //将server对象监听在任意地址的CPort端口上,并通过调用server的listen()函数来启动服务端
    server->listen(QHostAddress::AnyIPv6, CPort);
	qDebug() << CPort;

    //将server的newConnection信号连接到P2P类的Connect()槽函数,以便接收来自其他客户端的连接信号并进行处理
	connect(server, SIGNAL(newConnection()), this, SLOT(Connect())); //接受来自其他客户端的信号并连

    //将socket的readyRead信号连接到P2P类的Read_Data()槽函数,以便接收来自服务器的数据并进行处理
	connect(socket, SIGNAL(readyRead()), this, SLOT(Read_Data())); //接受来自服务器的数据
}

P2P::~P2P() {
	delete ui;
}

void P2P::Port(int number) {
	this->CPort = number;
}

void P2P::Connect() { //连接客户端
	qDebug() << "连接客户端";
	socket = server->nextPendingConnection();
	connect(socket, SIGNAL(readyRead()), this, SLOT(Read_Data())); //获取来自服务器的数据
}

void P2P::Read_Data() {
	QString data = socket->readAll();   //读取服务器发送的数据
	QStringList list = data.split("#");
    QString title = windowTitle();//获取窗口的标题
	qDebug() << data << "到达P2P的读数据";
	if (list[0] == "h") { //点对点消息功能实现
		qDebug() << "发消息           P2P";
		QDateTime time = QDateTime::currentDateTime();
		QString Time = time.toString("yyyy-MM-dd ddd hh:mm:ss ");
		ui->textBrowser_show->append(Time);
		ui->textBrowser_show->append(list[1] + " : " + list[2]);
	} else if (list[0] == "s") { //发送文件获取ip和端口
        YIP = list[3];//获取IP地址
        cport = (list[4]).toInt();//获取端口
		socket->abort();   //取消已有的连接
		socket->connectToHost(YIP, cport + 1); //连接到客户端B
        socket->waitForConnected();//等待连接

        //连接成功后发送一个包含信息"m#"+标题的字符串
		QString ms = "m";
		QString data = ms + "#" + title;
		socket->write(data.toUtf8());
	} else if (list[0] == "y") { //获取ip和端口
        YIP = list[3];//获取IP地址
		cport = (list[4]).toInt();//获取端口
		qDebug() << cport << "  " << YIP;
		socket->abort();   //取消已有的连接
		socket->connectToHost(YIP, cport + 1); //连接到客户端B
		socket->waitForConnected();
		socket->write("66666666");
		qDebug() << "success!'''''''''''''";
	} else if (list[0] == "m") { //发文件
		qDebug() << "发文件";
		Sendfile *m1  = new  Sendfile();
		m1->show();
		m1->setWindowTitle(title);
	}
}

void P2P::on_pushButton_Send_clicked() {
    QString title = windowTitle();//获取窗口的标题
	QString fs;
    QString msg = ui->textEdit_write->toPlainText();//获取用户在文本编辑框中输入的消息
	//点对点私聊
	fs = "h"; //私聊
	QString data = fs + "#" + title + "#" + msg;
	socket->write(data.toUtf8());
	qDebug() << data;
	QDateTime time = QDateTime::currentDateTime();
	QString Time = time.toString("yyyy-MM-dd ddd hh:mm:ss ");
    //将日期和时间、标题和消息追加到显示框中
	ui->textBrowser_show->append(Time);
	ui->textBrowser_show->append(title + " : " + msg);
    ui->textEdit_write->clear();//清空文本编辑框
}

void P2P::on_pushButton_Sendfile_clicked() {
	QString title = windowTitle();
    socket->abort();//取消已有的连接

	socket->connectToHost(IP, port); //连接到服务器
	socket->waitForConnected();
	qDebug() << "1";
	QString ss = "s#";
	QString data = ss + title;
	socket->write(data.toUtf8());
	Sendfile *m1  = new  Sendfile();
	qDebug() << "2";
	m1->show();
	qDebug() << "3";
	m1->setWindowTitle(title);
}


void P2P::on_pushButton_Connect_clicked() {
	qDebug() << this->windowTitle();
	socket->abort();   //取消已有的连接
	//连接服务器
	socket->connectToHost(IP, port);
	//等待连接成功
	if (!socket->waitForConnected(30000)) {
		qDebug() << "Connection failed!";
		return;
	} else {
		qDebug() << "Connect successfully!   P2P!" << windowTitle();
		QString name = windowTitle();
		QString ss = "y#";
		QString data = ss + name;
		socket->write(data.toUtf8());
	}
}

void P2P::on_comboBox_currentIndexChanged(int num) {//换图片
	qDebug() << num;
	if (num == 0) {
        map.load(":/icon/0C89F7B185A1EA66CE5B1148061C5622.jpg");//加载图片并将其保存到map
        map = map.scaled(QSize(221, 381));//将图片调整为指定的大小并将结果保存回map
        ui->label_pic->setPixmap(map);//将调整后的图片设置为用户界面中名为label_pic的标签的像素图
	}
	if (num == 1) {
		map.load(":/icon/1C0A56ED7965651A3C7252C7C8BC4CCD.jpg");
		map = map.scaled(QSize(221, 381));
		ui->label_pic->setPixmap(map);
	}
	if (num == 2) {
		map.load(":/icon/2B589D8C0959A43101DA9A3A21CB713A.jpg");
		map = map.scaled(QSize(221, 381));
		ui->label_pic->setPixmap(map);
	}
	if (num == 3) {
		map.load(":/icon/3E9ABF37028B166DE4B5DCF1AAE1D14F.jpg");
		map = map.scaled(QSize(221, 381));
		ui->label_pic->setPixmap(map);
	}
	if (num == 4) {
		map.load(":/icon/3FAD45AFADED34F9854AA7289533884F.jpg");
		map = map.scaled(QSize(221, 381));
		ui->label_pic->setPixmap(map);
	}
	if (num == 5) {
		map.load(":/icon/468B317D6178792F1728B38937B1D143.jpg");
		map = map.scaled(QSize(221, 381));
		ui->label_pic->setPixmap(map);
	}
	if (num == 6) {
		map.load(":/icon/48C4A5DDA8AAADB312C726576B13B22E.jpg");
		map = map.scaled(QSize(221, 381));
		ui->label_pic->setPixmap(map);
	}
	if (num == 7) {
		map.load(":/icon/723E6423CC8B247CE0D903A5D4688A55.jpg");
		map = map.scaled(QSize(221, 381));
		ui->label_pic->setPixmap(map);
	}
	if (num == 8) {
		map.load(":/icon/865CE62027A75117E225A3104DEBF9F1.jpg");
		map = map.scaled(QSize(221, 381));
		ui->label_pic->setPixmap(map);
	}
	if (num == 9) {
		map.load(":/icon/AEEC264012EB9C71B036E637F0678EFA.jpg");
		map = map.scaled(QSize(221, 381));
		ui->label_pic->setPixmap(map);
	}
	if (num == 10) {
		map.load(":/icon/D3034F351F85E131D25BCDAD3A36EA8C.jpg");
		map = map.scaled(QSize(221, 381));
		ui->label_pic->setPixmap(map);
	}
	if (num == 11) {
		map.load(":/icon/D4B38D4B97344F4FAB55F710B43E5AF1.jpg");
		map = map.scaled(QSize(221, 381));
		ui->label_pic->setPixmap(map);
	}
	if (num == 12) {
		map.load(":/icon/D9C25CA4F04B83068A9E07548C2468B7.jpg");
		map = map.scaled(QSize(221, 381));
		ui->label_pic->setPixmap(map);
	}
}


