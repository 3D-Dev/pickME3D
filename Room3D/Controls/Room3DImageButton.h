//----------------------------------------------//
//	File: Room3DMenuBar.h								//
//	Desc: 										//
//	Update: 2012.6  (by 001	)				//
//----------------------------------------------//
#ifndef IMAGEBUTTON_H
#define IMAGEBUTTON_H

#include <QtGui>
#include "Define.h"
#include <QMenu>

class CRoom3DImageWidget : public QWidget
{
	Q_OBJECT
public:
// 	CRoom3DImageWidget(QString a_sImgPath, QWidget* parent = NULL){ CRoom3DImageWidget(a_sImgPath, false, parent); }
	CRoom3DImageWidget(QString a_sImgPath, QString a_sBorderImgPath, int a_sBorderThickness, bool a_stretchMode, QWidget* parent = NULL);
	CRoom3DImageWidget(QWidget* parent = NULL): m_blTransparent(true),m_bkLeftTopColor(0,0,0), m_bkRightBottomColor(0,0,0), QWidget(parent){};
	~CRoom3DImageWidget();
	void SetImage(QString a_sImgPath, QString a_sBorderImgPath, int a_sBorderThickness, bool a_stretchMode);
	void SetBkColor(QColor a_bkLeftTopColor, QColor a_bkRightBottomColor){m_bkLeftTopColor = a_bkLeftTopColor; m_bkRightBottomColor = a_bkRightBottomColor; m_blTransparent = false;};
protected:
	virtual void paintEvent(QPaintEvent * event);
	virtual void resizeEvent(QResizeEvent * event);
	void FitBkImageToRect(QSize a_targetSize);		// Function to stretch image to fit widget
protected:
	QPixmap     m_bkImg;		//Stretched image
	QPixmap     m_bkOrgImg;		// Original image
	QPixmap     m_bkBorderImg;		// Border image
	int			m_borderThickness;		// Thickness of Border
	bool		m_stretchMode;		// Image drawing mode ( true : stretch, false : tile )
	QColor      m_bkLeftTopColor;
	QColor      m_bkRightBottomColor;
	bool		m_blTransparent;
};
/*!
	\class CRoom3DUndoCmd
	\brief This is button class to  display different image according to the button state.(by 001, 2012/6/24).
	\sa 
*/
class CRoom3DImageButton : public QPushButton
{
    Q_OBJECT
public:
	CRoom3DImageButton(QString a_sImgPath, int a_nButtonID = -1, Qt::CursorShape cursor = Qt::ArrowCursor, QWidget* parent = NULL);
    ~CRoom3DImageButton();
	void SetPixmap(QPixmap a_pixmap);
	void SetPixmap(QString a_sImgPath);
	void SetMenu(QMenu* pMenu);
private:
    QPixmap     m_bkImg;
	QPixmap     m_bkOrgImg;
    BUTTON_STATE    m_state;
	bool	m_pressed;
	int		m_nBtnID;
	Qt::CursorShape m_cursoryShape;

	QMenu* m_pMenu;

public:
    void setState(BUTTON_STATE state = BTN_STATE_NORMAL, bool blWithLeaveEvent = false);
    BUTTON_STATE getState() { return m_state; }

signals:
    void pressed(int nBtnID);

public slots:
	void OnShowMenu();
	void OnHideMenu();

protected:
    virtual void paintEvent(QPaintEvent *e);
	virtual void resizeEvent ( QResizeEvent * event );
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
	void leaveEvent(QEvent *e);
	void enterEvent(QEvent *e);

	void UpdateButtonStateImg();
};

#endif // IMAGEBUTTON_H
