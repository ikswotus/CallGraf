#pragma once

#ifndef __THREADINFO_H
#define __THREADINFO_H



class ThreadInfo :
	public BaseInfo
{
public:

	ThreadInfo(ThreadID threadID, SIZE_T internal = 0);
	virtual ~ThreadInfo();


public:

	virtual void Dump();


public:

	DWORD  m_win32ThreadID;
	LStack *m_pThreadCallStack;
	LStack *m_pLatestUnwoundFunction;

	StackTraceInfo *m_pLatestStackTraceInfo;

}; // ThreadInfo

#endif // !__THREADINFO_H