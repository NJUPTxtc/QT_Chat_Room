#include "dialog.h"
#include "ui_dialog.h"
#include<QDebug>
int i;
Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog) {
    ui->setupUi(this);
    tcpSocket = new QTcpSocket(this);

    ip = getIP();
    //ip = "240e:460:780:1002:f161:b00a:fd9d:e977";

    map.load(":/icon/1.jpg");

    map = map.scaled(QSize(390, 330));
    ui->label_back->setPixmap(map);

    this->setWindowTitle(tr("私人聊天中..."));
    tcpSocket->connectToHost(serverIP, 8000);

    fDialog = new QFileDialog(this);
    fDialog->setFileMode(QFileDialog::ExistingFiles);
    connect(fDialog, SIGNAL(fileSelected ( const QString & )), this, SLOT(fs(const QString & )));
    fDialog->hide();
    initSocket();
}

Dialog::~Dialog() {
    delete ui;
}

void Dialog::fs(const QString & fs) {
    //选中文件点击open后会出发该信号 至在打开单一文件时出发
    qDebug() << "fs" << fs;
    files.clear();
    files.append(fs);

    file.setFileName(files);
    if (!file.open(QIODevice::ReadOnly))
        return;

    QByteArray qq;
    qq.append("|");
    qq.append(ip + "|");
    qq.append(ipRec + "|");
    qq.append(fs);
    tcpSocket->write(qq);
}

void Dialog::sendData() {
    if (!file.atEnd()) {
        QByteArray line = file.read(8000);
        udpSocket->writeDatagram(line, QHostAddress(ipRec), 7755);
        i++;
        qDebug() << "send over!" << i << line.size();
        if (line.size() < 8000) {
            QByteArray yes;
            yes.append("*^*文件接收完成！");
            tcpSocket->write(yes);
            QMessageBox::warning(this, tr("通知"), tr("发送成功！"), QMessageBox::Yes);
        }
    }
}

void Dialog::initSocket() {
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(QHostAddress::Any, 7758);
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));
}

void Dialog::readPendingDatagrams() {
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        udpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

        qDebug() << datagram;
        if (datagram == "1") {
            qDebug() << "send OK!";
            sendData();
        }
    }
}

void Dialog::on_pushButton_send_clicked() {       //发送键
    QByteArray tmp;
    tmp.append("&&");
    tmp.append(ipRec + "&&");
    tmp.append(ui->lineEdit->text() + "&&");
    tmp.append(ip);
    tcpSocket->write( tmp );

    tcpSocket->flush();
    ui->lineEdit->clear();
    ui->lineEdit->setFocus();
}

QString Dialog::getIP() { //获取ip地址
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    foreach (QHostAddress address, list) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol) {
            if (address.toString().contains("127.0.")) {
                continue;
            }
            return address.toString();
        }
    }
    return 0;
}

void Dialog::chuanIP(QString ip_rec, QString server_ip) {
    ipRec = ip_rec;
    ipRec.remove(0, 7);
    qDebug() << "ipRec1: " + ipRec;
//    ipRec="192.168.56.1";
    serverIP = server_ip;
    qDebug() << "ipRec2: " + ipRec;
    qDebug() << "serverIP: " + serverIP;
    ui->lineEdit_IP->setText(ipRec);
    ui->lineEdit_IP->setReadOnly(true);
    tcpSocket->connectToHost(serverIP, 8000);
}

void Dialog::chuanMessage(QString a) {
    qDebug() << a;
    ui->textEdit->append(a);
    ui->textEdit->setReadOnly(true);
}

void Dialog::chuanName(QString a) {
    ui->lineEdit_name->setText(a);
    ui->lineEdit_name->setReadOnly(true);
    ui->textEdit->clear();
}

void Dialog::on_comboBox_currentIndexChanged(int index) {
    if (index == 0) {
        map.load(":/icon/1.jpg");
        map = map.scaled(QSize(390, 330));
        ui->label_back->setPixmap(map);
    }
    if (index == 1) {
        map.load(":/icon/2.jpg");
        map = map.scaled(QSize(390, 330));
        ui->label_back->setPixmap(map);
    }
    if (index == 2) {
        map.load(":/icon/3.jpg");
        map = map.scaled(QSize(390, 330));
        ui->label_back->setPixmap(map);
    }
    if (index == 3) {
        map.load(":/icon/4.jpg");
        map = map.scaled(QSize(390, 330));
        ui->label_back->setPixmap(map);
    }
}

void Dialog::on_pushButton_sendFile_clicked() {
    fDialog->show();
}

void Dialog::send_() {
    sendData();
}


