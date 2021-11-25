#include "Room3DUndoMgr.h"
#include "Define.h"

CRoom3DUndoMgr::CRoom3DUndoMgr(void)
{
	m_nCurUndoPos = 0;
}

CRoom3DUndoMgr::~CRoom3DUndoMgr(void)
{
	Clear();
}

void CRoom3DUndoMgr::Clear()
{
	for (int i = 0; i < m_UndoCmds.size(); ++i) {
		CRoom3DUndoCmd* pUndoCmd = m_UndoCmds.at(i);
		SAFE_DELETE(pUndoCmd);
	}
	m_UndoCmds.clear();
	m_nCurUndoPos = 0;
}

bool CRoom3DUndoMgr::AddUndoCmd(CRoom3DUndoCmd* a_pCmd)
{
	if(a_pCmd == NULL)
		return false;
	if(!a_pCmd->IsUndoable())
		return false;
	// Remove all command in back than current pos.
	int nBackCmdCnt = m_UndoCmds.size() - m_nCurUndoPos - 1;
	while(m_UndoCmds.size() > m_nCurUndoPos)
	{
		CRoom3DUndoCmd* pUndoCmd = m_UndoCmds.back();
		SAFE_DELETE(pUndoCmd);
		m_UndoCmds.pop_back();
	}
	m_UndoCmds.push_back(a_pCmd);
	m_nCurUndoPos++;
	return true;
}

bool CRoom3DUndoMgr::Undo()
{
	// decreas cur pos by 1
	m_nCurUndoPos--;
	if(m_nCurUndoPos < 0 || m_nCurUndoPos >= m_UndoCmds.size())
	{
		m_nCurUndoPos = 0;
		return false;
	}
	// Undo of cur undo command, then decrease cur pos as 1
	return m_UndoCmds.at(m_nCurUndoPos)->Undo();
}

bool CRoom3DUndoMgr::Redo()
{
	// If cur pos is not unavailable , then adjust cur pos and return;
	if(m_nCurUndoPos < 0)
		return false;
	if(m_nCurUndoPos >= m_UndoCmds.size())
	{
		m_nCurUndoPos = m_UndoCmds.size();
		return false;
	}
	bool retVal;
	// Redo of cur undo command
	retVal = m_UndoCmds.at(m_nCurUndoPos)->Redo();
	// Increase cur pos by 1.
	m_nCurUndoPos++;
	return retVal;
}

bool CRoom3DUndoMgr::IsUndoAble()
{
	return m_nCurUndoPos <= 0 ? false : true;
}

bool CRoom3DUndoMgr::IsRedoAble()
{
	return m_nCurUndoPos > (m_UndoCmds.size() - 1) ? false : true;
}

