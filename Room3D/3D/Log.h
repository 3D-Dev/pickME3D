
#ifndef _Log_H
#define _Log_H

#include <stdio.h>

class CLog
{
public:

	CLog(char *Logfilename);
	~CLog();
	bool IsLog() {return m_IsLog;}
	void Write(char *format, ...);

private:

	char *m_Logfilename;
	FILE *m_Logfileptr;
	bool m_IsLog;

};


#endif  // _Log_H