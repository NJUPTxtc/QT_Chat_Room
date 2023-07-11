#include "feiq_3.h"
#include "ui_feiq_3.h"

FeiQ_3::FeiQ_3(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FeiQ_3)
{
    ui->setupUi(this);
    bbb = new Dialog(this);
    ccc = new DialogRec(this);

    ip = getIP();

    QPixmap tmp;
    tmp.load(":/icon/0.jpg");
    tmp = tmp.scaled(QSize(545,430));
    ui->label_map->setPixmap(tmp);

    this->setWindowTitle(tr("即时通信"));
    ui->lineEdit_server->setText("10.11.72.16");
    ui->lineEdit_name->setText("lihua");

    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket,SIGNAL(connected()),this, SLOT(connectedSlot()));
    connect(ui->lineEdit,SIGNAL(returnPressed()),this,SLOT(on_pushButton_clicked()));
    connect(ccc,SIGNAL(accepted()),this,SLOT(acc()));
}

FeiQ_3::~FeiQ_3()
{
    delete ui;
}

void FeiQ_3::connectToServer(QString server)  ////与服务器建立连接；
{
    tcpSocket->connectToHost(server,8000);
}

void FeiQ_3::readMessage()                            //读消息；
{
    QString tmp;
    tmp = tcpSocket->readAll();
    qDebug()<<"tmp: "+tmp;
    if(tmp.contains("   @") ){
        ui->listWidget->clear();
        tm = tmp.split("   @");
        qDebug()<<tm;
        for(int i = 0; i < tm.count(); i ++){
                ui->listWidget->addItem(tm.at(i));
        }
    }else if(tmp.startsWith("&&")){
        sm = tmp.split("&&");
        if(sm.at(3) == ip ){
            bbb->chuanMessage( "<font color=blue>［自已］</font>" + sm.at(3) + ":<br>" + sm.at(2));
            if(sm.at(1) == ip){
                bbb->chuanMessage("<font color=red>［对方］</font>" + sm.at(3) + ":<br>" + sm.at(2));
            }
        }
        if(sm.at(1) == ip ){
            if(sm.at(3) != ip){
                bbb->show();
                bbb->chuanIP(sm.at(3),ui->lineEdit_server->text());
                bbb->chuanMessage("<font color=red>［对方］</font>" + sm.at(3) +":<br>" + sm.at(2));
            }
        }
        qDebug()<<"readMessage over!";
    }else if(tmp.startsWith("|") ){
        rm = tmp.split("|");
        rece_ip = rm.at(2);
        pps = tmp.split("/");

        if(ip == rece_ip){
            ccc->show();
            ccc->fileName(pps.at(pps.count() -1), rm.at(1));
            bbb->chuanMessage(rm.at(1) + "给你传文件！" + rm.at(3));
        }
    }else if(tmp.startsWith("$$$")){
        bbb->send_();
    }
    else if(tmp.startsWith("*^*")){
        bbb->chuanMessage(tmp);
    }
    else{
    ui->textEdit->append( tmp );
    }
}

void FeiQ_3::connectedSlot()
{
    qDebug()<<"connect OK!";
    QMessageBox::warning(this,tr("恭喜"),tr("连接成功！"),QMessageBox::Yes);
}

void FeiQ_3::on_pushButton_clicked() //发送消息键
{
    QByteArray tmp,name;
    qint64 len;
    name.append(ui->lineEdit_name->text());
    name.append(": ");
    tmp.append( /*ip + "  "+ */"["+name+"]" + ui->lineEdit->text() );
    tcpSocket->write(  tmp + '\n' );

    tcpSocket->flush();
    ui->lineEdit->clear();
    ui->lineEdit->setFocus();
    qDebug()<<len;
}

QString FeiQ_3::getIP()  //获取ip地址
{
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    foreach (QHostAddress address, list){
       if(address.protocol() == QAbstractSocket::IPv4Protocol){
           if (address.toString().contains("127.0.")){
               continue;
           }
           return address.toString();
       }
    }
    return 0;
}

void FeiQ_3::on_pushButton_in_clicked() //连接服务器按键
{
    QString IP = ui->lineEdit_server->text();
    connectToServer(IP);
    ipname.append("#");
    ipname.append(ip);
    ipname.append("#");
    ipname.append(ui->lineEdit_name->text());
    tcpSocket->write(ipname);
    connect(tcpSocket,SIGNAL(readyRead()),this, SLOT(readMessage()));
}

void FeiQ_3::on_pushButton_out_clicked() //断开服务器按键
{
    tcpSocket->disconnectFromHost();
    ui->pushButton_in->setDisabled(false);
    ui->listWidget->clear();
}

void FeiQ_3::on_listWidget_doubleClicked(const QModelIndex &index)  //IP列表双击事件
{
    QString sip;
    QStringList s_ip;

    sip.append(tm.at(index.row()));
    qDebug()<<"sip: "+sip;
    s_ip = sip.split('\n');
    qDebug()<<s_ip;
    bbb->show();
    bbb->chuanIP(s_ip.at(0), ui->lineEdit_server->text());
    qDebug()<<"chuanIP";
//    qDebug()<<s_ip.at(1);
//    bbb->chuanName(s_ip.at(1));
    bbb->chuanName(ui->lineEdit_name->text());
    qDebug()<<"chuanName";
}

void FeiQ_3::acc()
{
    QByteArray pp;
    pp.append("$$$对方已接收你的文件请求！");
    tcpSocket->write(pp);
}
