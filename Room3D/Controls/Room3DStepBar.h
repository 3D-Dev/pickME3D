//----------------------------------------------//
//	File: Room3DStepBar.h						//
//	Desc: 										//
//	Update: 2012.6  (by 001	)				//
//----------------------------------------------//
#pragma once
#include <QFrame>
#include "Room3DImageButton.h"
#include <QHBoxLayout>

/*!
	\class CRoom3DStepBar
	\brief This is menubar class to contain step button.(by 001, 2012/6/21).
\sa 
*/
class CRoom3DStepBar : public QFrame
{
	Q_OBJECT
public:
	CRoom3DStepBar(QWidget* parent = 0);
	~CRoom3DStepBar(void);
	void UpdateControlState();
protected:
	void InitControls();

protected:
	CRoom3DImageWidget* m_pBkWidget;
	QHBoxLayout*	m_pBkLayout;
	// CRoom3DImageButton* m_pLangBtn;
	QHBoxLayout* m_pMainLayout;
	CRoom3DImageWidget* m_pStepBtn;
	CRoom3DImageWidget* m_pStepComment;
	QVBoxLayout* m_pStepCommentLayout;
	QLabel*		m_pStep1CommentLbl;
	QLabel*		m_pStep2CommentLbl_1;
	QLabel*		m_pStep2CommentLbl_2;
	QLabel*		m_pStep3CommentLbl_1;
	QLabel*		m_pStep3CommentLbl_2;
};
