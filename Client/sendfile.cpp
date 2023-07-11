#include "sendfile.h"
#include "ui_sendfile.h"
#include "mainwindow.h"
#define TIMEOUT (500)

/*
    文件传输的原理是在文件传输里面创建了两个端口，一个是服务端，一个是客户端。
    在我们打开文件传输的界面的时候，文件传输的客户端通过之前，就已经写在头文件里面的IP地址，去连接另一个服务器。
    我们把两个用户称为A和B。若a单打开的文件不为空，则进入传输文件。不断读取文件名的大小和传输文件的数据。
    服务端则对传输过来的信息进行判别，如果是文件，那么就在本地创建一个文件的缓冲区。
    如果是内容的话，则在缓冲区不断添加接收的内容。与客户端不断进行交互，当信息传输完毕的时候就结束传输。
*/

Sendfile::Sendfile(QWidget *parent) : QDialog(parent),
	ui(new Ui::Sendfile) {
	ui->setupUi(this);
	send_init();
	receive_init();
}


Sendfile::~Sendfile() {
	delete ui;
}

//文件传输--发送文件handletimeout()

void Sendfile::send_init() {
	send_loadSize = 50 * 1024;
	send_totalBytes = 0;
	send_bytesWritten = 0;
	send_bytesToWrite = 0;
	QString ip = YouIP;
	send_ip.setAddress(ip);
    send_udpSocket = new QUdpSocket(this);//用于进行UDP网络通信
    send_udpSocket->bind(4444, QUdpSocket::ShareAddress);//绑定到本地IP地址的4444端口上,以便监听来自该端口的UDP数据包

    m_pTimer = new QTimer(this);//用于定期触发一个定时事件
	connect(m_pTimer, SIGNAL(timeout()), this, SLOT(handletimeout()));
	connect(send_udpSocket, SIGNAL(readyRead()), this, SLOT(send_readmessage()));
    ui->pushButton_Send->setEnabled(false);//将发送按钮设为不可用状态
}


void Sendfile::openFile() {
    send_fileName = QFileDialog::getOpenFileName(this);//选择要发送的文件
    if (!send_fileName.isEmpty()) { //如果用户选择了一个有效的文件,发送按钮将被设置为可用状态
		ui->pushButton_Send->setEnabled(true);
	}
}

void Sendfile::startTransfer() {
    send_localFile = new QFile(send_fileName);//指定要发送的文件
    if (!send_localFile->open(QFile::ReadOnly)) {//打开文件send_localFile,如果打开失败,则输出错误信息并返回
		qDebug() << "open file error!";
		return;
	}
    send_totalBytes = send_localFile->size();//获取文件的总字节数
	qDebug() << send_totalBytes;

    //创建一个QDataStream对象sendOut,并将其与一个QByteArray对象send_outBlock关联起来,用于数据的读写操作
	QDataStream sendOut(&send_outBlock, QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_5_6);//设置数据流的版本

    //从文件名send_fileName中提取文件名部分
	QString currentFileName = send_fileName.right(send_fileName.size() - send_fileName.lastIndexOf('/') - 1);
    //将三个数据依次写入数据流sendOut中，分别是0、0和currentFileName
    sendOut << qint64(0) << qint64(0) << currentFileName;
    send_totalBytes  += send_outBlock.size();//将数据流的大小加到总字节数上
    sendOut.device()->seek(0);//将数据流的读写指针设置为开头位置
    sendOut << send_totalBytes << qint64((send_outBlock.size() - sizeof(qint64) * 2));//将两个值写入数据流sendOut中
    m_pTimer->start(TIMEOUT);//启动一个定时器,在时间到达之后执行相应的处理函数

    //将数据流send_outBlock中的数据通过UDP协议发送到指定IP地址和端口,并将未发送的字节数赋值给变量send_bytesToWrite
	send_bytesToWrite = send_totalBytes - send_udpSocket->writeDatagram(send_outBlock, send_outBlock.size(), send_ip, 7777);
	qDebug() << send_bytesToWrite;
    send_bytesWritten = send_outBlock.size();//将已经写入的字节数赋值给变量send_bytesWritten
	qDebug() << send_bytesWritten;
    ui->progressBar_send->setMaximum(send_totalBytes);//设置发送进度条的最大值为文件的总字节数
    ui->progressBar_send->setValue(send_bytesWritten);//设置发送进度条的当前值为已经写入的字节数
    send_outBlock.resize(0);//将数据流send_outBlock的大小调整为0,清空数据
}

void Sendfile::send() {
    //判断文件是否已经读取完毕
    if (!send_localFile->atEnd()) { //没有读取完毕
        line = send_localFile->read(qMin(send_bytesToWrite, send_loadSize));//从文件中读取一行数据

        //更新send_bytesToWrite和send_bytesWritten 的值,分别减去发送的字节数和增加已发送的字节数
        send_bytesToWrite -= send_udpSocket->writeDatagram(line, line.size(), send_ip, 7777);
		send_bytesWritten = send_bytesWritten + line.size();

        //更新进度条的最大值和当前值
		ui->progressBar_send->setMaximum(send_totalBytes);
		ui->progressBar_send->setValue(send_bytesWritten);
		qDebug() << send_bytesWritten;
    } else { //已经读取完毕
        send_udpSocket->writeDatagram("over", send_ip, 7777);//发送一个包含字符串 "over" 的数据报
        m_pTimer->stop();//关闭计时器
        ui->progressBar_send->close();//关闭进度条
        send_udpSocket->close();//关闭 UDP socket
        send_localFile->close();//关闭文件
	}
	return;
}

