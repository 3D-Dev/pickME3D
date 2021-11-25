#include "Room3DSetScaleDlg.h"
#include "common.h"

CRoom3DSetScaleDlg::CRoom3DSetScaleDlg(QWidget* a_pParent) : CRoom3DBaseDlg(a_pParent)
{
	InitControls();
	this->setWindowTitle(tr("Input Scale"));
}

CRoom3DSetScaleDlg::~CRoom3DSetScaleDlg(void)
{
	SAFE_DELETE(m_pMainLayout);
#ifdef _SUPPORT_STEP2_TOOLBOX_CLASSIC
	SAFE_DELETE(m_pAxisComboBox);
#endif
	SAFE_DELETE(m_pScaleSpinBox);
	SAFE_DELETE(m_pButtonBox);
}

int CRoom3DSetScaleDlg::getScaleAxis()
{
#ifdef _SUPPORT_STEP2_TOOLBOX_CLASSIC
	return m_pAxisComboBox->currentIndex();
#else
	return 1;// Y axis
#endif
}
double CRoom3DSetScaleDlg::getScale()
{
	return m_pScaleSpinBox->value();
}

void CRoom3DSetScaleDlg::InitControls()
{
#ifdef _SUPPORT_STEP2_TOOLBOX_CLASSIC
	this->setFixedSize(QSize(300,80));
#else
	this->setFixedSize(QSize(200,80));
#endif
	m_pMainLayout = new QVBoxLayout(this);
	m_pScaleLayout = new QHBoxLayout();
	m_pMainLayout->addLayout(m_pScaleLayout);
#ifdef _SUPPORT_STEP2_TOOLBOX_CLASSIC
	m_pAxisComboBox = new QComboBox(this);
#endif
	m_pScaleSpinBox = new QDoubleSpinBox(this);
	m_pScaleSpinBox->setSizePolicy(QSizePolicy::Expanding , QSizePolicy::Preferred);
	m_pScaleUnitLbl = new QLabel(this);
	m_pScaleUnitLbl->setSizePolicy(QSizePolicy::Minimum , QSizePolicy::Preferred);
	m_pScaleSpinBox->setMinimum(0);
	m_pScaleSpinBox->setMaximum(10000);
	m_pScaleSpinBox->setValue(100.0f);
	m_pScaleSpinBox->setSingleStep(10.f);
#ifdef _SUPPORT_STEP2_TOOLBOX_CLASSIC
	m_pScaleLayout->addWidget(m_pAxisComboBox);
#endif
	m_pScaleLayout->addWidget(m_pScaleSpinBox);
	m_pScaleLayout->addWidget(m_pScaleUnitLbl );	

	m_pButtonBox = new QDialogButtonBox(this);
	m_pButtonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
	m_pMainLayout->addWidget(m_pButtonBox);

	connect(m_pButtonBox, SIGNAL(accepted()), this, SLOT(verify()));
	connect(m_pButtonBox, SIGNAL(rejected()), this, SLOT(reject()));

#ifdef _SUPPORT_STEP2_TOOLBOX_CLASSIC
	m_pAxisComboBox->addItem(STR_CAMERA_X_AXIS);
	m_pAxisComboBox->addItem(STR_CAMERA_Y_AXIS);
	m_pAxisComboBox->addItem(STR_CAMERA_Z_AXIS);
#endif
	m_pScaleUnitLbl->setText(STR_CENTIMETER);
}

void CRoom3DSetScaleDlg::verify()
{
	accept();
}
