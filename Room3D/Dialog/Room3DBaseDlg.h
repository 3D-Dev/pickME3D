//----------------------------------------------//
//	File: Room3DBaseDlg.h						//
//	Desc: 										//
//	Update: 2012.7  (by 001	)				//
//----------------------------------------------//
#pragma once
#include <QDialog>

/*!
	\class CRoom3DBaseDlg
	\brief This is base class of all of the dialogs.(by 001, 2012/7/7).
	\sa 
*/
class CRoom3DBaseDlg : public QDialog
{
	Q_OBJECT
public:
	CRoom3DBaseDlg(QWidget* a_pParent = NULL);
	~CRoom3DBaseDlg(void);
public:
	void paintEvent(QPaintEvent *e);
};
