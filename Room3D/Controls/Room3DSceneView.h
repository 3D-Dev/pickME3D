//----------------------------------------------//
//	File: Room3DSceneView.h						//
//	Desc: 										//
//	Update: 2012.6  (by 001	)				//
//----------------------------------------------//
#pragma once
#include <QGLWidget>
#include "Global.h"
#include "Room3DImageButton.h"
#include <QTime>

struct ITEM_INFO;

/*!
	\class CRoom3DSceneView
	\brief This is view class to render room image and furnitures.(by 001, 2012/6/21).
	\sa 
*/
class CRoom3DSceneView : public QGLWidget
{
	Q_OBJECT
public:
	CRoom3DSceneView(QWidget* parent = 0);
	~CRoom3DSceneView(void);

	QRect GetViewportRect();
protected:

	virtual void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();

	virtual void mousePressEvent(QMouseEvent * a_pEvent);
	virtual void mouseReleaseEvent(QMouseEvent * a_pEvent);
	virtual void mouseMoveEvent(QMouseEvent * a_pEvent);
	virtual void wheelEvent(QWheelEvent* a_pEvent);

	virtual void keyPressEvent ( QKeyEvent * a_pEvent );
	virtual void keyReleaseEvent ( QKeyEvent * a_pEvent );

protected:
	void dragEnterEvent(QDragEnterEvent *event);
	void dragMoveEvent(QDragMoveEvent *event);
	void dragLeaveEvent(QDragLeaveEvent *event);
	void dropEvent(QDropEvent *event);
private:
	ITEM_INFO* GetDropedItemInfo(QDragEnterEvent* a_pEvent);
#if SHOW_FPS
	QTime	m_time;
	int		m_nRenderCount;
	long	m_nTotalTickCount;
	float	m_fFPS;
#endif
};

class CRoom3DSceneViewWrapper : public CRoom3DImageWidget
{
	Q_OBJECT
	friend class CRoom3DSceneView;
public:
	CRoom3DSceneViewWrapper(QWidget* a_pParent);
	~CRoom3DSceneViewWrapper(){};
	void UpdateSceneVeiwLayout();
protected:
	void paintEvent(QPaintEvent * event);
	void resizeEvent(QResizeEvent * event);
protected:
	QRect	m_ScenViewRect;
};