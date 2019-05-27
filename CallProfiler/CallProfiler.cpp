// CallProfiler.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "CallProfiler.h"

#include <fstream>

#include <atlconv.h>

#include "Utils.hpp"

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
	const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

MIDL_DEFINE_GUID(IID, IID_ICorProfilerCallback, 0x176FBED1, 0xA55C, 0x4796, 0x98, 0xCA, 0xA9, 0xDA, 0x0E, 0xF8, 0x83, 0xE7);
MIDL_DEFINE_GUID(IID, IID_ICorProfilerCallback2, 0x8A8CC829, 0xCCF2, 0x49fe, 0xBB, 0xAE, 0x0F, 0x02, 0x22, 0x28, 0x07, 0x1A);
MIDL_DEFINE_GUID(IID, IID_ICorProfilerCallback3, 0x4FD2ED52, 0x7731, 0x4b8d, 0x94, 0x69, 0x03, 0xD2, 0xCC, 0x30, 0x86, 0xC5);



// these are linked in StubX64.asm
EXTERN_C void EnterNaked2(FunctionID funcId,
	UINT_PTR clientData,
	COR_PRF_FRAME_INFO func,
	COR_PRF_FUNCTION_ARGUMENT_INFO *argumentInfo);
EXTERN_C void LeaveNaked2(FunctionID funcId,
	UINT_PTR clientData,
	COR_PRF_FRAME_INFO func,
	COR_PRF_FUNCTION_ARGUMENT_RANGE *retvalRange);
EXTERN_C void TailcallNaked2(FunctionID funcId,
	UINT_PTR clientData,
	COR_PRF_FRAME_INFO func);

EXTERN_C void __stdcall EnterStub(FunctionID functionID, UINT_PTR clientData, COR_PRF_FRAME_INFO frameInfo, COR_PRF_FUNCTION_ARGUMENT_INFO *retvalRange)
{
	CallProfiler::Enter(functionID, clientData, frameInfo, retvalRange);

} // EnterStub

EXTERN_C void __stdcall LeaveStub(FunctionID functionID)
{
	CallProfiler::Leave(functionID);

} // LeaveStub


EXTERN_C void __stdcall TailcallStub(FunctionID functionID)
{
	CallProfiler::Tailcall(functionID);

} // TailcallStub

EXTERN_C void EnterNaked3(FunctionIDOrClientID functionIDOrClientID);
EXTERN_C void LeaveNaked3(FunctionIDOrClientID functionIDOrClientID);
EXTERN_C void TailcallNaked3(FunctionIDOrClientID functionIDOrClientID);



using namespace std;

CallProfiler* g_profiler = NULL;
CallProfiler::CallProfiler()
	: m_buffer(2000),
	  m_stop(false),
	  m_sync(),
	  m_writeThread(&CallProfiler::WriteLogsToDisk, this)
{
	printf("CallProfiler::ctor()\n");
	m_info = NULL;
	m_refCount = 0;
	m_callStackSize = 0;
	//m_functionMap();

}

CallProfiler::~CallProfiler()
{
	// TODO: Currently doing nothing in here
	// All logic was moved into Shutdown() because it was throwing an abort() exception

}

void CallProfiler::Flush()
{
	printf("Writing logs");
	// Lock while swapping buffer, then write to disk
	m_sync.lock();
	
	std::vector<std::string> temp(2000);
	temp.swap(m_buffer);

	m_sync.unlock();

	if (temp.size() == 0)
		return;

	// TODO: Get output file location?
	// Env variable?
	// Config File?
	const std::string oname("E:\\Code\\TEST_" + generate_hex(20) + ".txt");
	std::ofstream outFile(oname);
	for (std::vector<std::string>::const_iterator it = temp.cbegin(); it != temp.cend(); it++)
	{
		outFile << it->c_str();
	}
	outFile.close();
}

void CallProfiler::WriteLogsToDisk()
{
	printf("WriteLogs starting %s \n", m_stop ? "true" : "false");
	// Wait for termination
	while (!m_stop)
	{
		//TODO: determine good size for this 
		for (int i = 0; i < 30 && !m_stop; i++)
			Sleep(500);

		if (m_stop)
			break;
		Flush();
	}
	
	// Write final output
	Flush();
	
	printf("WriteLogs exiting \n");
}

HRESULT STDMETHODCALLTYPE CallProfiler::InitializeForAttach(IUnknown *pICorProfilerInfoUnk, void *pvClientData, UINT cbClientData)
{
	return S_OK;
}

