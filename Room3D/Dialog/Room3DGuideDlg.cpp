#include "Room3DGuideDlg.h"
#include "Room3DApplication.h"

CRoom3DGuideDlg::CRoom3DGuideDlg(QWidget *a_parent)
: QWidget(a_parent)
{
	startTimer(100);
	m_fOpacity = 1;
	tmpPixmap = QPixmap(GUIDE_ARROW_IMG);
	maskBitmap = QBitmap(GUIDE_ARROW_MASK);
	this->setFixedSize(tmpPixmap.size());
}
CRoom3DGuideDlg::~CRoom3DGuideDlg(void)
{
}

void CRoom3DGuideDlg::timerEvent(QTimerEvent * event)
{
	m_fOpacity -= 0.2f;
	if(m_fOpacity < -1.f)
		m_fOpacity = 1.f;
	update();
}

void CRoom3DGuideDlg::showEvent(QShowEvent * event)
{
	m_fOpacity = 1.f;
}

void CRoom3DGuideDlg::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		this->hide();
		g_pApp->GetMainWidget()->ShowGuideDlg(STEP_NONE);
		event->accept();
	}
}

void CRoom3DGuideDlg::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	float fOpacity = fabs(m_fOpacity);
	painter.setOpacity(fOpacity);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.save();
	painter.drawPixmap(tmpPixmap.rect(), tmpPixmap);
	painter.restore();
}

void CRoom3DGuideDlg::resizeEvent(QResizeEvent * /* event */)
{
	setMask(maskBitmap);
}