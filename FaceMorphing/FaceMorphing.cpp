#include "FaceMorphing.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QtWidgets/QApplication>
#include <direct.h>

FaceMorphing::FaceMorphing(QWidget *parent)
	: QWidget(parent)
{
	init();
}

void FaceMorphing::init()
{
	setObjectName("centerWidget");
	setWindowTitle(tr("FaceMorphing"));
    setWindowIcon(QIcon("./Resources/logo1.ico"));
	//Layout
	pMorphLabel = new QLabel(tr("Morph Function"), this);
	pMorphLabel->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	pMorphCbox = new QComboBox(this);
	pMorphCbox->setMinimumHeight(20);
	pBSHSbox = new QSpinBox(this);
	pBSHLabel = new QLabel(tr("Lattice Spacing"), this);
	pBSHLabel->setAlignment(Qt::AlignVCenter | Qt::AlignRight);


	pInterpLabel = new QLabel(tr("Interpolation Method"), this);
	pInterpLabel->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	pInterpCbox = new QComboBox(this);
	pInterpCbox->setMinimumHeight(20);

	pSrc = new ClickLabel(this);
	pSrc->setAlignment(Qt::AlignCenter);
	pArrow = new ClickLabel(this);
	pArrow->setAlignment(Qt::AlignCenter);
	pTpl = new ClickLabel(this);
	pTpl->setAlignment(Qt::AlignCenter);
	pEquals = new ClickLabel(this);
	pEquals->setAlignment(Qt::AlignCenter);
	pResult = new ClickLabel(this);
	pResult->setAlignment(Qt::AlignCenter);

	pSrcLabel = new QLabel(tr("Source"), this);
	pSrcLabel->setAlignment(Qt::AlignCenter);
	pTplLabel = new QLabel(tr("Destination"), this);
	pTplLabel->setAlignment(Qt::AlignCenter);
	pResultLabel = new QLabel(tr("Result"), this);
	pResultLabel->setAlignment(Qt::AlignCenter);

	pBSHSbox->setVisible(false);
	pBSHLabel->setVisible(false);

	auto mainlayout = new QVBoxLayout(this);
	auto hLayout = new QHBoxLayout;

	auto subhLayout = new QHBoxLayout;
	subhLayout->addWidget(pMorphLabel, 0);
	subhLayout->addWidget(pMorphCbox, 0);
	hLayout->addLayout(subhLayout, 1);

	subhLayout = new QHBoxLayout;
	subhLayout->addWidget(pBSHLabel, 0);
	subhLayout->addWidget(pBSHSbox, 0);
	hLayout->addLayout(subhLayout, 1);

	subhLayout = new QHBoxLayout;
	subhLayout->addWidget(pInterpLabel, 0);
	subhLayout->addWidget(pInterpCbox, 0);
	hLayout->addLayout(subhLayout, 1);

	mainlayout->addLayout(hLayout, 1);
	hLayout->setContentsMargins(0, 0, 0, 0);

	hLayout = new QHBoxLayout;
	hLayout->addWidget(pSrc, 2);
	hLayout->addWidget(pArrow, 1);
	hLayout->addWidget(pTpl, 2);
	hLayout->addWidget(pEquals, 1);
	hLayout->addWidget(pResult, 2);

	mainlayout->addLayout(hLayout, 5);

	hLayout = new QHBoxLayout;
	hLayout->addWidget(pSrcLabel, 2);
	hLayout->addStretch(1);
	hLayout->addWidget(pTplLabel, 2);
	hLayout->addStretch(1);
	hLayout->addWidget(pResultLabel, 2);
	mainlayout->addLayout(hLayout, 1);

	//Options
	morphFunctions << tr("TPS") << tr("B-Spline");
	pMorphCbox->addItems(morphFunctions);
	morphFunctions.clear();
	morphFunctions << "TPS" << "B_Spline";
	interpMethods << tr("nearest neighbor") << tr("bilinear") << tr("bicubic");
	pInterpCbox->addItems(interpMethods);
	interpMethods.clear();
	interpMethods << "nearest_neighbor" << "bilinear" << "bicubic";

	//Images
    QString prefix = "./Resources/";
    pSrc->setDefaultPixmap(prefix + "add.png");
    pArrow->setDefaultPixmap(prefix + "exchange.png");
    pTpl->setDefaultPixmap(prefix + "add.png");
    pEquals->setDefaultPixmap(prefix + "equals.png");
    pResult->setDefaultPixmap(prefix + "question.png");

	//variables
	inputChanged = false;

	//connect
	connect(pSrc, &ClickLabel::clicked, [&]() {
		loadImage(true);
	});
	connect(pTpl, &ClickLabel::clicked, [&]() {
		loadImage(false);
	});
	connect(pArrow, &ClickLabel::clicked, [&]() {
		exchangeImage();
	});
	connect(pEquals, &ClickLabel::clicked, [&]() {
		startMorph(pMorphCbox->currentIndex(), pInterpCbox->currentIndex());
	});
	connect(pResult, &ClickLabel::clicked, [&]() {
		getSavePath();
	});
	connect(pInterpCbox, &QComboBox::currentTextChanged, [&]() {
		inputChanged = true;
	});
	connect(pMorphCbox, &QComboBox::currentTextChanged, [&]() {
		inputChanged = true;
		if (pMorphCbox->currentIndex() == 0)
		{
			pBSHSbox->setVisible(false);
			pBSHLabel->setVisible(false);
		}
		else
		{
			pBSHSbox->setVisible(true);
			pBSHLabel->setVisible(true);
			updateSpinbox();
		}
	});
	connect(pBSHSbox, SIGNAL(valueChanged(int )), this, SLOT(setBSH(int)));
}

