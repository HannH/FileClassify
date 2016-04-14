#include "fileclassify.h"
#include <QtWidgets/QApplication>


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QString name = QCoreApplication::applicationDirPath();
	FileClassify w(name);
	w.show();
	return a.exec();
}
