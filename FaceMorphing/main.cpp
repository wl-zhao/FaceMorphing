#include "FaceMorphing.h"
#include "B_Spline.h"
#include <opencv2/opencv.hpp>
#include <QtWidgets/QApplication>
#include <iostream>
#include <QDebug>
#include <QTranslator>
#include <QFontDatabase>
using namespace std;
#include <QDir>

int handleError(int status, const char* func_name,
	const char* err_msg, const char* file_name,
	int line, void* userdata)
{
	//Do nothing -- will suppress console output
	return 0;   //Return value is not used
}

void setStyle(const QString &style) {
	QFile qss(style);
	qss.open(QFile::ReadOnly);
	qApp->setStyleSheet(qss.readAll());
	qss.close();
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QFont f(QString::fromLocal8Bit("»ªÎÄ¿¬Ìå"), 13);
	a.setFont(f);
	QTranslator *trans = new QTranslator;
    int ret = trans->load("./Resources/facemorphing_zh.qm");
    QDir dir;
    qDebug() << QDir::current();
    qDebug() << ret;
	qApp->installTranslator(trans);
    setStyle("./Resources/white.qss");

	cv::redirectError(handleError);
	FaceMorphing w;
	w.show();
	return a.exec();
}
