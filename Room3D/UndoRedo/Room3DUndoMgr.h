//----------------------------------------------//
//	File: Room3DUndoMgr.h						//
//	Desc: 										//
//	Update: 2012.6  (by 001	)				//
//----------------------------------------------//
#pragma once
#include <QVector>
#include "Room3DUndoCmd.h"

/*!
	\class CRoom3DUndoMgr
	\brief This is class to manage undo commands.(by 001, 2012/6/23).
	\sa 
*/
class CRoom3DUndoMgr
{
public:
	CRoom3DUndoMgr(void);
	~CRoom3DUndoMgr(void);
	bool AddUndoCmd(CRoom3DUndoCmd* a_pCmd);
	void Clear();
	bool Undo();
	bool Redo();
	bool IsUndoAble();
	bool IsRedoAble();
protected:
	QVector<CRoom3DUndoCmd*> m_UndoCmds;
	int m_nCurUndoPos;
};
