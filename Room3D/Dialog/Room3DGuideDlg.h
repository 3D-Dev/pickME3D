#ifndef ROOM3DGUIDEDLG_H
#define ROOM3DGUIDEDLG_H

#include <QWidget>
#include <QtGui>
#include "Common.h"

class CRoom3DGuideDlg : public QWidget
{
	Q_OBJECT

public:
	CRoom3DGuideDlg(QWidget* a_parent = NULL);
	~CRoom3DGuideDlg(void);
protected:
	void timerEvent ( QTimerEvent * event );
	void showEvent(QShowEvent * event);
protected:
	void mousePressEvent(QMouseEvent *event);
	void paintEvent(QPaintEvent *event);
	void resizeEvent(QResizeEvent *event);

protected:
	QPixmap tmpPixmap;
	QBitmap maskBitmap;
	float	m_fOpacity;
};

#endif