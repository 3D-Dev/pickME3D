//----------------------------------------------//
//	File: Room3DBaseCmd.h						//
//	Desc:										//
//	Update: 2012.6  (by 001	)				//
//----------------------------------------------//
#pragma once
#include "Room3DBaseCmd.h"

enum COMMAND_ID
{
	CMD_NULL,
	CMD_STEP1,
	CMD_STEP2,
	CMD_STEP3
};

/*!
	\class CRoom3DBaseCmd
	\brief This is base class to all edit command.(by 001, 2012/6/26).
	\sa 
*/

class CRoom3DCommandMgr
{
public:
	CRoom3DCommandMgr(void);
	~CRoom3DCommandMgr(void);
	CRoom3DBaseCmd* GetCurrentCmd(){return m_pCurrentCmd;};
	bool CreateCommand(COMMAND_ID a_cmdID);
protected:
	CRoom3DBaseCmd* m_pCurrentCmd;
};
