#ifndef MYSQL_H
#define MYSQL_H

#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QMessageBox>
class MySql
{
public:
     MySql();
     bool initsql();
     int Login(QString Name, QString Password); //登录
     bool Regist(QString Name, QString Password, QString Mail); //注册
     int findpwd(QString name,QString Mail); //找回密码
private:
    QSqlQuery *query;

};

#endif // MYSQL_H
