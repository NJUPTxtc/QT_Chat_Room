#ifndef REGISTER_H
#define REGISTER_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QGridLayout>
#include <QWidget>
#include <QObject>
#include <QtSql>
#include <QTcpSocket>
#include <QString>
#include <QMessageBox>
#include <QPainter>
#include <QtSql/QSql>
#include <QDebug>
#include "login.h"
namespace Ui {
    class Register;
}

class Register: public QDialog
{
    Q_OBJECT
public:
    explicit Register(QWidget * parent=0);
    ~Register();
    bool IsEnglish(QString &qstrSrc);
    QString IP = Login::IP;
    int Port = Login::Port;
private:
    Ui::Register *ui;
    QTcpSocket *tcpSocket;
    QString Name;
    QString Password;
    QString Repassword;
    QString Mail;
private slots:
    void readMSG();
    void on_pushButton_Register_clicked();
    void on_pushButton_Back_clicked();
};

#endif // REGISTER_H
