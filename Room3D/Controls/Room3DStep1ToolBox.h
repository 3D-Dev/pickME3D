//----------------------------------------------//
//	File: Room3DStep1ToolBox.h								//
//	Desc: 										//
//	Update: 2012.6  (by 001	)				//
//----------------------------------------------//

#pragma once
#include <QWidget>
#include "Room3DImageButton.h"
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QTextEdit>

class CRoom3DBaseToolBox : public QWidget
{
	Q_OBJECT
public:
	CRoom3DBaseToolBox(QWidget* a_pParent);
	virtual ~CRoom3DBaseToolBox();
protected:
	void InitBkWidget();
protected:
	CRoom3DImageWidget* m_pBkWidget;
	QVBoxLayout*		m_pBaseLayout;
};


class CRoom3DStep1ToolBox : public CRoom3DBaseToolBox
{
	Q_OBJECT
	enum BUTTON_ID
	{
		BTN_BROWSE = 0,
		BTN_TUTORIAL
	};
public:
	CRoom3DStep1ToolBox(QWidget* parent = 0);
	~CRoom3DStep1ToolBox(void);
	void UpdateControlState();
protected:
	void InitControls();
protected slots:
		void pressedButton(int a_nBtnID);
public: //protected:
	QVBoxLayout*	m_pVerticalLayout;

	QHBoxLayout*		m_pBrowseLayout;
	CRoom3DImageButton*	m_pBrowseBtn;

	QFrame*			m_pSeparator;

	QHBoxLayout*	m_pCommentLayout;
	QTextEdit*		m_pComment;

};
