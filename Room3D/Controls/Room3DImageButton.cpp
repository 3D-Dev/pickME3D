#include "Room3DImageButton.h"
#include "Room3DUtility.h"
#include "Room3DApplication.h"

// #define USE_BUTTON_STATE_IMG_PROCESS

CRoom3DImageButton::CRoom3DImageButton(QString a_sImgPath, int a_nButtonID, Qt::CursorShape cursor, QWidget* parent)
    : QPushButton(parent)
{
    QString bkResName = a_sImgPath;
    QPixmap* pBkResPixmap = new QPixmap(bkResName);
    QBitmap bitmask;
    if (pBkResPixmap != NULL) {
        bitmask = pBkResPixmap->mask();
        //m_bkOrgImg = pBkResPixmap->toImage();
		m_bkOrgImg = *pBkResPixmap;
        delete pBkResPixmap;
    }

    // set stage
    int width = 0;
    int height = 0;

    if (!m_bkOrgImg.isNull()) {
        width = m_bkOrgImg.width();
#ifdef USE_BUTTON_STATE_IMG_PROCESS
		height = m_bkOrgImg.height();
#else
        height = m_bkOrgImg.height() / 4;
#endif
        // by rich for modified bitmap mask setting. (Bug 6553)
        //setMask(bitmask);
    }
    // set windows size, not resizable
   setFixedSize(width, height);

	m_pressed = false;
	m_nBtnID = a_nButtonID;
	m_cursoryShape = cursor;
	setState(BTN_STATE_NORMAL);
    setCursor(m_cursoryShape);
	setMouseTracking(true);
	setAttribute(Qt::WA_NoMousePropagation);

	m_pMenu = NULL;
}

CRoom3DImageButton::~CRoom3DImageButton()
{
}

void CRoom3DImageButton::SetPixmap(QPixmap a_pixmap)
{
	m_bkOrgImg = a_pixmap;
	UpdateButtonStateImg();
	update();
}

void CRoom3DImageButton::SetPixmap(QString a_sImgPath)
{
	m_bkOrgImg.load(a_sImgPath);
	UpdateButtonStateImg();
	update();
}

void CRoom3DImageButton::SetMenu(QMenu* pMenu)
{
	m_pMenu = pMenu;
	setMenu(m_pMenu);

	if(m_pMenu)
	{
		connect(m_pMenu, SIGNAL(aboutToShow()), this, SLOT(OnShowMenu()));
		connect(m_pMenu, SIGNAL(aboutToHide()), this, SLOT(OnHideMenu()));
	}
}

void CRoom3DImageButton::setState(BUTTON_STATE state, bool blWithLeaveEvent)
{
//    if (m_state != state) {
        m_state = state;
		if (m_state != BTN_STATE_DISABLE) {
			if (m_state == BTN_STATE_NORMAL && blWithLeaveEvent == false) {
				QPoint pt = QCursor::pos();
				if (rect().contains(mapFromGlobal(pt))) {
					m_state = BTN_STATE_FOCUS;
				}
			}
		    setCursor(m_cursoryShape);
		} else {
			setCursor(Qt::ArrowCursor);
		}
		UpdateButtonStateImg();
        update();
//    }
}

