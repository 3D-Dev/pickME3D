#include "Room3DTabWidget.h"
#include "common.h"
#include "Room3DApplication.h"
#define BUT_HIGHT	25
#define BUT_SARTPOS	10
#define ROUND_GAP	30

CTabButton::CTabButton(QWidget *pParent, QString caption)
: QPushButton(pParent)
{
	this->setCheckable(true);
	m_Caption = caption;
	m_nTabIndex = -1;

	CTabViewContainer* pParentWidget = (CTabViewContainer*)pParent;
}

CTabButton::~CTabButton()
{
}

void CTabButton::mousePressEvent(QMouseEvent *event)
{
	CTabViewContainer* pParentWidget = (CTabViewContainer*)this->parentWidget();
	pParentWidget->OnUpdateTabs(m_nTabIndex);
	this->setChecked(true);
}

void CTabButton::paintEvent(QPaintEvent *event)
{
	QRect rt = this->rect();
	int w = rt.width();
	int h = rt.height();

	QLinearGradient linearGradient(0, 0, rt.left(), h);
	if (this->isChecked())
	{
		linearGradient.setColorAt(0.0, QColor(60, 60, 60));
		linearGradient.setColorAt(0.07, QColor(70, 70, 70));
		linearGradient.setColorAt(0.11, QColor(120, 120, 120));
		linearGradient.setColorAt(1.0, QColor(180, 180, 180));
	}else {
		linearGradient.setColorAt(0.0, QColor(88, 86, 78));
		linearGradient.setColorAt(0.07, QColor(68, 66, 57));
		linearGradient.setColorAt(0.11, QColor(63, 60, 54));
		linearGradient.setColorAt(1.0, QColor(59, 55, 50));
	}

	QPainterPath path;
	path.moveTo(rt.left(), rt.bottom());
	path.lineTo(rt.right(), rt.bottom());
	path.arcTo(rt.right()-h*2, rt.top(), h*2, h*2, 0, 90);
	path.lineTo(rt.left()+h/2, rt.top());
	path.arcTo(rt.left(), rt.top(), h/2, h/2, 90, 90);
	path.lineTo(rt.left(), rt.bottom());

	//QPolygon outLine;
	//for (int i = 0; i < 100; i++) {
	//	QPointF ptF = path.pointAtPercent(i*0.01);
	//	QPoint pt;
	//	pt.setX((int)ptF.x());
	//	pt.setY((int)ptF.y());
	//	outLine << pt;
	//}
	//QRegion clipRgn(outLine);
	//this->setMask(outLine);

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setPen(QPen(QColor(60,60,60)));
	painter.setBrush(linearGradient);
	painter.drawPath(path);

	painter.setPen(QPen(QColor(255, 243, 243)));
	QFont ft("MS UI Gothic", 10, QFont::Bold);
	painter.setFont(ft);
	painter.drawText(4, h-10, m_Caption);

	//QPushButton::paintEvent(event);
}


CTabViewContainer::CTabViewContainer(QWidget *pParent)
: m_cPages(this), QWidget(pParent)
{
	for (int i = 0; i < TAB_NUM; i++)
	{
		m_pTabs[i] = NULL;
	}
}
CTabViewContainer::~CTabViewContainer()
{
}

void CTabViewContainer::paintEvent(QPaintEvent *event)
{
	QRect rt = this->rect();
	QPainter painter(this);
	//painter.fillRect(rt, QBrush(Qt::black));

	QPainterPath path;
	int tmpH = rt.top()+2*BUT_HIGHT-3;
	path.moveTo(rt.left()+1, tmpH+ROUND_GAP-9);
	path.lineTo(rt.left()+1, rt.bottom()-ROUND_GAP);
	path.arcTo(rt.left()+1, rt.bottom()-ROUND_GAP, ROUND_GAP, ROUND_GAP, 180,90);
	path.lineTo(rt.right()-ROUND_GAP-1, rt.bottom());
	path.arcTo(rt.right()-ROUND_GAP-1, rt.bottom()-ROUND_GAP, ROUND_GAP, ROUND_GAP, -90,90);
	path.lineTo(rt.right()-1, tmpH+ROUND_GAP);
	path.arcTo(rt.right()-ROUND_GAP-1, tmpH, ROUND_GAP, ROUND_GAP, 0,90);
	path.lineTo(rt.left()+ROUND_GAP+1, tmpH);
	path.arcTo(rt.left()+1, tmpH, ROUND_GAP, ROUND_GAP, 90,90);

	QPen pen(QColor(120,120,120));
	pen.setWidth(6);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setPen(pen);
	painter.setBrush(QColor(255,253,232));
	painter.drawPath(path);
}

