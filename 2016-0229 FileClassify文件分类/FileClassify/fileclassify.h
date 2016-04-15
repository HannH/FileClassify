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
	QString clsName;//��������
	QStringList rankName;//�ּ�����
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
	void inputFilePath();//����Ŀ¼
 	void outputFilePath();//���Ŀ¼
 	void inputIniPath();//�����ļ�·��
 	void runClassify();//���з���
	void changeSubFlag(bool a){	subDirFlag = a;}
	void xmlInput(bool a){inputWay = a; };
	void openIni(){	QProcess::execute("notepad " + iniPath);}
private:
	Ui::FileClassifyClass ui;
	bool subDirFlag;
	QDir productPath;//����Ŀ¼
	QDir filePath;//�������ļ�Ŀ¼
	QString iniPath;//�����ļ�·��
	QMap<QString, QVector<float>> fileParameter;//�ļ���Ӧ����
	//ini�ļ��ڶ�ȡ����
	QVector<ClassificationConfig> clsRank;//ÿ������е���Ӧ�ȼ�
	bool inputWay=false;
	void iniRead();
	void parRead();
	void parReadFromXml();
	void subDirCreate(const QStringList&);
	void pathDef();
	bool pathCheck();
};

#endif // FILECLASSIFY_H