HRESULT CallProfiler::QueryInterface(
	REFIID  riid,
	void ** ppInterface)
{
	printf("CallProfiler::QueryInterface\n");
	if (riid == IID_IUnknown)
	{
		printf("CallProfiler::IID_IUNKNOWN\n");
		*ppInterface = static_cast<ICorProfilerCallback*>(this);

	}
	else if (riid == IID_ICorProfilerCallback)
	{
		printf("--IID_ICorProfilerCallback\n");
		*ppInterface = static_cast<ICorProfilerCallback*>(this);
	}
	else if (riid == IID_ICorProfilerCallback2)
	{
		printf("--IID_ICorProfilerCallback2\n");
		*ppInterface = static_cast<ICorProfilerCallback2*>(this);
	}
	else if (riid == IID_ICorProfilerCallback3)
	{
		printf("--IID_ICorProfilerCallback3\n");
		*ppInterface = static_cast<ICorProfilerCallback3*>(this);
	}
	else
	{
		printf("--IID_NOTIMPL\n");
		*ppInterface = NULL;
		return E_NOTIMPL;
	}
	reinterpret_cast<IUnknown *>(*ppInterface)->AddRef();
	printf("--returning s_OK\n");
	return S_OK;
}

HRESULT CallProfiler::Shutdown()
{
	printf("CallProfiler::Shutdown\n");

	m_stop = true;

	m_writeThread.join();

	//TODO: Force a flush of current function timing to disk.
	// Not sure if this will work...

	if (m_info != NULL)
		m_info->Release();

	if (g_profiler != NULL)
	{
		g_profiler->Release();
		g_profiler = NULL;
	}

	// Test to see if we destruct when service exits
	std::ofstream outFile("E:\\Code\\shutdown.bin");
	outFile << "Destructor!\n";
	outFile.close();
	return S_OK;
}

STDMETHODIMP CallProfiler::MovedReferences(ULONG cMovedObjectIDRanges, ObjectID oldObjectIDRangeStart[], ObjectID newObjectIDRangeStart[], ULONG cObjectIDRangeLength[])
{
	return S_OK;
}

STDMETHODIMP CallProfiler::SurvivingReferences(ULONG cSurvivingObjectIDRanges, ObjectID objectIDRangeStart[], ULONG cObjectIDRangeLength[])
{
	return S_OK;
}

STDMETHODIMP CallProfiler::RootReferences2(ULONG cRootRefs, ObjectID rootRefIds[], COR_PRF_GC_ROOT_KIND rootKinds[], COR_PRF_GC_ROOT_FLAGS rootFlags[], UINT_PTR rootIds[])
{
	return S_OK;
}


STDMETHODIMP CallProfiler::ObjectReferences(ObjectID objectId, ClassID classId, ULONG cObjectRefs, ObjectID objectRefIds[])
{
	return S_OK;
}

STDMETHODIMP CallProfiler::HandleCreated(GCHandleID handleId, ObjectID initialObjectId)
{
	return S_OK;
}

STDMETHODIMP CallProfiler::HandleDestroyed(GCHandleID handleId)
{
	return S_OK;
}

STDMETHODIMP CallProfiler::GarbageCollectionStarted(int cGenerations, BOOL generationCollected[], COR_PRF_GC_REASON reason)
{
	return S_OK;
}


#if defined(_M_IX86)
// see http://msdn.microsoft.com/en-us/library/4ks26t93.aspx  for inline assembly.   Not supported on X64.   

void __declspec(naked) __stdcall EnterMethodNaked(FunctionID funcID)
{
	__asm
	{
		push eax
		push ecx
		push edx
		push[esp + 16]		// Push the function ID
		call EnterMethod
		pop edx
		pop ecx
		pop eax
		ret 4
	}
} // EnterNaked

  // Currently we don't care about the leave method (can we avoid making the call?
void __declspec(naked) __stdcall LeaveMethodNaked(FunctionID funcID)
{
	__asm
	{
		ret 4
	}
}

void __declspec(naked) __stdcall TailcallMethodNaked(FunctionID funcID)
{
	__asm
	{
		push eax
		push ecx
		push edx
		push[esp + 16]
		call TailcallMethod
		pop edx
		pop ecx
		pop eax
		ret 4
	}
}

#else
EXTERN_C void __stdcall EnterMethodNaked(FunctionID functionID);
EXTERN_C void __stdcall LeaveMethodNaked(FunctionID functionID);
EXTERN_C void __stdcall TailcallMethodNaked(FunctionID functionID);
#endif 


//==============================================================================
STDMETHODIMP CallProfiler::GarbageCollectionFinished(void)
{
	return S_OK;
}

//==============================================================================
STDMETHODIMP CallProfiler::FinalizeableObjectQueued(DWORD finalizerFlags, ObjectID objectID)
{

	return S_OK;
}

