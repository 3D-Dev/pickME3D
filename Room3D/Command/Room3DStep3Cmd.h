//----------------------------------------------//
//	File: Room3DStep3Cmd.h						//
//	Desc:										//
//	Update: 2012.6  (by 001	)				//
//----------------------------------------------//
#pragma once
#include "Room3DBaseCmd.h"
#include "Math3D.h"
#include "Global.h"

/*!
	\class CRoom3DStep3Cmd
	\brief This is class to process all actions on the step3.(by 001, 2012/6/26).
	\sa 
*/
class CRoom3DStep3Cmd : public CRoom3DBaseCmd
{
public:
	CRoom3DStep3Cmd(void);
	~CRoom3DStep3Cmd(void);

	virtual void ResetDocument();
protected:
	virtual void MousePressEvent(QMouseEvent * a_pEvent);
	virtual void MouseReleaseEvent(QMouseEvent * a_pEvent);
	virtual void MouseMoveEvent(QMouseEvent * a_pEvent);
	virtual void WheelEvent(QWheelEvent* a_pEvent);

	virtual void keyPressEvent ( QKeyEvent * a_pEvent );
	virtual void keyReleaseEvent ( QKeyEvent * a_pEvent );

protected:
	bool m_bRBFlag;
	bool m_bLBFlag;
	int m_nPrevX;
	int m_nPrevY;

	vec3 m_startPos;
	vec3 m_deltaPos;
	float m_fStartRotY;
};
