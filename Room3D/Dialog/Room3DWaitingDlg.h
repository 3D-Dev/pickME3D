#ifndef ROOM3DWAITINGDLG_H
#define ROOM3DWAITINGDLG_H

#include <QWidget>
#include <QtGui>
#include "Common.h"
#include "Room3DBaseDlg.h"

class CRoom3DWaitingDlg : public CRoom3DBaseDlg
{
	Q_OBJECT

public:
	CRoom3DWaitingDlg(QWidget* a_parent = NULL);
	~CRoom3DWaitingDlg(void);

protected:
	void timerEvent ( QTimerEvent * event );
	void showEvent(QShowEvent * event);
	void hideEvent(QHideEvent* event);

protected:
	void paintEvent(QPaintEvent *event);
	void resizeEvent(QResizeEvent *event);
	void DrawBackground();
	void DrawLoadingText();
	void DrawRotation();

protected:
	int	m_nRotIndex;
	int m_nTimerID;
};

#endif