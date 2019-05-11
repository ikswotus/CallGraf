#pragma once

//#include "cor.h"
//#include "corprof.h"
#include "CallProfiler.h"

class CFunctionInfo
{
public:
	CFunctionInfo(FunctionID functionID, const char* name);
	virtual ~CFunctionInfo();

	char* GetName();
	FunctionID GetFunctionID();
	long GetCallCount();
	void IncrementCallCount();

private:
	FunctionID m_functionID;
	char* m_name;
	long m_callCount;

};