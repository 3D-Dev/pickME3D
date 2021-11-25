//----------------------------------------------//
//	File: Room3DStep2Cmd.h						//
//	Desc:										//
//	Update: 2012.6  (by 001	)				//
//----------------------------------------------//
#pragma once
#include "Room3DBaseCmd.h"
/*!
	\class CRoom3DStep2Cmd
	\brief This is class to process all actions on the step2.(by 001, 2012/6/26).
	\sa 
*/
class CRoom3DStep2Cmd : public CRoom3DBaseCmd
{
public:
	CRoom3DStep2Cmd(void);
	~CRoom3DStep2Cmd(void);

	virtual void ResetDocument();
protected:
	virtual void MousePressEvent(QMouseEvent * a_pEvent);
	virtual void MouseReleaseEvent(QMouseEvent * a_pEvent);
	virtual void MouseMoveEvent(QMouseEvent * a_pEvent);
};