void Sendfile::send_readmessage() {
    //循环判断是否有待处理的数据报
	while (send_udpSocket->hasPendingDatagrams()) {
		QByteArray res = "false";
        res.resize(send_udpSocket->pendingDatagramSize());
        send_udpSocket->readDatagram(res.data(), res.size());//读取数据报内容
		qDebug() << res;
		if (res == "true") {
			flag = 1;
		}
	}
}

//用于处理超时事件
void Sendfile::handletimeout() {
	qDebug() << flag;
	if (flag == 1) {
        send();//发送文件数据
		flag = 0;
	} else {
        send_udpSocket->writeDatagram(line, line.size(), send_ip, 7777);//将当前行数据发送出去
	}
}



//文件传输--接收文件
void Sendfile::receive_init() {
    //初始化，包括接收的总字节数、已接收的字节数和文件名的大小
	receive_totalBytes = 0;
	receive_bytesReceived = 0;
	receive_fileNameSize = 0;
    QString ip = YouIP;
    receive_ip.setAddress(ip);//IP地址设置为YouIP
    receive_udpSocket = new QUdpSocket();//创建了receive_udpSocket用于接收UDP数据
    //将receive_udpSocket与本地7777端口进行绑定,并设置可以与其他进程共享该端口
	receive_udpSocket->bind(7777, QUdpSocket::ShareAddress);
	connect(receive_udpSocket, SIGNAL(readyRead()), this, SLOT(processPendingDatagram()));
}

void Sendfile::processPendingDatagram() {
    while (receive_udpSocket->hasPendingDatagrams()) { //有待处理的数据报
        //向发送方发送一个包含字符串"true"的数据报,并指定接收方的IP地址为receive_ip,端口为4444
		receive_udpSocket->writeDatagram("true", receive_ip, 4444);
        //通过pendingDatagramSize函数获取数据报的大小,然后通过resize函数将接收缓冲区receive_inBlock的大小调整为数据报的大小
        receive_inBlock.resize(receive_udpSocket->pendingDatagramSize());
		qDebug() << receive_inBlock.size();
        receive_udpSocket->readDatagram(receive_inBlock.data(), receive_inBlock.size());//将数据报中的数据读取到receive_inBlock中
        if (receive_inBlock == "over") {//文件传输结束
			qDebug() << receive_inBlock;
            //关闭UDP socket、文件和进度条
			receive_udpSocket->close();
			receive_localFile->close();
			ui->progressBar_receive->close();
		} else {
            updateServerProgress();//更新服务器端的传输进度
		}
	}
}

void Sendfile::updateServerProgress() {
    QDataStream in(&receive_inBlock, QIODevice::ReadOnly);//对接收到的数据进行解析,使用QIODevice::ReadOnly模式打开数据流
    in.setVersion(QDataStream::Qt_5_6);//设置数据流版本为Qt5.6
    if (receive_bytesReceived <= sizeof(qint64) * 2) { //检查接收到的数据字节数是否小于等于sizeof(qint64) * 2
        //接收的数据块大小是否大于等于sizeof(qint64) * 2,并且接收的文件名大小为0
        if (((unsigned int)receive_inBlock.size() >= sizeof(qint64) * 2)
		        && (receive_fileNameSize == 0)) {
            in >> receive_totalBytes >> receive_fileNameSize;//读取接收的数据块中的总字节数和文件名大小
			qDebug() << receive_totalBytes << receive_fileNameSize;
            //设置接收进度条的最大值为总字节数,当前值为已接收的字节数
			ui->progressBar_receive->setMaximum(receive_totalBytes);
			ui->progressBar_receive->setValue(receive_bytesReceived);
			qDebug() << receive_bytesReceived;
		}
        //检查接收的数据块是否大于等于文件名大小,并且文件名大小不为0
		if (((unsigned int)receive_inBlock.size() >= receive_fileNameSize)
		        && (receive_fileNameSize != 0)) {
            in >> receive_fileName;//读取接收的数据块中的文件名
			qDebug() << receive_fileName;
			qDebug() << receive_bytesReceived;
            receive_localFile = new QFile(receive_fileName);//保存接收到的文件
            if (!receive_localFile->open(QFile::WriteOnly)) {//以只写模式打开文件
				qDebug() << "open file error!";
				return;
			}
		} else return;
	}
    //检查已接收的字节数是否小于总字节数
	if (receive_bytesReceived < receive_totalBytes) {
        receive_bytesReceived += receive_inBlock.size();//将已接收的字节数增加接收数据块的大小
        receive_localFile->write(receive_inBlock);//将接收数据块写入到本地文件中
		qDebug() << receive_localFile->size();
        receive_inBlock.resize(0);//将接收数据块的大小设为0,以便下一次接收新的数据块
	}
	qDebug() << receive_bytesReceived;
    ui->progressBar_receive->setMaximum(receive_totalBytes);//设置接收进度条的最大值为总字节数
    ui->progressBar_receive->setValue(receive_bytesReceived);//当前值为已接收的字节数
}

void Sendfile::on_pushButton_Open_clicked() {
	openFile();
}

void Sendfile::on_pushButton_Send_clicked() {
	startTransfer();
}
