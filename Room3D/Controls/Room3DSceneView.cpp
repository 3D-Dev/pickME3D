#include "Room3DSceneView.h"
#include "Room3DApplication.h"
#include "Room3DCommandMgr.h"

#define MIN_MARGIN_H	10
#define MIN_MARGIN_V	5

CRoom3DSceneViewWrapper::CRoom3DSceneViewWrapper(QWidget* a_pParent) :CRoom3DImageWidget(a_pParent)
{
	this->SetImage(BACK_IMG_SCENEVIEW, "", 1, false);
}

void CRoom3DSceneViewWrapper::paintEvent(QPaintEvent * event)
{
	CRoom3DImageWidget::paintEvent(event);

	// Draw border of GLWidget if image is loaded
	if(g_pApp->GetDocument()->GetBkImageAspectRatio() > 0.f)
	{
		QPainter p(this);
 		QColor borderColor(qRgba(0, 0, 0, 10));
 		p.setPen(borderColor);
		p.drawLine(m_ScenViewRect.left(), m_ScenViewRect.bottom() , m_ScenViewRect.left(), m_ScenViewRect.top());
		p.drawLine(m_ScenViewRect.left(), m_ScenViewRect.top() , m_ScenViewRect.right(),m_ScenViewRect.top());
		borderColor = QColor(qRgba(255, 255, 255, 40));
		p.setPen(borderColor);
		p.drawLine(m_ScenViewRect.right(),m_ScenViewRect.top() , m_ScenViewRect.right(),m_ScenViewRect.bottom());
		p.drawLine(m_ScenViewRect.right(),m_ScenViewRect.bottom() , m_ScenViewRect.left(), m_ScenViewRect.bottom());

		borderColor=QColor(qRgba(0, 0, 0, 40));
		p.setPen(borderColor);
		p.drawLine(m_ScenViewRect.left()-1, m_ScenViewRect.bottom()+1 , m_ScenViewRect.left()-1, m_ScenViewRect.top()-1);
		p.drawLine(m_ScenViewRect.left()-1, m_ScenViewRect.top()-1 , m_ScenViewRect.right()+1,m_ScenViewRect.top()-1);
		borderColor = QColor(qRgba(255, 255, 255, 10));
		p.setPen(borderColor);
		p.drawLine(m_ScenViewRect.right()+1,m_ScenViewRect.top()-1 , m_ScenViewRect.right()+1,m_ScenViewRect.bottom()+1);
		p.drawLine(m_ScenViewRect.right()+1,m_ScenViewRect.bottom()+1 , m_ScenViewRect.left()-1, m_ScenViewRect.bottom()+1);
	}
	else
	{
		QPainter p(this);
		QPoint startPt = m_ScenViewRect.topLeft();
		QPoint endPt = m_ScenViewRect.bottomRight();
		QLinearGradient gradient(startPt, endPt);
		gradient.setColorAt(0, SCEN_VIEW_BK_COLOR_START);
		gradient.setColorAt(1, SCEN_VIEW_BK_COLOR_END);
		p.setBrush(gradient);
		p.drawRect(m_ScenViewRect);

	}
}

void CRoom3DSceneViewWrapper::resizeEvent(QResizeEvent * event)
{
	CRoom3DImageWidget::resizeEvent(event);
	UpdateSceneVeiwLayout();
}

void CRoom3DSceneViewWrapper::UpdateSceneVeiwLayout()
{
	QRect srcRect = rect();
	float fViewAspect = g_pApp->GetDocument()->GetBkImageAspectRatio();
	CRoom3DSceneView* pView = g_pApp->GetSceneView();
	srcRect.adjust(MIN_MARGIN_H, MIN_MARGIN_V, -MIN_MARGIN_H, -MIN_MARGIN_V);
	if(fViewAspect > 0.f)
	{
		pView->show();
	}
	else
	{
		pView->hide();
	//	srcRect.adjust(m_borderThickness, m_borderThickness, -m_borderThickness, -m_borderThickness);
	}
	QRect dstRect = srcRect;
	int nDstWidth, nDstHeight;
	nDstWidth = dstRect.width();
	nDstHeight = dstRect.height();
	float fSrcAspect = (float) srcRect.width() / (float) srcRect.height();
	if(pView)
	{
		if(fViewAspect > 0.f)
		{
			if(fViewAspect > fSrcAspect)
				nDstHeight = srcRect.width() / fViewAspect;
			else
				nDstWidth = srcRect.height() * fViewAspect;
			dstRect.setLeft(srcRect.left() + (srcRect.width() - nDstWidth) / 2);
			dstRect.setTop(srcRect.top() + (srcRect.height() - nDstHeight) / 2);
			dstRect.setSize(QSize(nDstWidth, nDstHeight));
		}
		pView->setGeometry(dstRect);
		m_ScenViewRect = dstRect;
	}
	this->update();
}

CRoom3DSceneView::CRoom3DSceneView(QWidget* parent) : QGLWidget(parent)
{
	setAcceptDrops(true);
	setMouseTracking(true);
	setFocusPolicy(Qt::ClickFocus);
	QGLFormat curFormat = this->format();
	curFormat.setSwapInterval(0);
	this->setFormat(curFormat);
#if SHOW_FPS
	m_nTotalTickCount = 0;
	m_nRenderCount = 0;
	m_fFPS = 0.f;
#endif
}

