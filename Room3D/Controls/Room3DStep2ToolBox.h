//----------------------------------------------//
//	File: Room3DStep2ToolBox					//
//	Desc: 										//
//	Update: 2012.6  (by 001	)				//
//----------------------------------------------//
#pragma once
#include <QWidget>
#include <QCheckBox>
#include "Room3DImageButton.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include "Room3DStep1ToolBox.h"

#ifdef _SUPPORT_STEP2_TOOLBOX_CLASSIC
class CRoom3DStep2ToolBox : public CRoom3DBaseToolBox
{
	Q_OBJECT
	enum BUTTON_ID
	{
		BTN_SET_FLOOR,
		CHECK_BTN_SET_FLOOR,
		BTN_SET_SCALE,
		CHECK_BTN_SET_SCALE,
		BTN_TUTORIAL
	};
public:
	CRoom3DStep2ToolBox(QWidget* parent = 0);
	~CRoom3DStep2ToolBox(void);
	void UpdateControlState();
	void ResetState();
protected:
	void InitCotrols();
protected slots:
	void OnStateChangedOfFloorCheckBox(int a_nState);
	void OnStateChangedOfScaleCheckBox(int a_nState);
	void pressedButton(int a_nBtnID);
public: //protected:
	QVBoxLayout*	m_pVerticalLayout;
	QHBoxLayout*	m_pSetFloorHorizontalLayout;
	CRoom3DImageButton*	m_pSetFloorBtn;
	QCheckBox*			m_pSetFloorCheckBox;

	QHBoxLayout*		m_pSetScaleHorizontalLayout;
	CRoom3DImageButton*	m_pSetScaleBtn;
	QCheckBox*			m_pSetScaleCheckBox;

	QSpacerItem*		m_pVerticalSpacer;

	QTextEdit*		m_pSetFloorComment;
	QTextEdit*		m_pSetScaleComment;

	QFrame*			m_pSeparator1;
};
#else

class CRoom3DStep2ToolBox : public CRoom3DBaseToolBox
{
	Q_OBJECT
		enum BUTTON_ID
	{
		BTN_SET_CORNER,
		BTN_SET_HEIGHT,
		BTN_CALIBRATE,
		CHECK_BTN_FINISH_SETTING,
	};
public:
	CRoom3DStep2ToolBox(QWidget* parent = 0);
	~CRoom3DStep2ToolBox(void);
	void UpdateControlState();
	void ResetState();
protected:
	void InitCotrols();
protected slots:
	void OnFinishSetting(int a_nState);
	void pressedButton(int a_nBtnID);
public: //protected:
	QVBoxLayout*	m_pVerticalLayout;

	QVBoxLayout*	m_pSetCornerLayout;
	QHBoxLayout*	m_pSetCornerBtnLayout;
	CRoom3DImageButton*	m_pSetCornerBtn;
	QTextEdit*		m_pSetCornerComment;
	QFrame*			m_pSeparator1;

	QVBoxLayout*	m_pSetScaleLayout;
	QHBoxLayout*	m_pSetScaleBtnLayout;
	CRoom3DImageButton*	m_pSetScaleBtn;
	QTextEdit*		m_pSetScaleComment;
	QFrame*			m_pSeparator2;

	QVBoxLayout*	m_pCalibrateLayout;
	QHBoxLayout*	m_pCalibrateBtnLayout;
	CRoom3DImageButton*	m_pCalibrateBtn;
	QTextEdit*			m_pCalibrateComment;

	QHBoxLayout*		m_pFinishSettingLayout;
	QCheckBox*			m_pFinishSettingCheckBox;
};

#endif
