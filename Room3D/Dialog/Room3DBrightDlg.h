//----------------------------------------------//
//	File: Room3DBrightDlg.h						//
//	Desc: 										//
//	Update: 2012.7  (by 001	)				//
//----------------------------------------------//
#pragma once
#include "Room3DBaseDlg.h"
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QSlider>

class CRoom3DBrightDlg : public CRoom3DBaseDlg
{
	Q_OBJECT
public:
	CRoom3DBrightDlg(int a_nBright, int a_nContrast, QWidget* a_pParent = NULL);
	virtual ~CRoom3DBrightDlg(void);
	void GetBrightAndContrast(int& a_nBright, int& a_nContrast){a_nContrast = m_nContrast; a_nBright = m_nBright;}
protected:
	void InitControls();
protected slots:
	void verify();
	void OnCancel();
	void OnClickedButton(QAbstractButton* a_pButton);
	void OnBrightValueChanged(int a_nValue);
	void OnContrastValueChanged(int a_nValue);
	void OnPressDefaultBtn();
protected:
	QGridLayout *m_pMainLayout;
	QLabel *m_pBrightnessLbl;
	QLabel *m_pContrastLbl;
	QSlider *m_pBrightSlider;
	QSlider *m_pContrastSlider;
	QDialogButtonBox *m_pButtonBox;
	QPushButton* m_pDefaultButton;
protected:
	int m_nBright;
	int m_nContrast;
	int m_nOrgBright;
	int m_nOrgContrast;
	bool m_blDefaultSetting;
};
