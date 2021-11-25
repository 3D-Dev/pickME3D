#include "Room3DBrightDlg.h"
#include "Define.h"
#include "Room3DApplication.h"
#include "Room3DUtility.h"

CRoom3DBrightDlg::CRoom3DBrightDlg(int a_nBright, int a_nContrast, QWidget* a_pParent) : CRoom3DBaseDlg(a_pParent)
{
	m_nBright = a_nBright;
	m_nContrast = a_nContrast;
	m_nOrgBright = m_nBright;
	m_nOrgContrast = m_nContrast;
	InitControls();
	this->setWindowTitle(STR_BRIGHT_DLG_TITLE);
	m_blDefaultSetting = false;
}

CRoom3DBrightDlg::~CRoom3DBrightDlg(void)
{
}

void CRoom3DBrightDlg::InitControls()
{
	this->setFixedSize(400, 117);
	m_pMainLayout = new QGridLayout(this);
	m_pMainLayout->setObjectName(QString::fromUtf8("m_pMainLayout"));
	m_pBrightnessLbl = new QLabel(this);
	m_pBrightnessLbl->setObjectName(QString::fromUtf8("m_pBrightnessLbl"));

	m_pMainLayout->addWidget(m_pBrightnessLbl, 1, 0, 1, 1);

	m_pContrastLbl = new QLabel(this);
	m_pContrastLbl->setObjectName(QString::fromUtf8("m_pContrastLbl"));

	m_pMainLayout->addWidget(m_pContrastLbl, 3, 0, 1, 1);

	m_pBrightSlider = new QSlider(this);
	m_pBrightSlider->setObjectName(QString::fromUtf8("m_pBrightSlider"));
	m_pBrightSlider->setOrientation(Qt::Horizontal);

	m_pMainLayout->addWidget(m_pBrightSlider, 1, 1, 1, 1);

	m_pContrastSlider = new QSlider(this);
	m_pContrastSlider->setObjectName(QString::fromUtf8("m_pContrastSlider"));
	m_pContrastSlider->setOrientation(Qt::Horizontal);

	m_pMainLayout->addWidget(m_pContrastSlider, 3, 1, 1, 1);

	m_pDefaultButton = new QPushButton(this);
	//m_pDefaultButton->setMaximumWidth(40);
	//m_pDefaultButton->setMinimumWidth(40);
	m_pMainLayout->addWidget(m_pDefaultButton, 4, 0, 1, 1);

	m_pButtonBox = new QDialogButtonBox(this);
	m_pButtonBox->setObjectName(QString::fromUtf8("m_pButtonBox"));
	m_pButtonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

	m_pMainLayout->addWidget(m_pButtonBox, 4, 1, 1, 1);

	m_pBrightSlider->setRange(MIN_BRIGHTNESS, MAX_BRIGHTNESS);
	m_pBrightSlider->setValue(m_nBright);
	m_pContrastSlider->setRange(MIN_CONTRAST, MAX_CONTRAST);
	m_pContrastSlider->setValue(m_nContrast);

	connect(m_pButtonBox, SIGNAL(accepted()), this, SLOT(verify()));
	connect(m_pButtonBox, SIGNAL(rejected()), this, SLOT(OnCancel()));
	connect(m_pBrightSlider, SIGNAL(valueChanged(int)), this, SLOT(OnBrightValueChanged(int)));
	connect(m_pContrastSlider, SIGNAL(valueChanged(int)), this, SLOT(OnContrastValueChanged(int)));
	connect(m_pDefaultButton, SIGNAL(pressed()), this, SLOT(OnPressDefaultBtn()));

	m_pBrightnessLbl->setText(STR_BRIGHTNESS);
	m_pContrastLbl->setText(STR_CONTRAST);
	m_pDefaultButton->setText(STR_SET_DEFAULT);
}

void CRoom3DBrightDlg::OnClickedButton(QAbstractButton* a_pButton)
{
}

void CRoom3DBrightDlg::OnBrightValueChanged(int a_nValue)
{
	if(!m_blDefaultSetting)
	{
		m_nBright = a_nValue;
		g_pApp->GetDocument()->SetBright(m_nBright);
		g_pApp->GetMainWidget()->UpdateView();
	}
}

void CRoom3DBrightDlg::OnContrastValueChanged(int a_nValue)
{
	if(!m_blDefaultSetting)
	{
		m_nContrast = a_nValue;
		g_pApp->GetDocument()->SetContrast(m_nContrast);
		g_pApp->GetMainWidget()->UpdateView();
	}
}

void CRoom3DBrightDlg::verify()
{
	g_pApp->GetDocument()->SetBrightAndContrast(m_nOrgBright, m_nOrgContrast);
	accept();
}

void CRoom3DBrightDlg::OnCancel()
{
	g_pApp->GetDocument()->SetBrightAndContrast(m_nOrgBright, m_nOrgContrast);
	reject();
}

void CRoom3DBrightDlg::OnPressDefaultBtn()
{
	m_blDefaultSetting = true;
	m_pBrightSlider->setValue(m_nOrgBright);
	m_pContrastSlider->setValue(m_nOrgContrast);
	g_pApp->GetDocument()->SetBrightAndContrast(m_nOrgBright, m_nOrgContrast);
	g_pApp->GetMainWidget()->UpdateView();
	m_blDefaultSetting = false;

}