void CRoom3DImageButton::UpdateButtonStateImg()
{
#ifdef USE_BUTTON_STATE_IMG_PROCESS
	m_bkImg = m_bkOrgImg.scaled(rect().width(), rect().height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	QIcon icon = QIcon(m_bkImg);
	QIcon::Mode nMode;
	QIcon::State nState = QIcon::Off;
	switch(m_state)
	{
	case BTN_STATE_DISABLE:
		nMode = QIcon::Disabled;
		break;
	case BTN_STATE_NORMAL:
		nMode = QIcon::Normal;
		break;
	case BTN_STATE_FOCUS:
		nMode = QIcon::Active;
		break;
	case BTN_STATE_PRESSED:
		nMode = QIcon::Selected;
		break;
	}
	m_bkImg = icon.pixmap(rect().size(), nMode, nState);
	QPixmapCache::clear();

#endif
}

void CRoom3DImageButton::paintEvent(QPaintEvent *e)
{
    QPainter paint(this);
	QPoint sp(0,0);
	QRect rt, targetRect;
	targetRect = rect();
#ifdef USE_BUTTON_STATE_IMG_PROCESS
	rt.setRect(0,0,m_bkImg.width(), m_bkImg.height());
#else
    switch(m_state) {
    case BTN_STATE_NORMAL:
        rt.setTop(0);
        break;
    case BTN_STATE_FOCUS:
        rt.setTop(m_bkImg.height() / 4);
        break;
    case BTN_STATE_PRESSED:
        rt.setTop(m_bkImg.height() / 2);
        break;
    case BTN_STATE_DISABLE:
        rt.setTop(m_bkImg.height() * 3 / 4);
        break;
    }
    rt.setLeft(0);
    rt.setSize(size());
#endif
	// paint.drawImage(targetRect, m_bkImg, rt);
	paint.drawPixmap(targetRect, m_bkImg, rt);
}

void CRoom3DImageButton::OnShowMenu()
{
	if(m_pMenu)
	{
		if (m_state != BTN_STATE_DISABLE) 
		{
			m_pressed = true;
			setState(BTN_STATE_PRESSED);
		}
	}
}

void CRoom3DImageButton::OnHideMenu()
{
	if(m_pMenu)
	{
		if (m_state != BTN_STATE_DISABLE) 
		{
			m_pressed = false;
			setState(BTN_STATE_NORMAL);
		}
	}
}

void CRoom3DImageButton::mousePressEvent(QMouseEvent *e)
{
    if (m_state != BTN_STATE_DISABLE) {
			if (e->buttons() == Qt::LeftButton) {
				m_pressed = true;
			} else {
				m_pressed = false;
			}
			
			if (m_pressed) {
				setState(BTN_STATE_PRESSED);
			} else {
				setState(BTN_STATE_FOCUS);
			}
	}

	QPushButton::mousePressEvent(e);
}

void CRoom3DImageButton::mouseReleaseEvent(QMouseEvent *e)
{
	m_pressed = false;
    if (m_state != BTN_STATE_DISABLE) {
		if (m_state == BTN_STATE_PRESSED) {
			setState(BTN_STATE_NORMAL);
			g_pApp->GetSoundMgr()->Play(SOUND_BUTTON, 20);
			emit pressed(m_nBtnID);
		}
	}

	QPushButton::mouseReleaseEvent(e);
}

void CRoom3DImageButton::mouseMoveEvent(QMouseEvent *e)
{
    if (m_state != BTN_STATE_DISABLE) {
		if (rect().contains(e->pos())) {
			if (m_pressed) {
				setState(BTN_STATE_PRESSED);
			} else {
				setState(BTN_STATE_FOCUS);
			}
		} else {
			setState(BTN_STATE_NORMAL);
		}
	}

	QPushButton::mouseMoveEvent(e);
}

void CRoom3DImageButton::leaveEvent(QEvent *e)
{
    if (m_state != BTN_STATE_DISABLE) {
		if(m_pMenu == NULL)
		{
			setState(BTN_STATE_NORMAL, true);
		}
		else
		{
			if(m_pressed == false)
				setState(BTN_STATE_NORMAL, true);
		}
    }

	QPushButton::leaveEvent(e);
}

void CRoom3DImageButton::enterEvent(QEvent *e)
{
    if (m_state != BTN_STATE_DISABLE) {
		if(m_pMenu == NULL)
		{
	        setState(BTN_STATE_FOCUS);
		}
		else
		{
			if(m_pressed == false)
				setState(BTN_STATE_FOCUS);
		}
    }

	QPushButton::enterEvent(e);
}

void CRoom3DImageButton::resizeEvent ( QResizeEvent * event )
{
	QSize size = event->size();
#ifdef USE_BUTTON_STATE_IMG_PROCESS
	UpdateButtonStateImg();
#else
	m_bkImg = m_bkOrgImg.scaled(size.width(), size.height() * 4, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
#endif
}






CRoom3DImageWidget::CRoom3DImageWidget(QString a_sImgPath, QString a_sBorderImgPath, int a_sBorderThickness, bool a_stretchMode, QWidget* parent) : QWidget(parent)
{
	SetImage(a_sImgPath, a_sBorderImgPath, a_sBorderThickness, a_stretchMode);
	m_bkLeftTopColor = QColor(0,0,0);
	m_bkRightBottomColor= QColor(0,0,0);
}

CRoom3DImageWidget::~CRoom3DImageWidget()
{

}

void CRoom3DImageWidget::SetImage(QString a_sImgPath, QString a_sBorderImgPath, int a_sBorderThickness, bool a_stretchMode)
{
	m_stretchMode = a_stretchMode;
	m_borderThickness = a_sBorderThickness;
	m_bkOrgImg = QPixmap();
	m_bkOrgImg.load(a_sImgPath);
	m_bkBorderImg.load(a_sBorderImgPath);
	FitBkImageToRect(rect().size());
	this->update();
	if(a_sImgPath.isEmpty())
	{
		m_blTransparent = true;
	}
	else
		m_blTransparent = false;

}

void CRoom3DImageWidget::paintEvent(QPaintEvent * event)
{
	QPainter p(this);
	QRect rt, targetRect;
	targetRect = rect();
	int width, height ,borderWidth, borderHeight, bcw, bch, bth;
	width = m_bkOrgImg.width();
	height = m_bkOrgImg.height();
	borderWidth = m_bkBorderImg.width();
	borderHeight = m_bkBorderImg.height();
	bcw = borderWidth / 3;		// border cell width
	bch = borderHeight / 3;		// border cell height
	bth = m_borderThickness;	// border thickness
	if(!m_blTransparent)
	{
		if(m_bkImg.isNull())
		{
			QPoint startPt = targetRect.topLeft();
			QPoint endPt = targetRect.bottomRight();
			QLinearGradient gradient(startPt, endPt);
			gradient.setColorAt(0, m_bkLeftTopColor);
			gradient.setColorAt(1, m_bkRightBottomColor);
			p.setBrush(gradient);
			p.drawRect(targetRect);
		}
		else
		{
			// Draw BackGround Image
			if ( m_stretchMode ){			// Stretch Mode
				p.drawPixmap(targetRect, m_bkImg);
			}
			else{				// Tile Mode
				for ( int i = 0; i < (int)( (double)(targetRect.width()) / width ) + 1; i++ ){
					for ( int j = 0; j < (int)( (double)(targetRect.height()) / height ) + 1; j++ )	{
						p.drawPixmap( i * width , j * height, m_bkOrgImg );
					}
				}
			}
		}
	}
	
	// Draw Border
	//		__     __     __
	//	   |(1)    (2)   (3)|
	//
	//	   |(4)           (6)|
	//
	//	   |(7)_  __  _(9)|
	//
	//
	p.drawPixmap(0, 0, bth, bth, m_bkBorderImg, 0, 0, bcw, bch );																					// (1)
	for  ( int i = 0; i < (int)(double(targetRect.width()) / bth-1); i++ )
		p.drawPixmap((i+1) * (bth), 0, bth, bth, m_bkBorderImg, bcw, 0, bcw, bch );															// (2)
	p.drawPixmap( targetRect.width() - bth, 0, bth, bth, m_bkBorderImg, bcw * 2, 0, bcw, bch );											// (3)
	for  ( int i = 0; i < (int)((double)(targetRect.height()) / (bth)-1); i++ )
		p.drawPixmap( 0, (i+1) * (bth), bth, bth, m_bkBorderImg, 0, bch, bcw, bch );															// (4)
	for  ( int i = 0; i < (int)((double)(targetRect.height()) / (bth)-1); i++ )
		p.drawPixmap( targetRect.width() - bth, (i+1) * (bth), bth, bth, m_bkBorderImg, bcw * 2, bch, bcw, bch );					// (6)
	p.drawPixmap(0, targetRect.height() - bth, bth, bth, m_bkBorderImg, 0, bch * 2, bcw, bch );											// (7)
	for  ( int i = 0; i < (int)((double)(targetRect.width()) / (bth)-1); i++ )
		p.drawPixmap((i+1) * (bth), targetRect.height() - bth, bth, bth, m_bkBorderImg, bcw, bch * 2, bcw, bch );					// (8)
	p.drawPixmap(targetRect.width() - bth, targetRect.height() - bth, bth, bth, m_bkBorderImg, bch * 2, bch * 2, bcw, bch );	// (9)

// 	p.drawPixmap(0, 0, m_bkBorderImg , 0, 0, bcw, bch );																					// (1)
// 	for  ( int i = 0; i < (int)((double)(targetRect.width()) / (bcw)-1); i++ )
// 		p.drawPixmap((i+1) * (bcw), 0, m_bkBorderImg , bcw, 0, bcw, bch );															// (2)
// 	p.drawPixmap( targetRect.width() - bcw, 0, m_bkBorderImg , bcw * 2, 0, bcw, bch );											// (3)
// 	for  ( int i = 0; i < (int)((double)(targetRect.height()) / (bch)-1); i++ )
// 		p.drawPixmap( 0, (i+1) * (bch), m_bkBorderImg , 0, bch, bcw, bch );															// (4)
// 	for  ( int i = 0; i < (int)((double)(targetRect.height()) / (bch)-1); i++ )
// 		p.drawPixmap( targetRect.width() - bcw, (i+1) * (bch), m_bkBorderImg , bcw * 2, bch, bcw, bch );					// (6)
// 	p.drawPixmap(0, targetRect.height() - bch, m_bkBorderImg , 0, bch * 2, bcw, bch );											// (7)
// 	for  ( int i = 0; i < (int)((double)(targetRect.width()) / (bcw)-1); i++ )
// 		p.drawPixmap((i+1) * (bcw), targetRect.height() - bch, m_bkBorderImg , bcw, bch * 2, bcw, bch );					// (8)
// 	p.drawPixmap(targetRect.width() - bcw, targetRect.height() - bch, m_bkBorderImg , bch * 2, bch * 2, bcw, bch );	// (9)

}
void CRoom3DImageWidget::resizeEvent(QResizeEvent * event)
{
	FitBkImageToRect(rect().size());
}

void CRoom3DImageWidget::FitBkImageToRect(QSize a_targetSize)
{
	QSize size = a_targetSize;
	if(!m_bkOrgImg.isNull())
	{
		m_bkImg = m_bkOrgImg.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	}
	else
		m_bkImg = QPixmap();
}