#include "dialogrec.h"
#include "ui_dialogrec.h"

DialogRec::DialogRec(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogRec)
{
    ui->setupUi(this);
    bbb = new Dialog(this);

    fDialog = new QFileDialog(this);
    fDialog->setFileMode(QFileDialog::Directory);
    QPixmap map;
    map.load(":/icon/6.jpg");
    map = map.scaled(QSize(210,150));
    ui->label_map->setPixmap(map);

    this->setWindowTitle(tr("文件传输！"));
    connect(fDialog,SIGNAL(fileSelected ( const QString & )),this,SLOT(fs(const QString & )));

    fDialog->hide();

    udpSocket = new QUdpSocket(this);
    udpSocket->bind(QHostAddress::Any, 7755);
    ui->lineEdit_path->clear();
}

DialogRec::~DialogRec()
{
    delete ui;
}

void DialogRec::fs(const QString & file){
    //选中文件点击open后会出发该信号 至在打开单一文件时出发
    qDebug() <<"fs";
    qDebug() << file;

    ui->lineEdit_path->setText(file);
    filee.append(file);
    filee.append("/");
    filee.append(file_name);
    qDebug()<<filee<<"filee";
    qDebug()<<"OKKKKK";
}

void DialogRec::initSocket()
{
    connect(udpSocket, SIGNAL(readyRead()),
            this, SLOT(readPendingDatagrams()));
    file.setFileName( filee );
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Unbuffered))
        return;
    filee.clear();
}

void DialogRec::readPendingDatagrams()
{
    int i = 0;        //receive
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        udpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

        file.write(datagram.data(),datagram.size());
        i++;
        udpSocket->writeDatagram("1",1, sender ,7758);

        qDebug() << i << datagram.size();
    }
}

void DialogRec::fileName(QString a,QString b)
{
    filee.clear();
    file_name.clear();
    file_name.append(a);
    ui->lineEdit->setText(file_name);
    ui->label->setText(b + " 给您传文件，<br>是否接收？？？");

}

void DialogRec::on_pushButton_3_clicked()
{
    fDialog->show();
}

void DialogRec::on_pushButton_clicked()
{
    initSocket();
}
