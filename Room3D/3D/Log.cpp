
#include "Log.h"

CLog::CLog(char *Logfilename)
{
	m_Logfileptr = fopen(Logfilename, "w");
	if(m_Logfileptr)
	{
		m_IsLog = true;
		Write("\t\t\t        3DS FILES VIEWER\n"); 
		Write("\t\t\t++----+--+++++--+--+++++--+-----++");
		Write("\t\t\t++--+++++--+--+++++--+--+++++---++");
		Write("\t\t\t++----+--+++++--+--+++++--+-----++\n\n");
	}
	else
		m_IsLog = false;

}

CLog::~CLog()
{
	if(m_IsLog)
	{
		Write("\n\n\t\t\t++----+--+++++--+--+++++--+-----++");
		Write("\t\t\t++--+++++--+--+++++--+--+++++---++");
		Write("\t\t\t++----+--+++++--+--+++++--+-----++\n");
		Write("\t\t\t    hassanibi@hotmail.com  (2006)\n"); 
		fclose(m_Logfileptr);
	}

}

void CLog::Write(char *format, ...)
{
	char *args;
    char buffer[1024];
	args = (char *)&format + ((sizeof(format) + sizeof(int) - 1) & ~(sizeof(int) - 1));
    vsprintf(buffer, format, args);
	args = (char *)0;
	if(m_IsLog)
		fprintf(m_Logfileptr, "%s\n", buffer);
  
}