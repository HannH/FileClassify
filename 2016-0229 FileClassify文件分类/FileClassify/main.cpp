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

/*#include <qxmlstream.h>
#include <QString>

void main(){
QString qs = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<root>\n"
"<elements>\n"
"<element name=\"PrjScaleFactor\"><![CDATA[1.000000]]></element>\n"
"<element name = \"Recipient\"><![CDATA[-1.1]]></element>\n"
"</elements></root>\n";
QXmlStreamReader xml(qs);
while (!xml.atEnd()){
xml.readNext();
auto name = xml.name().toString();
auto attr = xml.attributes();
}
}*/