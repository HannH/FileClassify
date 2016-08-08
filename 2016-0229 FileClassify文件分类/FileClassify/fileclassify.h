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
#include <vector>
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

private slots:
	void inputFilePath();//输入目录
 	void outputFilePath();//输出目录
 	void inputIniPath();//配置文件路径
 	void runClassify();//进行分类
	void undo();//撤销
	void changeSubFlag(bool a){	onlyReport = a;}
	void inBigLevel(bool a){ isBigLevel = a; }
	void xmlInput(bool a){inputWay = a; };
	void openIni(){	QProcess::execute("notepad " + iniPath);}
	void pathDef();
private:
	Ui::FileClassifyClass ui;
	bool onlyReport=false,isBigLevel=false;
	QDir productPath;//工程目录
	QDir filePath;//待分类文件目录
	QString iniPath;//配置文件路径
	QString m_clsName;//决定的分类名
	//名称替换搜寻参数
	bool m_renameNeedReplace = false;
	QString m_renameSplitChar,m_renameAppend;//分隔符
	std::vector<int> m_renameDstSeq;//名称顺序
	bool m_renameReplaceDot;//是否替换.

	QMap<QString, QVector<QString>> fileParameter;//文件对应参数
	//ini文件内读取参数
	QVector<ClassificationConfig> clsRank;//每个类别中的相应等级
	bool inputWay=false;
	void iniRead();
	void parRead();
	void parReadFromXml();
	void subDirCreate(const QStringList&);
	void nameReplace();
	bool pathCheck();
};

#endif // FILECLASSIFY_H