CRoom3DSceneView::~CRoom3DSceneView(void)
{
}

 void CRoom3DSceneView::initializeGL()
 {
	 // define display lists etc.:
	 g_pApp->GetDocument()->initializeGL();
 }

 void CRoom3DSceneView::resizeGL(int w, int h)
 {
	 // setup viewport, projection etc.:
	 g_pApp->GetDocument()->resizeGL(w, h);
 }

 void CRoom3DSceneView::paintGL()
 {
#if SHOW_FPS
	m_time.restart();
#endif
	// draw the scene:
	g_pApp->GetDocument()->Render();

#if SHOW_FPS
	m_nTotalTickCount += m_time.elapsed();
	m_nRenderCount++;
	if(m_nTotalTickCount > 100 && m_nRenderCount > 10)
	{
		m_fFPS = m_nRenderCount * 1000.f / m_nTotalTickCount;
		m_nRenderCount = 0;
		m_nTotalTickCount = 0;
	}
	if(g_pApp->GetAppStep() == APP_EDIT_STEP_3)
	{
		QFont font("Arial Unicode MS", 10, QFont::Bold);
		font.setLetterSpacing(QFont::PercentageSpacing, 180);
		QString sFPS;
		sFPS.setNum(m_fFPS, 'f', 2);
		glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
		renderText(30, 30, sFPS, font, 2000 );
	}
#endif
}

void CRoom3DSceneView::mousePressEvent(QMouseEvent * a_pEvent)
{
	//setFocus(Qt::MouseFocusReason);
	CRoom3DCommandMgr* pCmdMgr = g_pApp->GetCommandMgr();
	if(pCmdMgr->GetCurrentCmd())
	{
		pCmdMgr->GetCurrentCmd()->MousePressEvent(a_pEvent);
	}
}

void CRoom3DSceneView::mouseReleaseEvent(QMouseEvent * a_pEvent)
{
	CRoom3DCommandMgr* pCmdMgr = g_pApp->GetCommandMgr();
	if(pCmdMgr->GetCurrentCmd())
	{
		pCmdMgr->GetCurrentCmd()->MouseReleaseEvent(a_pEvent);
	}
}

void CRoom3DSceneView::mouseMoveEvent(QMouseEvent * a_pEvent)
{
	CRoom3DCommandMgr* pCmdMgr = g_pApp->GetCommandMgr();
	if(pCmdMgr->GetCurrentCmd())
	{
		pCmdMgr->GetCurrentCmd()->MouseMoveEvent(a_pEvent);
	}

}

void CRoom3DSceneView::wheelEvent(QWheelEvent* a_pEvent)
{
	CRoom3DCommandMgr* pCmdMgr = g_pApp->GetCommandMgr();
	if(pCmdMgr->GetCurrentCmd())
	{
		pCmdMgr->GetCurrentCmd()->WheelEvent(a_pEvent);
	}
}

void CRoom3DSceneView::keyPressEvent ( QKeyEvent * a_pEvent )
{
	CRoom3DCommandMgr* pCmdMgr = g_pApp->GetCommandMgr();
	if(pCmdMgr->GetCurrentCmd())
	{
		pCmdMgr->GetCurrentCmd()->keyPressEvent(a_pEvent);
	}
}

void CRoom3DSceneView::keyReleaseEvent ( QKeyEvent * a_pEvent )
{
	CRoom3DCommandMgr* pCmdMgr = g_pApp->GetCommandMgr();
	if(pCmdMgr->GetCurrentCmd())
	{
		pCmdMgr->GetCurrentCmd()->keyReleaseEvent(a_pEvent);
	}
}

void CRoom3DSceneView::dragEnterEvent(QDragEnterEvent *event)
{
	setBackgroundRole(QPalette::Highlight);

	if(event->mimeData()->hasFormat(ITEM_DRAG_MIME_TYPE))
	{
		event->acceptProposedAction();
		ITEM_INFO* pItemInfo = GetDropedItemInfo(event);
		g_pApp->GetDocument()->OnEnterDragItem((QDragMoveEvent*)event, pItemInfo);
	}
	// emit changed(event->mimeData());
}

void CRoom3DSceneView::dragMoveEvent(QDragMoveEvent *event)
{
	event->acceptProposedAction();
	g_pApp->GetDocument()->OnMoveDragItem(event);
}

void CRoom3DSceneView::dropEvent(QDropEvent *event)
{
	setBackgroundRole(QPalette::Dark);
	event->acceptProposedAction();
	g_pApp->GetDocument()->UpdateDoc(APP_MSG_FURNITURE_ADD);
	g_pApp->GetDocument()->OnDropDragItem();
}

void CRoom3DSceneView::dragLeaveEvent(QDragLeaveEvent *event)
{
	// clear();
	event->accept();
	g_pApp->GetDocument()->OnLeaveDragItem();
	g_pApp->GetMainWidget()->UpdateView();
}

ITEM_INFO* CRoom3DSceneView::GetDropedItemInfo(QDragEnterEvent* a_pEvent)
{
	const QMimeData *pMimeData = a_pEvent->mimeData();
	// decode and insert
	QByteArray encoded = pMimeData->data(ITEM_DRAG_MIME_TYPE);
	QDataStream stream(&encoded, QIODevice::ReadOnly);
	QVector<QMap<int, QVariant> > itemDatas;

	while (!stream.atEnd()) {
		int r, c;
		QMap<int, QVariant> v;
		stream >> r >> c >> v;
		itemDatas.append(v);
	}
	if(itemDatas.size())
	{
		QMap<int, QVariant> itemData = itemDatas.at(0);
		ITEM_INFO* pInfo = (ITEM_INFO*) itemData.value(Qt::UserRole).toInt();
		return pInfo;
	}
	return NULL;
}

QRect CRoom3DSceneView::GetViewportRect()
{
	QRect viewportRect = rect();
	return viewportRect;
}
