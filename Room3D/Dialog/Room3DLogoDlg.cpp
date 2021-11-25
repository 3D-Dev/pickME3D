#include "Room3DLogoDlg.h"
#include <QPainter>
#include "Common.h"
#include "Define.h"
#include "Room3DApplication.h"

#define MAX_LOGO_TEXT_COUTNT	4

CRoom3DLogoDlg::CRoom3DLogoDlg(QWidget* a_pParent, QString a_sImgPath) : QSplashScreen(a_pParent)
{
	connect(this, SIGNAL(messageChanged(QString)), this, SLOT(OnMessageChanged(QString)));
	m_sMessages.clear();
	QPixmap pixmap(a_sImgPath);
	setPixmap(pixmap);
	this->setWindowTitle(g_pApp->GetMainWidget()->windowTitle());
}

CRoom3DLogoDlg::~CRoom3DLogoDlg(void)
{
	m_sMessages.clear();
}

void CRoom3DLogoDlg::AddLogoText(QString a_sMessage)
{
	if(m_sMessages.count() >= MAX_LOGO_TEXT_COUTNT)
	{
		while(m_sMessages.count() < MAX_LOGO_TEXT_COUTNT)
		{
			m_sMessages.pop_front();
		}
	}
	m_sMessages.push_back(a_sMessage);
}

void CRoom3DLogoDlg::drawContents(QPainter * painter)
{
	int nCount = m_sMessages.count();
	QRect rt = rect();
	QPoint startPt;
	QStringList::iterator it = m_sMessages.begin();
	while(it != m_sMessages.end())
	{
		QString sMessage = (*it);
		it++;
	}
}

void CRoom3DLogoDlg::OnMessageChanged(QString a_sMessage)
{
	AddLogoText(a_sMessage);
}

void CRoom3DLogoDlg::resizeEvent(QResizeEvent * event)
{
}
