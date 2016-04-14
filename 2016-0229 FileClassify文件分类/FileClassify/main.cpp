#include "fileclassify.h"
#include <QtWidgets/QApplication>
#include <QString>
#include <QFile>
bool creatFolder(char *destdir){
	char cmd[1024] = "md ";
	strcat(cmd, destdir);
	return !system(cmd);
}
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QString name = QCoreApplication::applicationDirPath();
	if(creatFolder("E:\\DATA\\2")) return 1;
	FileClassify w(name);
	w.show();
	return a.exec();
}
