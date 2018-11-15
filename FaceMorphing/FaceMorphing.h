#pragma once

#include <QtWidgets/QWidget>
#include "ClickLabel.h"
#include "FaceMorpher.h"
#include <QComboBox>
#include <QSpinBox>

class FaceMorphing : public QWidget
{
	Q_OBJECT

public:
	FaceMorphing(QWidget *parent = Q_NULLPTR);
	void init();

private:
	QLabel *pMorphLabel;
	QComboBox *pMorphCbox;
	QLabel *pBSHLabel;
	QSpinBox *pBSHSbox;
	QLabel *pInterpLabel;
	QComboBox *pInterpCbox;

	ClickLabel *pSrc;
	QLabel *pSrcLabel;
	ClickLabel *pArrow;
	ClickLabel *pTpl;
	QLabel *pTplLabel;
	ClickLabel *pEquals;
	ClickLabel *pResult;
	QLabel *pResultLabel;

	QString srcImgFilename;
	QString tplImgFilename;
	QString outputFilename;
	QStringList morphFunctions;
	QStringList interpMethods;
	bool inputChanged;
	FaceMorpher fm;

	void loadImage(bool isSrc);
	void getSavePath();
	void exchangeImage();
	void startMorph(int morphFunc, int interpMethod, bool detect = false);
	void getOuputFileName();
protected slots:
	void setBSH(int h);//set the spacing of lattice in BSpline
	void updateSpinbox();
};
int mkpath(char* file_path);

