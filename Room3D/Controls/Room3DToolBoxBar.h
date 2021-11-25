//----------------------------------------------//
//	File: Room3DToolBoxBar.h					//
//	Desc: 										//
//	Update: 2012.6  (by 001	)				//
//----------------------------------------------//
#pragma once
#include <QFrame>
#include "Room3DStep1ToolBox.h"
#include "Room3DStep2ToolBox.h"
#include "Room3DStep3ToolBox.h"
#include <QStackedWidget>
#include <QVBoxLayout>
/*!
	\class CRoom3DToolBoxBar
	\brief This is toolboxbar class to contain tool box acording to processing step.(by 001, 2012/6/21).
\sa 
*/
class CRoom3DToolBoxBar : public QFrame
{
	Q_OBJECT
public:
	CRoom3DToolBoxBar(QWidget* parent = 0);
	~CRoom3DToolBoxBar(void);
	void UpdateControlState(void* a_pUserData = NULL);
protected:
	void InitControls();
public: //protected:
	CRoom3DStep1ToolBox* m_pStep1ToolBox;
	CRoom3DStep2ToolBox* m_pStep2ToolBox;
	CRoom3DStep3ToolBox* m_pStep3ToolBox;
	QStackedWidget *	m_pStackedWidget;
	QVBoxLayout *		m_pLayout;

};
