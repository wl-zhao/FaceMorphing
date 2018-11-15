#include "ClickLabel.h"

ClickLabel::ClickLabel(QWidget* parent, Qt::WindowFlags f)
	: QLabel(parent) {
	setMinimumSize(1, 1);
}

QPixmap ClickLabel::scaledPixmap() const
{
	return pix.scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

ClickLabel::~ClickLabel() {}

void ClickLabel::setDefaultPixmap(QString filename)
{
	default_pix = QPixmap(filename);
	if (!default_pix.isNull())
		setPixmap(default_pix);
}

void ClickLabel::setPixmap(QString filename)
{
	setPixmap(QPixmap(filename));
}

void ClickLabel::resizeEvent(QResizeEvent *)
{
	if (!pix.isNull())
		QLabel::setPixmap(scaledPixmap());
}

void ClickLabel::resetPixmap()
{
	setPixmap(default_pix);
}

void ClickLabel::mousePressEvent(QMouseEvent* event) {
	emit clicked();
}

void ClickLabel::setPixmap(const QPixmap & p)
{
	pix = p;
	QLabel::setPixmap(p);
	this->adjustSize();
}