STDMETHODIMP CallProfiler::ObjectAllocated(ObjectID objectId, ClassID classId)
{
	return S_OK;
}

void CallProfiler::Leave(FunctionID functionID)
{
	g_profiler->LeaveHandler(functionID);
}

void CallProfiler::Tailcall(FunctionID functionID)
{
	g_profiler->TailHandler(functionID);
}

void CallProfiler::Enter(FunctionID functionID, UINT_PTR clientData, COR_PRF_FRAME_INFO frameInfo, COR_PRF_FUNCTION_ARGUMENT_INFO *retvalRange)
{
	if (g_profiler != NULL)
	{
		g_profiler->EnterHandler(functionID, clientData, frameInfo, retvalRange);
	}
}

void CallProfiler::TailHandler(FunctionID functionID)
{
	CFunctionInfo* functionInfo = NULL;
	std::map<FunctionID, CFunctionInfo*>::iterator iter = m_functionMap.find(functionID);
	if (iter != m_functionMap.end())
	{
		// get it from the map and update it
		functionInfo = (iter->second);

		ThreadID tid = GetThreadID();

		char buffer[250];
		snprintf(buffer, 250, "TAIL %I64u %zu %s\n", GetTickCount64(), tid, functionInfo->GetName());
		m_sync.lock();
		m_buffer.push_back(std::string(buffer));
		m_sync.unlock();
		//printf("Tail %I64u %zu %s\n", GetTickCount64(), tid, functionInfo->GetName());

	}
	else
	{
		//// log an error (this shouldn't happen because we're caching the functions
		//// in the function mapping callback
		printf("Error finding function ID %d in the map.\r\n", (int)functionID);
	}
}

void CallProfiler::LeaveHandler(FunctionID functionID)
{
	CFunctionInfo* functionInfo = NULL;
	std::map<FunctionID, CFunctionInfo*>::iterator iter = m_functionMap.find(functionID);
	if (iter != m_functionMap.end())
	{
		// get it from the map and update it
		functionInfo = (iter->second);

		ThreadID tid = GetThreadID();

		char buffer[250];
		snprintf(buffer, 250, "LEAVE %I64u %zu %s\n", GetTickCount64(), tid, functionInfo->GetName());
		m_sync.lock();
		m_buffer.push_back(std::string(buffer));
		m_sync.unlock();
		//printf("Leave: %I64u %zu %s\n", GetTickCount64(), tid, functionInfo->GetName());

	}
	else
	{
		//// log an error (this shouldn't happen because we're caching the functions
		//// in the function mapping callback
		printf("Error finding function ID %d in the map.\r\n", (int)functionID);
	}
}


void CallProfiler::EnterHandler(FunctionID functionID, UINT_PTR clientData, COR_PRF_FRAME_INFO frameInfo, COR_PRF_FUNCTION_ARGUMENT_INFO *argInfo)
{
	CFunctionInfo* functionInfo = NULL;
	std::map<FunctionID, CFunctionInfo*>::iterator iter = m_functionMap.find(functionID);
	if (iter != m_functionMap.end())
	{
		// get it from the map and update it
		functionInfo = (iter->second);
		
		ThreadID tid = GetThreadID();

		char buffer[250];
		snprintf(buffer, 250, "ENTER %I64u %zu %s\n", GetTickCount64(), tid, functionInfo->GetName());
		m_sync.lock();
		m_buffer.push_back(std::string(buffer));
		m_sync.unlock();
			
		//printf("EnterMethod(Nargs): %I64u %zu %s\n", GetTickCount64(), tid, functionInfo->GetName());
		
	}
	else
	{
		//// log an error (this shouldn't happen because we're caching the functions
		//// in the function mapping callback
		printf("Error finding function ID %d in the map.\r\n", (int)functionID);
	}

}


__forceinline ThreadID CallProfiler::GetThreadID()
{
	
	ThreadID tid = 0;
	g_profiler->m_info->GetCurrentThreadID(&tid);
	return tid;
}



void CallProfiler::EnterHandler(FunctionID functionID)
{
	CFunctionInfo* functionInfo = NULL;
	std::map<FunctionID, CFunctionInfo*>::iterator iter = m_functionMap.find(functionID);
	if (iter != m_functionMap.end())
	{
		// get it from the map and update it
		functionInfo = (iter->second);
		//printf("EnterMethod: %I64u %s\n", GetTickCount64(), functionInfo->GetName());
		
	}
	else
	{
		//// log an error (this shouldn't happen because we're caching the functions
		//// in the function mapping callback
		//printf("Error finding function ID %d in the map.\r\n", (int)functionID);
	}
}

