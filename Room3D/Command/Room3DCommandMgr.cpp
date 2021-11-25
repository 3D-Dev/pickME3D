#include "Room3DCommandMgr.h"
#include "Room3DStep1Cmd.h"
#include "Room3DStep2Cmd.h"
#include "Room3DStep3Cmd.h"
#include "Define.h"

CRoom3DCommandMgr::CRoom3DCommandMgr(void)
{
	m_pCurrentCmd = NULL;
}

CRoom3DCommandMgr::~CRoom3DCommandMgr(void)
{
	SAFE_DELETE(m_pCurrentCmd);
}

bool CRoom3DCommandMgr::CreateCommand(COMMAND_ID a_cmdID)
{
	CRoom3DBaseCmd* pCmd = NULL;
	switch(a_cmdID)
	{
	case CMD_STEP1:
		pCmd = new CRoom3DStep1Cmd();
		break;
	case CMD_STEP2:
		pCmd = new CRoom3DStep2Cmd();
		break;
	case CMD_STEP3:
		pCmd = new CRoom3DStep3Cmd();
		break;
	}
	if(pCmd)
	{
		SAFE_DELETE(m_pCurrentCmd);
		m_pCurrentCmd = pCmd;
		m_pCurrentCmd->InitCommand();
		return true;
	}
	return false;
}