#ifndef FILECLASSIFY_H
#define FILECLASSIFY_H

#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
#include <QtWidgets/QMainWindow>
#include <QtCore/QMap>
#include <QtCore/QVector>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QDir>
#include <QProcess>
#include <cpl_win32ce_api.h>
#include "ui_fileclassify.h"
struct ClassificationConfig
{
	QString clsName;//分类名称
	QStringList rankName;//分级名称
	QVector<float> maxValue, minValue;
};
class FileClassify : public QMainWindow
{
	Q_OBJECT

public:
	FileClassify(QWidget *parent = 0);
	FileClassify(QString path){ 
	ui.setupUi(this); 
	iniPath = path + "/config.ini";
	QDir t;
	if(t.exists(iniPath))	ui.lineEdit_3->setText(iniPath);
	};
	~FileClassify();
	void getDirPath(QString path){ iniPath = path + "config.ini"; };
protected:
	void mousePressEvent(QMouseEvent *qm){
		pathDef();
	}
private slots:
	void inputFilePath();//输入目录
 	void outputFilePath();//输出目录
 	void inputIniPath();//配置文件路径
 	void runClassify();//进行分类
	void changeSubFlag(bool a){	subDirFlag = a;}
	void xmlInput(bool a){inputWay = a; };
	void openIni(){	QProcess::execute("notepad " + iniPath);}
private:
	Ui::FileClassifyClass ui;
	bool subDirFlag;
	QDir productPath;//工程目录
	QDir filePath;//待分类文件目录
	QString iniPath;//配置文件路径
	QMap<QString, QVector<float>> fileParameter;//文件对应参数
	//ini文件内读取参数
	QVector<ClassificationConfig> clsRank;//每个类别中的相应等级
	bool inputWay=false;
	void iniRead();
	void parRead();
	void parReadFromXml();
	void subDirCreate(const QStringList&);
	void pathDef();
	bool pathCheck();
};

#endif // FILECLASSIFY_H
