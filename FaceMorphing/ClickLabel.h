#pragma once
#include <QLabel>
#include <QWidget>
#include <Qt>
#include <QPixmap>
#include <QResizeEvent>
#include <QDebug>

//Clickable Label, for displaying images
class ClickLabel : public QLabel
{
	Q_OBJECT
public:
	explicit ClickLabel(QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
	QPixmap scaledPixmap() const;
	QPixmap default_pix;
	QPixmap pix;
	~ClickLabel();

signals:
	void clicked();
public slots:
	void setDefaultPixmap(QString filename);
	void setPixmap(QString filename);
	void setPixmap(const QPixmap &);
	void resizeEvent(QResizeEvent *);
	void resetPixmap();
protected:
	void mousePressEvent(QMouseEvent *event);
};

