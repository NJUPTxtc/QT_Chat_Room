#include "login.h"
#include <QApplication>

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	Login w;
	w.setWindowTitle("Login");
	w.show();
	return a.exec();
}
