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

private slots:
	void inputFilePath();//����Ŀ¼
 	void outputFilePath();//���Ŀ¼
 	void inputIniPath();//�����ļ�·��
 	void runClassify();//���з���
	void undo();//����
	void changeSubFlag(bool a){	onlyReport = a;}
	void inBigLevel(bool a){ isBigLevel = a; }
	void xmlInput(bool a){inputWay = a; };
	void openIni(){	QProcess::execute("notepad " + iniPath);}
	void pathDef();
private:
	Ui::FileClassifyClass ui;
	bool onlyReport=false,isBigLevel=false;
	QDir productPath;//����Ŀ¼
	QDir filePath;//�������ļ�Ŀ¼
	QString iniPath;//�����ļ�·��
	QString m_clsName;//�����ķ�����
	//�����滻��Ѱ����
	bool m_renameNeedReplace = false;
	QString m_renameSplitChar,m_renameAppend;//�ָ���
	std::vector<int> m_renameDstSeq;//����˳��
	bool m_renameReplaceDot;//�Ƿ��滻.

	QMap<QString, QVector<QString>> fileParameter;//�ļ���Ӧ����
	//ini�ļ��ڶ�ȡ����
	QVector<ClassificationConfig> clsRank;//ÿ������е���Ӧ�ȼ�
	bool inputWay=false;
	void iniRead();
	void parRead();
	void parReadFromXml();
	void subDirCreate(const QStringList&);
	void nameReplace();
	bool pathCheck();
};

#endif // FILECLASSIFY_H
