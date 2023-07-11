#include "server.h"
#include <QApplication>
#include "mysql.h"
#include <QTextCodec>

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	MySql ms;
	ms.initsql();
	Server w;
	w.setWindowTitle("JNT Server");
	w.show();

	return a.exec();
}
