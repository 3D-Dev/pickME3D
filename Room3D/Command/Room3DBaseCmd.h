//----------------------------------------------//
//	File: Room3DBaseCmd.h						//
//	Desc:										//
//	Update: 2012.6  (by 001	)				//
//----------------------------------------------//
#pragma once
#include <QMouseEvent>
#include <QKeyEvent>
/*!
	\class CRoom3DBaseCmd
	\brief This is base class to all edit command.(by 001, 2012/6/26).
	\sa 
*/
class CRoom3DDocument;
class CRoom3DSceneView;
class CRoom3DBaseCmd
{
	friend class CRoom3DSceneView;
public:
	CRoom3DBaseCmd(void);
	~CRoom3DBaseCmd(void);

	virtual void InitCommand(){};
	virtual void ResetDocument(){};
protected:
	virtual void MousePressEvent(QMouseEvent * a_pEvent){};
	virtual void MouseReleaseEvent(QMouseEvent * a_pEvent){};
	virtual void MouseMoveEvent(QMouseEvent * a_pEvent){};
	virtual void WheelEvent(QWheelEvent* a_pEvent){};

	virtual void keyPressEvent ( QKeyEvent * a_pEvent ) {};
	virtual void keyReleaseEvent ( QKeyEvent * a_pEvent ){};

protected:
	CRoom3DDocument* m_pDocument;
	CRoom3DSceneView* m_pSceneView;
};
