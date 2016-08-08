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
#include <QDebug>
#include <windows.h>
#include <Shlwapi.h>


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
		return;
	}
	QSettings sets(iniPath, QSettings::IniFormat);
	QStringList childKey, numKey;

	//读取命名规则
	sets.beginGroup("ProductNameRule");
	QString t_value;
	t_value=sets.value("srcSplitBy").toString();
	m_renameSplitChar = t_value;
	t_value=sets.value("dstNameSequence").toString();
	if (t_value.size())
		m_renameNeedReplace = true;
	m_renameDstSeq.resize(t_value.size());
	for (int i = 0; i < t_value.size(); i++){
		m_renameDstSeq[i] = t_value[i].digitValue();
	}
	t_value=sets.value("DotOrD").toString();
	m_renameReplaceDot = t_value.toInt();
	t_value = sets.value("append").toString();
	m_renameAppend = t_value;
	sets.endGroup();

	//读取对应分类名
	sets.beginGroup("basic");
	childKey = sets.childKeys();
	sets.endGroup();
	int i, j, sz = childKey.size();
	clsRank.resize(sz);
	//读取分级表
	for (i = 0; i < sz; i++){
		if (childKey[i]=="levelNameInXml"){
			m_clsName = sets.value("basic/levelNameInXml").toString();
			clsRank.remove(i);
			break;
		}
		sets.beginGroup(childKey[i]);
		clsRank[i].clsName = childKey[i];
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
//判断str是否是纯数字(true为是)
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
	int i, j, sz = clsRank.size();
	std::string str;
	fileParameter.clear();
	for (i = 0; i < sz; i++){
		QString filename = productPath.absolutePath() + "/" + clsRank[i].clsName + ".txt";
		if (!filePath.exists(filename))	{
			QMessageBox::warning(this, "警告", filename + "路径错误,请检查后重试", QMessageBox::Ok);
			return;
		}
		std::ifstream inStream(filename.toStdWString());
		std::string info;
		while (inStream >> info){
			if (isNum(info))	continue;
			QFileInfo sInfo = QFileInfo(QString::fromStdString(info));
			QString sInfoName = sInfo.completeBaseName();
			j = sInfoName.indexOf('-');
			if (j != -1)		sInfoName = sInfoName.remove(j, sInfoName.size() - j);
			inStream >> str;
			if (!fileParameter.contains(sInfoName))		fileParameter[sInfoName].fill("", sz);
			fileParameter[sInfoName][i] = QString::fromStdString(str);
		}
	}
	ui.plainTextEdit->clear();
	for (i = 0; i < fileParameter.size(); i++){
		ui.plainTextEdit->appendPlainText(fileParameter.keys()[i]);
	}
}
void FileClassify::parReadFromXml(){
	int i, j, sz = clsRank.size();
	QStringList attributeNames;
	float m;
	for (i = 0; i < sz; i++)
		attributeNames.append(clsRank[i].clsName);
	fileParameter.clear();
	QFileInfoList folders = filePath.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
	for (QFileInfo folder : folders){
		QDir imgDir(folder.absoluteFilePath());
		QFile xmlName;
		for (QFileInfo file : imgDir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot)){
			QString filename = file.absoluteFilePath();
			if (filename.count(QRegExp(".*MSS.*_gDosMeta.xml"))){
				xmlName.setFileName(filename);
				break;
			}
			else if (filename.count(QRegExp(".*_gDosMeta.xml"))){
				xmlName.setFileName(filename);
				break;
			}
		}
		if (!xmlName.exists())
			continue;
		xmlName.open(QIODevice::ReadOnly | QIODevice::Text);
		QXmlStreamReader xml(&xmlName);
		while (!xml.atEnd()){
			xml.readNext();
			if (xml.isStartElement() && xml.name() == "element"){
				QString name = xml.attributes().value("name").toString();
				if (attributeNames.count(name)){
					j = attributeNames.indexOf(name);
					//文件名为key
					auto ImgName = folder.fileName();
					if (!fileParameter.contains(ImgName))		fileParameter[ImgName].fill("", sz);
					fileParameter[ImgName][j] = xml.readElementText();
				}
				else if (name == m_clsName){
					auto ImgName = folder.fileName();
					fileParameter[ImgName].push_back("level=" + xml.readElementText());
				}
			}
		}
	}
	ui.plainTextEdit->clear();
	for (i = 0; i < fileParameter.size(); i++){
		ui.plainTextEdit->appendPlainText(fileParameter.keys()[i]);
	}
}
void FileClassify::pathDef(){
	productPath = ui.lineEdit->text();
	filePath = ui.lineEdit_2->text();
	iniPath = ui.lineEdit_3->text();
	iniRead();
	ui.progressBar->setValue(0);
	if (inputWay == 0)
		parRead();
	else parReadFromXml();
	if (m_renameNeedReplace)
		nameReplace(), m_renameNeedReplace = false;
}
bool FileClassify::pathCheck(){
	if (!filePath.exists(iniPath)) {
		QMessageBox::warning(this, "警告", "输入路径不存在,请重试", QMessageBox::Ok);
		return false;
	}
	if (!filePath.exists()) {
		QMessageBox::warning(this, "警告", "输出路径不存在,请重试", QMessageBox::Ok);
		return false;
	}
	if (inputWay == false){
		if (!productPath.exists()) {
			QMessageBox::warning(this, "警告", "工程文件路径错误,请重试", QMessageBox::Ok);
			return false;
		}
	}
	return true;
}
void FileClassify::inputFilePath(){
	//	productPath = QFileDialog::getExistingDirectory(this, tr("工程路径"),"J:\\YangtzeRiver_Prj");
	productPath = QFileDialog::getExistingDirectory(this, tr("工程路径"), "E:\\苏州项目\\FileClassify\\测试工程");
	if (!productPath.exists())
		return;
	ui.lineEdit->setText(productPath.absolutePath());
	filePath = productPath.absolutePath() + "/product/DOM";
	ui.lineEdit_2->setText(filePath.absolutePath());
	iniRead();
}
void FileClassify::outputFilePath(){
	filePath = QFileDialog::getExistingDirectory(this, tr("输出路径"), "E:\\CodeLib\\2016-0229 FileClassify文件分类\\测试工程\\product\\DOM");
	if (!filePath.exists())
		return;
	ui.lineEdit_2->setText(filePath.absolutePath());
}
void FileClassify::inputIniPath(){
	iniPath = QFileDialog::getOpenFileName(this, tr("配置文件名称"), filePath.absolutePath(), "*.ini");
	if (!filePath.exists(iniPath))
		return;
	ui.lineEdit_3->setText(iniPath);
}
/*
//创建文件夹(成功返回true)
bool creatFolder(const char *destdir){
char cmd[1024] = "md ";
strcat(cmd, destdir);
return !system(cmd);
}*/
std::wstring pathTrans(const char *path){
	QString name = QDir::toNativeSeparators(QString::fromLocal8Bit(path));
	std::wstring ws = name.toStdWString();
	ws.push_back('\0');
	ws.push_back('\0');
	return ws;
}
//移动文件夹(成功返回true)
int moveFolder(const char* srcdir, const char* destdir){
	std::wstring ws(pathTrans(srcdir)), wd(pathTrans(destdir));
	SHFILEOPSTRUCT FileOp;
	FileOp.hwnd = 0;
	FileOp.wFunc = FO_MOVE; //执行文件拷贝
	FileOp.pFrom = ws.c_str();
	FileOp.pTo = wd.c_str();
	FileOp.fFlags = FOF_NO_UI;
	return SHFileOperation(&FileOp);
}
int deleteFolder(const char* srcdir){
	std::wstring ws(pathTrans(srcdir));
	SHFILEOPSTRUCT FileOp;
	FileOp.hwnd = 0;
	FileOp.wFunc = FO_DELETE; //执行文件拷贝
	FileOp.pFrom = ws.c_str();
	FileOp.pTo = ws.c_str();
	FileOp.fFlags = FOF_NO_UI;
	return SHFileOperation(&FileOp);
}
QString nameTrans(const QString &name){
	int i;
	for (i = 0; i < name.size(); i++) if (name[i] >= 'A'&&name[i] <= 'Z')	break;
	QString resault(name);
	return resault.remove(i + 1, name.size() - i - 1);
}
void FileClassify::subDirCreate(const QStringList& rankName){
	for (int i = 0; i < rankName.size(); i++){
		if (onlyReport == false){
			filePath.mkdir(filePath.absoluteFilePath(nameTrans(rankName[i])));
			moveFolder(QDir::toNativeSeparators(filePath.absoluteFilePath(rankName[i])).toLocal8Bit(),
				QDir::toNativeSeparators(filePath.absoluteFilePath(nameTrans(rankName[i]))).toLocal8Bit());
		}
	}
}
void FileClassify::runClassify(){
	pathDef();
	if (pathCheck() == false)
		return;
	int i = 0, j, m = 0;
	QStringList rankName = clsRank[0].rankName;
	QStringList rankName_Org = clsRank[0].rankName;
	for (i = 0; i < rankName.size(); i++){
		rankName[i] = nameTrans(rankName[i]);
		for (j = 0; j < clsRank.size(); j++){
			if (isBigLevel == true)
				rankName[i].replace("\\d?[a-z|A-Z](\\d)", "");
			else rankName[i] += '_' + QString::number(clsRank[j].minValue[i]) + '-' + QString::number(clsRank[j].maxValue[i]);
		}
	}
	char ch = 'A';
	i = 0;
	//创立分级文件夹
	qDebug() << "-------------------------start----------------------" << endl;
	if (onlyReport == false)
		while (i < rankName.size()){
			filePath.mkdir(filePath.absoluteFilePath(rankName[i]));
			i++;
		}
	qDebug() << "make directory sucess" << endl;
	//创建报告文件
	QFile file("classifyReport.txt");
	file.open(QIODevice::WriteOnly | QIODevice::Truncate);
	QTextStream text_stream(&file);
	text_stream << QString("影像名       ");
	for (auto name : clsRank)			text_stream << "  " << name.clsName;
	text_stream << QString("   等级") << endl;
	if (onlyReport == false)
		filePath.mkdir(filePath.absoluteFilePath("未分类"));
	//剪切文件夹
	QStringList keyName = fileParameter.keys();
	for (auto ite = fileParameter.begin(); ite != fileParameter.end(); ite++, m++){
		QCoreApplication::processEvents();
		QString clsName;
		i = 0, j = 0;
		bool flag = false, haveCls = false;
		for (i = 0; i < ite->size(); i++)
			if (ite->at(i).contains("level=")){
				QString nn = (*ite)[i];
				clsName = nn.replace("level=", ""), haveCls = true;
			}
		//深度优先搜素
		else while (j < rankName.size()){
			if (abs(ite->at(i).toFloat()) >= clsRank[i].minValue[j] && abs(ite->at(i).toFloat()) < clsRank[i].maxValue[j])
				if (i == clsRank.size() - 1)
					break;
				else i++;
			else i = 0, j++;
		}
		if (onlyReport == false)
			if (haveCls){
				QString t_name = filePath.absoluteFilePath(clsName);
				if (!QFile(t_name).exists())
					filePath.mkdir(t_name);
				moveFolder(QDir::toNativeSeparators(filePath.absoluteFilePath(keyName[m])).toLocal8Bit(),
					QDir::toNativeSeparators(filePath.absoluteFilePath(clsName)).toLocal8Bit());
			}
			else moveFolder(QDir::toNativeSeparators(filePath.absoluteFilePath(keyName[m])).toLocal8Bit(),
				QDir::toNativeSeparators(filePath.absoluteFilePath(rankName[j])).toLocal8Bit());
			flag = true;
			QStringList t_KeyName = keyName[m].split("_");
			if (t_KeyName.size() == 6)	{t_KeyName.removeFirst(); t_KeyName.removeFirst(); t_KeyName.removeLast();}
			text_stream << t_KeyName.join("_");
			if (!haveCls){
				for (auto name : *ite)			text_stream << "  " << name.toLocal8Bit();
				text_stream << "  " << rankName[j] << endl;
			}
			else {
				for (int kk = 0; kk < ite->size()-1;kk++)
					text_stream << "  " << ite->at(kk).toLocal8Bit();
				text_stream << "  " << clsName << endl;
			}
			if (flag == false)	{
				if (onlyReport == false)
					moveFolder(QDir::toNativeSeparators(filePath.absoluteFilePath(keyName[m])).toLocal8Bit(),
					QDir::toNativeSeparators(filePath.absoluteFilePath("未分类")).toLocal8Bit());
				text_stream << keyName[m];
				for (auto name : *ite)			text_stream << "  " << name;
				text_stream << "  " << QString("未分类") << endl;
			}
			ui.progressBar->setValue(1.0*m / keyName.size() * 100);
	}
	qDebug() << "filePar" << endl << fileParameter << endl;
	qDebug() << "-----------------------end------------------------" << endl;
	ui.progressBar->setValue(100);
	file.close();
	QMessageBox::information(this, "提示", "分类完成!", QMessageBox::Ok);
}
void FileClassify::undo(){
	if (pathCheck() == false)
		return;
	int i = 0, j, m = 0;
	QStringList rankName = clsRank[0].rankName;
	QStringList rankName_Org = clsRank[0].rankName;
	for (i = 0; i < rankName.size(); i++){
		rankName[i] = nameTrans(rankName[i]);
		for (j = 0; j < clsRank.size(); j++){
			if (isBigLevel == true)
				rankName[i].replace("\\d+[a-z|A-Z](\\d)", "");
			else rankName[i] += '_' + QString::number(clsRank[j].minValue[i]) + '-' + QString::number(clsRank[j].maxValue[i]);
		}
	}
	//创立分级文件夹
	qDebug() << "-------------------------start----------------------" << endl;
	//剪切文件夹
	QStringList keyName = fileParameter.keys();
	for (auto ite = fileParameter.begin(); ite != fileParameter.end(); ite++, m++){
		QCoreApplication::processEvents();
		QString clsName;
		i = 0, j = 0;
		bool flag = false, haveCls = false;
		for (i = 0; i < ite->size(); i++)
			if (ite->at(i).contains("level=")){
				QString nn = (*ite)[i];
				clsName = nn.replace("level=", ""), haveCls = true;
			}
		//深度优先搜素
		else while (j < rankName.size()){
			if (abs(ite->at(i).toFloat()) >= clsRank[i].minValue[j] && abs(ite->at(i).toFloat()) < clsRank[i].maxValue[j])
				if (i == clsRank.size() - 1)
					break;
				else i++;
			else i = 0, j++;
		}
		if (onlyReport == false)
			if (haveCls)
				moveFolder(QDir::toNativeSeparators(filePath.absoluteFilePath(clsName + "/" + keyName[m])).toLocal8Bit(),
				QDir::toNativeSeparators(filePath.absolutePath()).toLocal8Bit()),
				deleteFolder(QDir::toNativeSeparators(filePath.absoluteFilePath(clsName)).toLocal8Bit());
			else moveFolder(QDir::toNativeSeparators(filePath.absoluteFilePath(rankName[j] + "/" + keyName[m])).toLocal8Bit(),
				QDir::toNativeSeparators(filePath.absolutePath()).toLocal8Bit());
			flag = true;
			ui.progressBar->setValue(1.0*m / keyName.size() * 100);
	}
	qDebug() << "filePar" << endl << fileParameter << endl;
	i = 0;
	while (i < rankName.size()){
		deleteFolder(QDir::toNativeSeparators(filePath.absoluteFilePath(rankName[i])).toLocal8Bit());
		i++;
	}
	if (onlyReport == false)
		deleteFolder(QDir::toNativeSeparators(filePath.absoluteFilePath("未分类")).toLocal8Bit());
	qDebug() << "remove directory sucess" << endl;
	qDebug() << "-----------------------end------------------------" << endl;
	ui.progressBar->setValue(100);
	QMessageBox::information(this, "提示", "重做完成!", QMessageBox::Ok);
}
void FileClassify::nameReplace(){
	int j = 0;
	for (auto ite = fileParameter.begin(); ite != fileParameter.end(); ite++,j++){
		QString t_nameRef = ite.key();
		QVector<QString> t_value = *ite;
		fileParameter.remove(t_nameRef);
		QStringList t_srcNameList = t_nameRef.split(m_renameSplitChar),t_dstNameList;
		for (int i = 0; i < m_renameDstSeq.size(); i++){
			if (m_renameReplaceDot)
				t_srcNameList[m_renameDstSeq[i]].replace("\.", "D");
			t_dstNameList << t_srcNameList[m_renameDstSeq[i]];
		}
		t_dstNameList << m_renameAppend;
		t_nameRef = t_dstNameList.join(m_renameSplitChar);
		fileParameter[t_nameRef] = t_value;
		ite = fileParameter.begin() + j;
	}
}