EXTERN_C void __stdcall EnterMethod(FunctionID functionID)
{
}

EXTERN_C void __stdcall TailcallMethod(FunctionID functionID)
{
	printf("TailcallMethod\n");
}

UINT_PTR CallProfiler::FunctionMapper(FunctionID functionID, BOOL *pbHookFunction)
{
	if (g_profiler != NULL)
	{
		g_profiler->AddFunction(functionID);
	}
	return (UINT_PTR)functionID;
}

void CallProfiler::AddFunction(FunctionID functionid)
{
	CFunctionInfo* functionInfo = NULL;
	std::map<FunctionID, CFunctionInfo*>::iterator iter = m_functionMap.find(functionid);

	if (iter == m_functionMap.end())
	{
		
		//printf("Looking up functionid: %d\n", functionid);

		IMetaDataImport* pIMetaDataImport = 0;
		HRESULT hr = S_OK;
		mdToken funcToken = 0;
		WCHAR szFunction[MAX_CLASS_NAME];
		WCHAR szClass[MAX_CLASS_NAME];

		// get the token for the function which we will use to get its name
		hr = g_profiler->m_info->GetTokenAndMetaDataFromFunction(functionid, IID_IMetaDataImport, (LPUNKNOWN *)&pIMetaDataImport, &funcToken);
		if (SUCCEEDED(hr))
		{
			mdTypeDef classTypeDef;
			ULONG cchFunction;
			ULONG cchClass;

			// retrieve the function properties based on the token
			hr = pIMetaDataImport->GetMethodProps(funcToken, &classTypeDef, szFunction, MAX_CLASS_NAME, &cchFunction, 0, 0, 0, 0, 0);
			if (SUCCEEDED(hr))
			{
				// get the function name
				hr = pIMetaDataImport->GetTypeDefProps(classTypeDef, szClass, MAX_CLASS_NAME, &cchClass, 0, 0);
				if (SUCCEEDED(hr))
				{
					// create the fully qualified name
						//printf("EnterMethod: %I64u %S.%S\n", GetTickCount64(), szClass, szFunction);
						WCHAR szMethodName[1024];
					_snwprintf_s(szMethodName, 1024, 1024, L"%s.%s", szClass, szFunction);
					USES_CONVERSION;
					CFunctionInfo* funcinfo = new CFunctionInfo(functionid, W2A(szMethodName));
					m_functionMap.insert(std::pair<FunctionID, CFunctionInfo*>(functionid, funcinfo));
					
				}
			}
			// release our reference to the metadata
			pIMetaDataImport->Release();
		}
	}
}

HRESULT CallProfiler::Initialize(IUnknown *pICorProfilerInfoUnk)
{
	//printf("cp::initialize");

	//return S_OK;

	printf("CallProfiler::Initialize\n");

	m_startTime = time(NULL);

	HRESULT             hr = S_OK;

	// Get current process name -> See if its one we want to profile

	TCHAR szFileName[MAX_PATH + 1];

	DWORD res = GetModuleFileName(NULL, szFileName, MAX_PATH + 1);

	std::wstring name(szFileName);

	printf("ProcessName: %d %S \n" , res, szFileName);

	printf("\n");

	bool wantToProfile = true;

	// TODO: Compare processname to determine if we want to profile

	if(!wantToProfile)
		return S_FALSE;
	

	CALL_N_LOGONBADHR(pICorProfilerInfoUnk->QueryInterface(__uuidof(ICorProfilerInfo3), (void **)&m_info));

	// Even if we did not ask for them, turn on the profiler flags that can ONLY be done at startup. 
	DWORD oldFlags = 0;
	m_info->SetEventMask(COR_PRF_MONITOR_ENTERLEAVE);
	
	// TODO: Enabling function args may be useful, but will likely result in slow behavior
	// TODO: Also need to figure out how to turn argument range info struct into actual arguments
	// TODO: Need to lookup the functions metadata to get expected types, then translate address ranges to args
	//COR_PRF_ENABLE_FUNCTION_ARGS);
	//m_info->SetEnterLeaveFunctionHooks(EnterMethodNaked, LeaveMethodNaked, TailcallMethodNaked);
	m_info->SetEnterLeaveFunctionHooks2(EnterNaked2, LeaveNaked2, TailcallNaked2);

	m_info->SetFunctionIDMapper((FunctionIDMapper*)&FunctionMapper);

	g_profiler = this;

	

	return S_FALSE;
	
	printf("Initialize() returns %x\n" + hr);
	return hr;

exit:
	printf("Initialize() returns %x\n" + hr);
	return hr;
	
}