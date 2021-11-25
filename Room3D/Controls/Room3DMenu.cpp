#include "Room3DMenu.h"
#include <QPainter>

//----------------------------------------------
//	class : CRoom3DAction
//----------------------------------------------
CRoom3DAction::CRoom3DAction(QWidget* pParent)
:QAction(pParent)
{
	m_bStatic = false;

	QFont fontTemp(ACTION_FONT_FAMILY, ACTION_FONT_NORMAL_SIZE, QFont::Normal);
	fontTemp.setLetterSpacing(QFont::PercentageSpacing, ACTION_FONT_SPACING);
	fontTemp.setStretch(ACTION_FONT_STRETCH);
	setFont(fontTemp);
}

CRoom3DAction::~CRoom3DAction()
{

}

bool CRoom3DAction::isStatic()
{
	return m_bStatic;
}

void CRoom3DAction::setStatic(bool bStatic)
{
	m_bStatic = bStatic;
	setEnabled(!bStatic);
}

//----------------------------------------------
//	class : CRoom3DMenu
//----------------------------------------------
CRoom3DMenu::CRoom3DMenu(QWidget* pParent)
:QMenu(pParent)
{
}

CRoom3DMenu::~CRoom3DMenu()
{
}

void CRoom3DMenu::SetRegion(const QRegion& region)
{
	m_sRegion = region;
}

#define ROUND_X 4
#define ROUND_Y 4

void CRoom3DMenu::resizeEvent(QResizeEvent *e)
{
	QSize sSize = size();
	QRegion rectRegion1(ROUND_X, 0, sSize.width()-ROUND_X*2, sSize.height());
	QRegion rectRegion2(0, ROUND_Y, sSize.width(), sSize.height()-ROUND_Y*2);
	QRegion ellipRegion1(0, 0, ROUND_X*2, ROUND_Y*2, QRegion::Ellipse);
	QRegion ellipRegion2(sSize.width()-ROUND_X*2, 0, ROUND_X*2, ROUND_Y*2, QRegion::Ellipse);
	QRegion ellipRegion3(0, sSize.height()-ROUND_Y*2, ROUND_X*2, ROUND_Y*2, QRegion::Ellipse);
	QRegion ellipRegion4(sSize.width()-ROUND_X*2, sSize.height()-ROUND_Y*2, ROUND_X*2, ROUND_Y*2, QRegion::Ellipse);

	m_sRegion = rectRegion1 + rectRegion2;
	m_sRegion += ellipRegion1;
	m_sRegion += ellipRegion2;
	m_sRegion += ellipRegion3;
	m_sRegion += ellipRegion4;
}

void CRoom3DMenu::paintEvent(QPaintEvent *e)
{
	setMask(m_sRegion);

	QSize sSize = size();
	QRect rect(0, 0, sSize.width(), sSize.height());
	QPainter painter(this);
	painter.setPen(QColor(12,14,10));
	painter.setBrush(QColor(12,14,10));
	painter.drawRect(rect);
	int nX, nY, nWidth, nHeight;

	QFont fontBig(ACTION_FONT_FAMILY, ACTION_FONT_BIG_SIZE, QFont::Normal);
	fontBig.setLetterSpacing(QFont::PercentageSpacing, ACTION_FONT_SPACING);
	fontBig.setStretch(ACTION_FONT_STRETCH);

	QFont fontNorm(ACTION_FONT_FAMILY, ACTION_FONT_NORMAL_SIZE, QFont::Normal);
	fontNorm.setLetterSpacing(QFont::PercentageSpacing, ACTION_FONT_SPACING);
	fontNorm.setStretch(ACTION_FONT_STRETCH);

	QList<QAction *> menuActions = actions();
	CRoom3DAction* active = (CRoom3DAction*)activeAction();
	for(QList<QAction*>::iterator it=menuActions.begin(); it != menuActions.end(); it++)
	{
		CRoom3DAction *action = (CRoom3DAction *)(*it);
		rect = actionGeometry(action);
		nX = rect.left(); nY = rect.top(); nWidth = rect.width(); nHeight = rect.height();
		rect.setRect(nX+1, nY+1, nWidth-2, nHeight-2);


		if(action->isStatic())
		{
			painter.setFont(fontBig);
			painter.setPen(Qt::white);
			painter.drawText(rect, Qt::AlignCenter | Qt::AlignVCenter, action->text());
		}
		else
		{
			painter.setFont(fontNorm);
			if(action->isEnabled())
			{
				painter.setPen(Qt::white);
			}
			else
			{
				painter.setPen(Qt::darkGray);
			}
			painter.drawText(rect, Qt::AlignCenter | Qt::AlignVCenter, action->text());

			if(action == active)
			{
				painter.setBrush(QColor(255,255,255,ACTION_ACTIVE_ALPHA));
				painter.setPen(QColor(255,255,255,ACTION_ACTIVE_ALPHA*4));
			}
			else
			{
				painter.setBrush(QColor(255,255,255,ACTION_NORMAL_ALPHA));
				painter.setPen(QColor(255,255,255,ACTION_NORMAL_ALPHA*4));
			}
			painter.drawRoundedRect(rect,2,2);
		}
	}

	//	QMenu::paintEvent(e);
}