#include "fileclassify.h"
#include <fstream>
#include <QFileDialog>
#include <QTextStream>
#include <QFileInfo>
#include <QSettings>
#include <string>
#include <QMessageBox>
#include <sstream>
#include <qxmlstream.h>

# pragma execution_character_set("utf-8")
FileClassify::FileClassify(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
}
FileClassify::~FileClassify()
{

}
void FileClassify::iniRead(){
	if (!filePath.exists(iniPath))	{
		QMessageBox::warning(this, "����", "�����ļ�·������,������", QMessageBox::Ok);
		return;
	}
	QSettings sets(iniPath, QSettings::IniFormat);
	QStringList basicKey,childKey;
	//��ȡ��Ӧ������
	sets.beginGroup("basic");
	basicKey = sets.childKeys();
	sets.endGroup();
	int i, j,sz = basicKey.size();
	clsRank.resize(sz);
	//��ȡ�ּ���
	for (i = 0; i < sz; i++){
		sets.beginGroup(basicKey[i]);
		clsRank[i].clsName = basicKey[i];
		clsRank[i].rankName = sets.childKeys();
		int crs = clsRank[i].rankName.size();
		clsRank[i].maxValue.resize(crs), clsRank[i].minValue.resize(crs);
		QStringList sl;
		for (j = 0; j < crs; j++){
			sl = sets.value(clsRank[i].rankName[j]).toString().split(" ");
			clsRank[i].minValue[j] = sl[0].toFloat(), clsRank[i].maxValue[j] = sl[1].toFloat();
		}
		sets.endGroup();
	}
}
//�ж�str�Ƿ��Ǵ�����(trueΪ��)
bool isNum(std::string str)
{
	std::stringstream sin(str);
	double d;
	char c;
	if (!(sin >> d))
		return false;
	if (sin >> c)
		return false;
	return true;
}
void FileClassify::parRead(){
	int i,j,sz = clsRank.size();
	float m;
	fileParameter.clear();
	for (i = 0; i < sz; i++){
		QString filename = productPath.absolutePath() +"/" + clsRank[i].clsName + ".txt";
		if (!filePath.exists(filename))		{
			QMessageBox::warning(this, "����", filename+"·������,���������", QMessageBox::Ok);
			return;
		}
		std::ifstream inStream(filename.toStdWString());
		std::string info;
		//--
		while (inStream>>info){
			if (isNum(info))	continue;
			QFileInfo sInfo=QFileInfo(QString::fromStdString(info));
			QString sInfoName = sInfo.completeBaseName();
			j = sInfoName.indexOf('-');
			if(j!=-1)		sInfoName = sInfoName.remove(j,sInfoName.size()-j);
			inStream >> m;
			if (!fileParameter.contains(sInfoName))		fileParameter[sInfoName].fill(-1, sz);
			fileParameter[sInfoName][i] = abs(m);
		}
	}
	ui.plainTextEdit->clear();
	for (i = 0; i < fileParameter.size(); i++){
		ui.plainTextEdit->appendPlainText(fileParameter.keys()[i]);
	}
}
//��,��ڽ�����
QString cloudNameInXml = "CloudPercent", SatelliteAngle = "YawSatelliteAngle";
void FileClassify::parReadFromXml(){
	int i, j, sz = clsRank.size();
	QStringList attributeNames;
	float m;
	for (i = 0; i < sz; i++)
		attributeNames.append(clsRank[i].clsName);
	fileParameter.clear();
	QFileInfoList folders = filePath.entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot);
	for (QFileInfo folder : folders){
		QString xmlName(folder.absoluteFilePath()+'/'+folder.fileName()+"_gDosMeta.xml");
		if (!QFile::exists(xmlName))
			continue;
		QXmlStreamReader xml(folder.absoluteFilePath()+"_gDosMeta.xml");
		while (!xml.atEnd()){
			xml.readNext();
			QString name = xml.name().toString();
			if (xml.name() == "elements")
				auto xmlAttributes=xml.attributes();
			if (xml.name() == "element"){
				QString name = xml.attributes().value("name").toString();
				if (attributeNames.count(name)){
					j=attributeNames.indexOf(name);
					if (!fileParameter.contains(name))		fileParameter[name].fill(-1, sz);
					m = xml.readElementText().toFloat();
					fileParameter[name][j] = abs(m);
				}	
			}
		}
	}
	ui.plainTextEdit->clear();
	for (i = 0; i < fileParameter.size(); i++){
		ui.plainTextEdit->appendPlainText(fileParameter.keys()[i]);
	}
}
void FileClassify::inputFilePath(){
//	productPath = QFileDialog::getExistingDirectory(this, tr("����·��"),"J:\\YangtzeRiver_Prj");
	productPath = QFileDialog::getExistingDirectory(this, tr("����·��"), "E:\\������Ŀ\\FileClassify\\���Թ���");
	if (!productPath.exists()) {
		QMessageBox::warning(this, "����", "�����ļ�·������,������", QMessageBox::Ok);
		return;
	}
	ui.lineEdit->setText(productPath.absolutePath());
	filePath = productPath.absolutePath() + "/product/DOM";
	ui.lineEdit_2->setText(filePath.absolutePath());
	iniRead();
	if(inputWay==0)
		parRead();
	else parReadFromXml();
}
void FileClassify::outputFilePath(){
	filePath = QFileDialog::getExistingDirectory(this, tr("���·��"),filePath.absolutePath());
	if (!filePath.exists()) {
		QMessageBox::warning(this, "����", "���·��������,������", QMessageBox::Ok);
		return;
	}
	ui.lineEdit_2->setText(filePath.absolutePath());
	if (inputWay == 1)
		parReadFromXml();
}
void FileClassify::inputIniPath(){
	iniPath = QFileDialog::getOpenFileName(this, tr("�����ļ�����"), filePath.absolutePath(),"*.ini");
	if (!filePath.exists(iniPath)) {
		QMessageBox::warning(this, "����", "����·��������,������", QMessageBox::Ok);
		return;
	}
	ui.lineEdit_3->setText(iniPath);
}
/*
//�����ļ���(�ɹ�����true)
bool creatFolder(const char *destdir){
	char cmd[1024] = "md ";
	strcat(cmd, destdir);
	return !system(cmd);
}*/
//�ƶ��ļ���(�ɹ�����true)
bool moveFolder(const char* srcdir,const char* destdir){
	QDir ttemp = QString(destdir);
//	if (!ttemp.exists())	creatFolder(destdir);
	char cmd[1024] = "cmd /c move ";
	strcat(cmd, srcdir);
	strcat(cmd, " ");
	strcat(cmd, destdir);
	return !WinExec((LPCSTR)cmd, SW_HIDE);
}
QString nameTrans(const QString &name){
	int i;
	for (i = 0; i < name.size(); i++) if (name[i] >= 'A'&&name[i] <= 'Z')	break;
	QString resault(name);
	return resault.remove(i + 1, name.size() - i - 1);
}
void FileClassify::subDirCreate(const QStringList& rankName){
	for (int i = 0; i < rankName.size();i++){
			filePath.mkdir(filePath.absoluteFilePath(nameTrans(rankName[i])));
			moveFolder(QDir::toNativeSeparators(filePath.absoluteFilePath(rankName[i])).toLocal8Bit(), 
				QDir::toNativeSeparators(filePath.absoluteFilePath(nameTrans(rankName[i]))).toLocal8Bit());
		}
}
void FileClassify::runClassify(){
	int i=0, j,m=0;
	QStringList rankName	= clsRank[0].rankName;
	for (i = 0; i < rankName.size(); i++){
		rankName[i] = nameTrans(rankName[i]);
		for (j = 0; j < clsRank.size(); j++)
			rankName[i] += '_' + QString::number(clsRank[j].minValue[i]) + '-' + QString::number(clsRank[j].maxValue[i]) ;
	}
	char ch = 'A';
	i = 0;
	//�����ּ��ļ���
	while (i<rankName.size()){
//		bool flag = false;
//		if (rankName[i].contains(ch)){
// 			if (flag == false&&rankName[i]!=QString(ch))
// 				creatFolder((filePath.absoluteFilePath(QString(ch))).toStdString().c_str()),flag=true,ch++;
		filePath.mkdir(filePath.absoluteFilePath(rankName[i]));
		QString name = filePath.absolutePath();
//			creatFolder((filePath.absoluteFilePath(rankName[i])).toLocal8Bit());
//		}
		i++;
	}
	filePath.mkdir(filePath.absoluteFilePath("δ����"));
	//�����ļ���
	QStringList keyName = fileParameter.keys();
	for (auto ite = fileParameter.begin(); ite != fileParameter.end();ite++,m++){		//3:123,456,777
		i = 0, j = 0;
		bool flag = false;
		//�����������
		while (j < rankName.size()){		//3:1a,2a,b
			if (ite->at(i)>clsRank[i].minValue[j] && ite->at(i) <= clsRank[i].maxValue[j])
				if (i == clsRank.size() - 1){
					moveFolder(QDir::toNativeSeparators(filePath.absoluteFilePath(keyName[m])).toLocal8Bit(), QDir::toNativeSeparators(filePath.absoluteFilePath(rankName[j])).toLocal8Bit());
					flag = true;
					break;
				}
				else i++;
			else i=0,j++;
		}
		if (flag == false)	moveFolder(QDir::toNativeSeparators(filePath.absoluteFilePath(keyName[m])).toLocal8Bit(), QDir::toNativeSeparators(filePath.absoluteFilePath("δ����")).toLocal8Bit());
	}
	if (subDirFlag == true)	subDirCreate(rankName);
	QMessageBox::information(this ,"��ʾ", "�������!", QMessageBox::Ok);
}