void FaceMorphing::loadImage(bool isSrc)
{
	QString filename;
	if (isSrc)
		filename = QFileDialog::getOpenFileName(this, tr("Open Source Image"), "../picture/", tr("Images (*.png *.jpg *.bmp)"));
	else
		filename = QFileDialog::getOpenFileName(this, tr("Open Destination Image"), "../picture/", tr("Images (*.png *.jpg *.bmp)"));

	if (filename.isEmpty())
		return;

	if (isSrc && srcImgFilename != filename)
	{
		inputChanged = true;
		pSrc->setPixmap(filename);
		srcImgFilename = filename;
	}
	else if (!isSrc && tplImgFilename != filename)
	{
		inputChanged = true;
		pTpl->setPixmap(filename);
		tplImgFilename = filename;
	}

	if (pMorphCbox->currentIndex() == 1)
	{
		updateSpinbox();
	}
}

void FaceMorphing::getSavePath()
{
	auto tips = tr("Save File");
	outputFilename = QFileDialog::getSaveFileName(this, tips, "../picture/results/", tr("Images (*.png *.jpg *.bmp)"));
}

void FaceMorphing::exchangeImage()
{
	auto tmpPix = pSrc->pix;
	pSrc->setPixmap(pTpl->pix);
	pTpl->setPixmap(tmpPix);
	auto tmpFilename = srcImgFilename;
	srcImgFilename = tplImgFilename;
	tplImgFilename = tmpFilename;
	inputChanged = true;
}

void FaceMorphing::startMorph(int morphFunc, int interpMethod, bool detect)
{
	if (!inputChanged)
		return;
	getOuputFileName();
	inputChanged = false;
	if (srcImgFilename == tplImgFilename)
	{
		pResult->setPixmap(srcImgFilename);
		return;
	}

	pEquals->setPixmap("./Resources/waiting.png");
	qApp->processEvents();
	try
	{
		fm.Morph(srcImgFilename.toStdString(), tplImgFilename.toStdString(), morphFunc, interpMethod, outputFilename.toStdString());
	}
	catch (const std::exception& e)
	{
		qDebug() << e.what();
		if (srcImgFilename.isEmpty() || tplImgFilename.isEmpty())
		{
			QMessageBox::warning(this, tr("No input images"), tr("Please add images first"), QMessageBox::Ok);
			pEquals->resetPixmap();
			return;
		}
		int ret = QMessageBox::warning(this, tr(e.what()), tr("Detect control points?"), QMessageBox::Ok | QMessageBox::Cancel);
		if (ret == QMessageBox::Ok)
		{
			try
			{
				fm.Morph(srcImgFilename.toStdString(), tplImgFilename.toStdString(), morphFunc, interpMethod, outputFilename.toStdString(), true);
			}
			catch (const std::exception & detect_e)
			{
				QMessageBox::warning(this, tr((tr("Can't find face in") + detect_e.what()).toStdString().c_str()), tr("Please select another first"), QMessageBox::Ok);
				if (detect_e.what() == srcImgFilename)
				{
					srcImgFilename = "";
					pSrc->resetPixmap();
				}
				if (detect_e.what() == tplImgFilename)
				{
					srcImgFilename = "";
					pSrc->resetPixmap();
				}
			}
		}
	}
	pEquals->resetPixmap();
	qApp->processEvents();
	if (!outputFilename.isEmpty())
		pResult->setPixmap(outputFilename);
	else
		pResult->resetPixmap();
	outputFilename = "";
}

void FaceMorphing::getOuputFileName()
{
	if (!outputFilename.isEmpty())
		return;
	int idx1 = srcImgFilename.lastIndexOf("/");
	int idx2 = srcImgFilename.lastIndexOf(".");
	int idx3 = tplImgFilename.lastIndexOf("/");
	int idx4 = tplImgFilename.lastIndexOf(".");
	auto dir = srcImgFilename.mid(0, idx1) + "/result/" + morphFunctions[pMorphCbox->currentIndex()] + "/" + interpMethods[pInterpCbox->currentIndex()] + "/";
	char *s = new char[dir.size() + 1];
	strcpy(s, dir.toStdString().c_str());
	cout << s;
	if (mkpath(s) == -1)
	{
		if (errno == EEXIST) {
			// alredy exists
		}
		else {
			// something else
			std::cout << "cannot create folder error:" << strerror(errno) << std::endl;
		}
	}
	outputFilename = dir + srcImgFilename.mid(idx1 + 1, idx2 - idx1 - 1) + "_to_" + tplImgFilename.mid(idx3 + 1, idx4 - idx3 - 1) + \
		srcImgFilename.mid(idx2, srcImgFilename.size());
	qDebug() << outputFilename;
	delete s;
}

void FaceMorphing::updateSpinbox()
{
	int minsize = (pSrc->pix.width() > pSrc->pix.height()) ? pSrc->pix.height() : pSrc->pix.width();
	pBSHSbox->setRange(1, minsize);
	pBSHSbox->setValue(minsize / 15);
}

int mkpath(char* file_path) {
	char* p;
	for (p = strchr(file_path + 1, '/'); p; p = strchr(p + 1, '/')) {
		*p = '\0';
		if (_mkdir(file_path) == -1) {
			if (errno != EEXIST) { *p = '/'; return -1; }
		}
		*p = '/';
	}
	return 0;
}

void FaceMorphing::setBSH(int h)
{
	inputChanged = true;
	fm.setBSplineH(h);
}