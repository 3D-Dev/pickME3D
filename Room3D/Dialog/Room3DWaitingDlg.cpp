#include "Room3DWaitingDlg.h"
#include "Room3DApplication.h"


#define WAITING_DLG_WIDTH 300
#define WAITING_DLG_HIGHT 180
#define WAITING_DLG_ROT_NUM 12

#define ROTATION_RECT_WIDTH 20
#define ROTATION_RECT_HEIGHT 4
#define ROTATION_RECT_ROUND 2
#define ROTATION_RADIUS 24

CRoom3DWaitingDlg::CRoom3DWaitingDlg(QWidget *a_parent)
: CRoom3DBaseDlg(a_parent)
{
	m_nRotIndex = 0;
	m_nTimerID = 0;

	QFlag flgs = QFlag(Qt::Dialog | Qt::FramelessWindowHint);
	this->setWindowFlags(flgs);
	this->setWindowOpacity(0.5);
}

CRoom3DWaitingDlg::~CRoom3DWaitingDlg(void)
{
}

void CRoom3DWaitingDlg::timerEvent(QTimerEvent * event)
{
	m_nRotIndex++;
	m_nRotIndex = m_nRotIndex % WAITING_DLG_ROT_NUM;
	update();
}

void CRoom3DWaitingDlg::showEvent(QShowEvent * event)
{
	int nWidth = WAITING_DLG_WIDTH;
	int nHeight = WAITING_DLG_HIGHT;
	int nX = 0;
	int nY = 0;

	QWidget* pParent = parentWidget();
	if(pParent)
	{
		QRect rt = pParent->rect();
		nWidth = rt.width();
		nHeight = rt.height();
	}
	setFixedSize(nWidth, nHeight);
	QPoint pt(nX, nY);
	pt = pParent->mapToGlobal(pt);
	move(pt);

	m_nTimerID = startTimer(100);
	m_nRotIndex = 0;
}

void CRoom3DWaitingDlg::hideEvent(QHideEvent* event)
{
	if(m_nTimerID)
		killTimer(m_nTimerID);
}

void CRoom3DWaitingDlg::paintEvent(QPaintEvent *)
{
	DrawBackground();
	DrawLoadingText();
	DrawRotation();
}

void CRoom3DWaitingDlg::DrawBackground()
{
	QPainter painter(this);
	painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
	painter.setBackgroundMode(Qt::TransparentMode);

	QColor color(0, 0, 0, 255);
	QBrush brush(color);
	QPen pen(Qt::NoPen);
	painter.setPen(pen);
	painter.setBrush(brush);
	int nWidth = width();
	int nHeight = height();
	QRect rectangle(0.0, 0.0, nWidth, nHeight);
	painter.drawRect(rectangle);

	pen.setStyle(Qt::SolidLine);
	pen.setColor(QColor(0, 255, 0));
	pen.setWidth(3);
	painter.setPen(pen);
	brush.setColor(QColor(0, 0, 255, 255));
	painter.setBrush(brush);
	int nX = (nWidth - WAITING_DLG_WIDTH)/2;
	int nY = (nHeight - WAITING_DLG_HIGHT)/2;
	rectangle.setRect(nX, nY, WAITING_DLG_WIDTH, WAITING_DLG_HIGHT);
	painter.drawRoundedRect(rectangle, 20.0, 15.0);
}

void CRoom3DWaitingDlg::DrawLoadingText()
{
	int nWidth = width();
	int nHeight = height();
	int nX = (nWidth - WAITING_DLG_WIDTH)/2;
	int nY = (nHeight - WAITING_DLG_HIGHT)/2;

	QFont font("Arial", 12, QFont::Bold);

	QPainter painter(this);
	painter.setPen(Qt::yellow);
	painter.setFont(font);
	QString sLoading = tr("Loading Models");
	int nDot=m_nRotIndex / 3;
	for(int i=0; i<nDot; i++)
		sLoading += tr(".");

	QRect rectangle(nX+WAITING_DLG_WIDTH/6, nY+WAITING_DLG_HIGHT /6, WAITING_DLG_WIDTH, WAITING_DLG_HIGHT / 4);
	painter.drawText(rectangle, Qt::AlignLeft, sLoading);
}

void CRoom3DWaitingDlg::DrawRotation()
{
	QPainter painter(this);
	painter.setPen(Qt::NoPen);

	int nWidth = width();
	int nHeight = height();
	int nX = nWidth/2;
	int nY = nHeight/2;
	nY += WAITING_DLG_HIGHT / 6;

	QTransform transOrg = painter.transform();

	QRect rectangle(-ROTATION_RECT_WIDTH/2, -ROTATION_RECT_HEIGHT/2, ROTATION_RECT_WIDTH, ROTATION_RECT_HEIGHT);
	for(int i=0; i<WAITING_DLG_ROT_NUM; i++)
	{
		int nIndex = (m_nRotIndex + WAITING_DLG_ROT_NUM - i)%WAITING_DLG_ROT_NUM;
		QColor color(255-i*255/WAITING_DLG_ROT_NUM, 255-i*255/WAITING_DLG_ROT_NUM, 255-i*255/WAITING_DLG_ROT_NUM, 255);
		int angle = 360/WAITING_DLG_ROT_NUM * nIndex;

		QBrush brush(color);
		painter.setBrush(brush);

		QTransform trans;
 		trans.translate(nX, nY);
		trans.rotate(angle);
 		trans.translate(ROTATION_RADIUS, 0);
		painter.setTransform(trans);
		painter.drawRoundedRect(rectangle, ROTATION_RECT_ROUND, ROTATION_RECT_ROUND);
	}

	painter.setTransform(transOrg);
}

void CRoom3DWaitingDlg::resizeEvent(QResizeEvent * /* event */)
{
}