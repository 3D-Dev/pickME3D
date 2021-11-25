//----------------------------------------------//
//	File: Room3DMenu.h						//
//	Desc: 										//
//	Update: 2012.8  (by 002	)					//
//----------------------------------------------//
#pragma once
#include <QMenu>

//----------------------------------------------
//	class : CRoom3DAction
//----------------------------------------------
#define ACTION_FONT_NORMAL_SIZE 14
#define ACTION_FONT_BIG_SIZE 16
#define ACTION_FONT_FAMILY "Cordia New"
#define ACTION_FONT_SPACING 100
#define ACTION_FONT_STRETCH 100

#define ACTION_ACTIVE_ALPHA 63
#define ACTION_NORMAL_ALPHA 31

class CRoom3DAction : public QAction
{
	Q_OBJECT

public:
	CRoom3DAction(QWidget* pParent);
	~CRoom3DAction();

	bool isStatic();
	void setStatic(bool bStatic);
protected:
	bool m_bStatic;
};

//----------------------------------------------
//	class : CRoom3DMenu
//----------------------------------------------
class CRoom3DMenu :	public QMenu
{
	Q_OBJECT

public:
	CRoom3DMenu(QWidget* pParent);
	~CRoom3DMenu();

	void SetRegion(const QRegion& region);
protected:
	virtual void resizeEvent(QResizeEvent *e);
	virtual void paintEvent(QPaintEvent *e);

protected:
	QRegion m_sRegion;
};
