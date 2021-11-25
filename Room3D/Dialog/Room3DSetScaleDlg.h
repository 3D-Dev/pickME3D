#pragma once
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QLabel>
#include "Room3DBaseDlg.h"
#include "define.h"

class CRoom3DSetScaleDlg : public CRoom3DBaseDlg
{
	Q_OBJECT
public:
	CRoom3DSetScaleDlg(QWidget* a_pParent);
	~CRoom3DSetScaleDlg(void);

	int getScaleAxis();
	double getScale();
protected slots:
	void verify();
protected:
	void InitControls();
protected:
	QVBoxLayout*			m_pMainLayout;
	QHBoxLayout*			m_pScaleLayout;
	QDoubleSpinBox*			m_pScaleSpinBox;
#ifdef _SUPPORT_STEP2_TOOLBOX_CLASSIC
	QComboBox*				m_pAxisComboBox;
#endif
	QLabel*					m_pScaleUnitLbl;

	QDialogButtonBox *		m_pButtonBox;
};
