//----------------------------------------------//
//	File: Room3DStep1Cmd.h						//
//	Desc:										//
//	Update: 2012.6  (by 001	)				//
//----------------------------------------------//
#pragma once
#include "Room3DBaseCmd.h"

/*!
	\class CRoom3DStep1Cmd
	\brief This is class to process all actions on the step1.(by 001, 2012/6/26).
	\sa 
*/
class CRoom3DStep1Cmd : public CRoom3DBaseCmd
{
public:
	CRoom3DStep1Cmd(void);
	~CRoom3DStep1Cmd(void);

	virtual void ResetDocument();

};