void CTabViewContainer::addTab(QWidget *page, QString& label)
{
	if (page == NULL)
		return;

	int n = -1;
	for (int i = 0; i < TAB_NUM; i++)
	{
		if (m_pTabs[i] == NULL) {
			m_pTabs[i] = new CTabButton(this, label);
			m_pTabs[i]->SetTabIndex(i);
			m_cPages.addWidget(page);
			n = i;
			break;
		}
	}
	
	int w_home_decoration, w_office, w_outdoor, w_bedroom, w_living_room, w_kitchen, w_bathroom;
	if(g_pApp->GetLanguage() == CRoom3DApplication::Lang_Eng)
	{
		w_home_decoration = 137;
		w_office = 67;
		w_outdoor = 77;
		w_bedroom = 77;
		w_living_room = 90;
		w_kitchen = 70;
		w_bathroom = 75;
	}
	else
	{
		w_home_decoration = 85;
		w_office = 77;
		w_outdoor = 80;
		w_bedroom = 70;
		w_living_room = 80;
		w_kitchen = 70;
		w_bathroom = 67;
	}
	int margin = 3;

	switch(n)
	{
	case 0:
		m_pTabs[n]->setGeometry(BUT_SARTPOS + w_home_decoration + w_office - margin * 2 , 0, w_outdoor, BUT_HIGHT);
		break;
	case 1:
		m_pTabs[n]->setGeometry(BUT_SARTPOS + w_home_decoration - margin, 0, w_office, BUT_HIGHT);
		break;
	case 2:
		m_pTabs[n]->setGeometry(BUT_SARTPOS, 0, w_home_decoration, BUT_HIGHT);
		break;
	case 3:
		m_pTabs[n]->setGeometry(BUT_SARTPOS +  w_bedroom + w_living_room + w_kitchen - margin * 3, BUT_HIGHT - 5, w_bathroom, BUT_HIGHT);
		break;
	case 4:
		m_pTabs[n]->setGeometry(BUT_SARTPOS + w_bedroom + w_living_room - margin * 2, BUT_HIGHT - 5, w_kitchen, BUT_HIGHT);
		break;
	case 5:
		m_pTabs[n]->setGeometry(BUT_SARTPOS + w_bedroom - margin, BUT_HIGHT - 5, w_living_room, BUT_HIGHT);
		break;
	case 6:
		m_pTabs[n]->setGeometry(BUT_SARTPOS, BUT_HIGHT - 5, w_bedroom, BUT_HIGHT);
		break;
	}
}

void CTabViewContainer::OnUpdateTabs(int nTabIndex)
{
	if (m_pTabs[nTabIndex] == NULL)
		return;

	m_pTabs[nTabIndex]->setChecked(true);

	for (int i = 0; i < TAB_NUM; i++)
	{
		if (i == nTabIndex)
			continue;
		if (m_pTabs[i] == NULL)
			continue;
		m_pTabs[i]->setChecked(false);
	}
	m_cPages.setCurrentIndex(nTabIndex);
	QWidget *curWidget = m_cPages.currentWidget();

	emit currentChanged(nTabIndex);
}

void CTabViewContainer::clear()
{
	for (int i = 0; i < TAB_NUM; i++) {
		delete m_pTabs[i];
		m_pTabs[i] = NULL;
	}

	while (m_cPages.count() != 0)
		m_cPages.removeWidget(m_cPages.widget(0));
}

void CTabViewContainer::resizeEvent(QResizeEvent *event)
{
	QRect rt = this->rect();
	int tmpH = rt.top()+2*BUT_HIGHT-4;

	QWidget *pCurWidget = currentWidget();
	if (pCurWidget == NULL)
		return;
	m_cPages.setGeometry(rt.left()+9, tmpH+9, rt.width()-18, rt.bottom()-tmpH-18);
	
	QWidget::resizeEvent(event);
}

void CTabViewContainer::Init()
{
	m_cPages.setParent(this);
	m_pTabs[0]->setChecked(true);
	m_cPages.setCurrentIndex(0);
}

void CTabViewContainer::setCurrentWidget(QWidget* a_pWidget)
{
	m_cPages.setCurrentWidget(a_pWidget);
	int curInd = m_cPages.currentIndex();
	OnUpdateTabs(curInd);
}
