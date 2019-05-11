// CallProfiler.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "CallProfiler.h"

#include <fstream>


#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
	const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

MIDL_DEFINE_GUID(IID, IID_ICorProfilerCallback, 0x176FBED1, 0xA55C, 0x4796, 0x98, 0xCA, 0xA9, 0xDA, 0x0E, 0xF8, 0x83, 0xE7);
MIDL_DEFINE_GUID(IID, IID_ICorProfilerCallback2, 0x8A8CC829, 0xCCF2, 0x49fe, 0xBB, 0xAE, 0x0F, 0x02, 0x22, 0x28, 0x07, 0x1A);
MIDL_DEFINE_GUID(IID, IID_ICorProfilerCallback3, 0x4FD2ED52, 0x7731, 0x4b8d, 0x94, 0x69, 0x03, 0xD2, 0xCC, 0x30, 0x86, 0xC5);


//CallProfiler *g_pCallbackObject;        // global reference to callback object

using namespace std;

CallProfiler* g_profiler = NULL;

// our real handler for FunctionLeave notification
void CallProfiler::Leave(FunctionID functionID, UINT_PTR clientData, COR_PRF_FRAME_INFO frameInfo, COR_PRF_FUNCTION_ARGUMENT_RANGE *argumentRange)
{
	// decrement the call stack size
	if (m_callStackSize > 0)
		m_callStackSize--;
}

// our real handler for the FunctionTailcall notification
void CallProfiler::Tailcall(FunctionID functionID, UINT_PTR clientData, COR_PRF_FRAME_INFO frameInfo)
{
	// decrement the call stack size
	if (m_callStackSize > 0)
		m_callStackSize--;
}



void CallProfiler::Enter(FunctionID functionID, UINT_PTR clientData, COR_PRF_FRAME_INFO frameInfo, COR_PRF_FUNCTION_ARGUMENT_INFO *argumentInfo)
{
	// see if this function is in the map.  It should be since we are using the funciton mapper
	CFunctionInfo* functionInfo = NULL;
	std::map<FunctionID, CFunctionInfo*>::iterator iter = m_functionMap.find(functionID);
	if (iter != m_functionMap.end())
	{
		// get it from the map and update it
		functionInfo = (iter->second);
		// increment the call count
		functionInfo->IncrementCallCount();
		// create the padding based on the call stack size
		int padCharCount = m_callStackSize * 2;
		if (padCharCount > 0)
		{
			char* padding = new char[(padCharCount)+1];
			memset(padding, 0, padCharCount + 1);
			memset(padding, ' ', padCharCount);
			// log the function call
			printf("%s%s, id=%d, call count = %d\r\n", padding, functionInfo->GetName(), functionInfo->GetFunctionID(), functionInfo->GetCallCount());
			delete padding;
		}
		else
		{
			// log the function call
			printf("%s, id=%d, call count = %d\r\n", functionInfo->GetName(), functionInfo->GetFunctionID(), functionInfo->GetCallCount());
		}
	}
	else
	{
		// log an error (this shouldn't happen because we're caching the functions
		// in the function mapping callback
		printf("Error finding function ID %d in the map.\r\n", (int)functionID);
	}
	// increment the call stack size (we must do this even if we don't find the 
	// function in the map
	m_callStackSize++;
}

CallProfiler::CallProfiler() //: corProfilerInfo(NULL)
{
	printf("CallProfiler::ctor()\n");
	m_info = NULL;
	m_refCount = 0;
	m_callStackSize = 0;
	//m_functionMap();
}

CallProfiler::~CallProfiler()
{
	printf("CallProfiler::dtor\n");
	if (m_info != NULL)
		m_info->Release();
	if (g_profiler != NULL)
	{

	}
	/*if (this->corProfilerInfo != nullptr)
	{
		this->corProfilerInfo->Release();
		this->corProfilerInfo = nullptr;
	}
	g_pCallbackObject = NULL;*/
}

HRESULT STDMETHODCALLTYPE CallProfiler::InitializeForAttach(
	/* [in] */ IUnknown *pICorProfilerInfoUnk,
	/* [in] */ void *pvClientData,
	/* [in] */ UINT cbClientData)
{
	HRESULT             hr = S_OK;
	printf("cp::initializeforattach\n");
	return hr;
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
	// TODO FIX NOW add support or ICorProfilerCallback4 (for large objects)
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
	printf("Shutdown\n");
	if (m_info != NULL)
		m_info->Release();
	m_info = NULL;
	return S_OK;
}

STDMETHODIMP CallProfiler::MovedReferences(ULONG cMovedObjectIDRanges, ObjectID oldObjectIDRangeStart[], ObjectID newObjectIDRangeStart[], ULONG cObjectIDRangeLength[])
{
	/*LOG_TRACE(L"Moved Ref\n");
	const int maxCount = MaxEventPayload / (1 * sizeof(int) + 2 * sizeof(void*));
	for (ULONG idx = 0; idx < cMovedObjectIDRanges; idx += maxCount)
	{
		EventWriteObjectsMovedEvent(min(cMovedObjectIDRanges - idx, maxCount),
			(const void**)&oldObjectIDRangeStart[idx], (const void**)&newObjectIDRangeStart[idx], (const unsigned int*)&cObjectIDRangeLength[idx]);
	}*/
	return S_OK;
}

STDMETHODIMP CallProfiler::SurvivingReferences(ULONG cSurvivingObjectIDRanges, ObjectID objectIDRangeStart[], ULONG cObjectIDRangeLength[])
{
	/*LOG_TRACE(L"Surviving references\n");
	const int maxCount = MaxEventPayload / (1 * sizeof(int) + 1 * sizeof(void*));
	for (ULONG idx = 0; idx < cSurvivingObjectIDRanges; idx += maxCount)
	{
		EventWriteObjectsSurvivedEvent(min(cSurvivingObjectIDRanges - idx, maxCount),
			(const void**)&objectIDRangeStart[idx], (const unsigned int*)&cObjectIDRangeLength[idx]);
	}*/
	return S_OK;
}

STDMETHODIMP CallProfiler::RootReferences2(ULONG cRootRefs, ObjectID rootRefIds[], COR_PRF_GC_ROOT_KIND rootKinds[], COR_PRF_GC_ROOT_FLAGS rootFlags[], UINT_PTR rootIds[])
{
	// If we did not ask for the GCHeap events, do nothing.  
	/*if ((m_currentKeywords & GCHeapKeyword) == 0)
		return S_OK;

	LOG_TRACE(L"RootReferences2\n");
	const int maxCount = MaxEventPayload / (2 * sizeof(int) + 2 * sizeof(void*));
	for (ULONG idx = 0; idx < cRootRefs; idx += maxCount)
	{
		EventWriteRootReferencesEvent(min(cRootRefs - idx, maxCount),
			(const void**)&rootRefIds[idx], (unsigned int*)&rootKinds[idx], (unsigned int*)&rootFlags[idx], (const void**)&rootIds[idx]);
	}*/
	return S_OK;
}


STDMETHODIMP CallProfiler::ObjectReferences(ObjectID objectId, ClassID classId, ULONG cObjectRefs, ObjectID objectRefIds[])
{
	//if ((m_currentKeywords & GCHeapKeyword) == 0)
	//	return S_OK;
	//// LOG_TRACE(L"ObjectReferences\n");

	//// We do this for the side effect of logging the class  
	//ClassInfo* classInfo = GetClassInfo(classId);
	///** TODO FIX NOW
	//if (classInfo == NULL)
	//return E_FAIL;
	//**/
	//ULONG size = 0;
	//m_info->GetObjectSize(objectId, &size);

	//EventWriteObjectReferencesEvent(objectId, classId, size, cObjectRefs, (const void**)objectRefIds);
	return S_OK;
}

//==============================================================================
STDMETHODIMP CallProfiler::HandleCreated(GCHandleID handleId, ObjectID initialObjectId)
{
//	if ((m_currentKeywords & (GCHeapKeyword | GCAllocKeyword | GCAllocSampledKeyword)) == 0)
//		return S_OK;
//
//	LOG_TRACE(L"HandleCreated\n");
//#ifndef PIN_INVESTIGATION
//	// TODO FIX NOW HACK for exchange data collection 
//	EventWriteHandleCreatedEvent(handleId, initialObjectId);
//#endif
	return S_OK;
}

//==============================================================================
STDMETHODIMP CallProfiler::HandleDestroyed(GCHandleID handleId)
{
//	if ((m_currentKeywords & (GCHeapKeyword | GCAllocKeyword | GCAllocSampledKeyword)) == 0)
//		return S_OK;
//
//	LOG_TRACE(L"HandleDestroyed\n");
//#ifndef PIN_INVESTIGATION
//	// TODO FIX NOW HACK for exchange data collection 
//	EventWriteHandleDestroyedEvent(handleId);
//#endif 
	return S_OK;
}

STDMETHODIMP CallProfiler::GarbageCollectionStarted(int cGenerations, BOOL generationCollected[], COR_PRF_GC_REASON reason)
{
//	LOG_TRACE(L"GC Started\n");
//	int maxGenCollected = 0;
//	for (int i = 0; i < cGenerations; i++)
//		if (generationCollected[i])
//			maxGenCollected = i;
//
//	m_gcCount++;
//
//#if 0	// TODO FIX NOW implement
//	ULONG boundsCount = 0;
//	COR_PRF_GC_GENERATION_RANGE* bounds = new COR_PRF_GC_GENERATION_RANGE[4];
//	m_info->GetGenerationBounds(4, &boundsCount, bounds);
//	delete bounds;
//#endif
//
//	EventWriteGCStartEvent(m_gcCount, min(maxGenCollected, 2), reason == COR_PRF_GC_INDUCED);
//
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
	/*LOG_TRACE(L"GC End\r\n");
	EventWriteGCStopEvent(m_gcCount);*/
	return S_OK;
}

//==============================================================================
STDMETHODIMP CallProfiler::FinalizeableObjectQueued(DWORD finalizerFlags, ObjectID objectID)
{
//	LOG_TRACE(L"FinalizeableObjectQueued\n");
//#ifndef PIN_INVESTIGATION
//	// TODO FIX NOW HACK for exchange data collection 
//	ClassID classID = 0;
//	m_info->GetClassFromObject(objectID, &classID);
//	EventWriteFinalizeableObjectQueuedEvent(objectID, classID);
//#endif
	return S_OK;
}

STDMETHODIMP CallProfiler::ObjectAllocated(ObjectID objectId, ClassID classId)
{
	return S_OK;
}

EXTERN_C void __stdcall EnterMethod(FunctionID functionID)
{
	// TODO: See if the map already contains functionID, if not - lookup and add
	// TODO: Record (stack) 
	// TODO: Timing info (in,out), need utc long of timer start
	// TOOD: How to pair up in, out? Look at callstack mapping (CLRProfiler)
	// TODO: Arguments (And hooks for checking names)

	if (g_profiler != NULL)
	{

		IMetaDataImport* pIMetaDataImport = 0;
		HRESULT hr = S_OK;
		mdToken funcToken = 0;
		WCHAR szFunction[MAX_CLASS_NAME];
		WCHAR szClass[MAX_CLASS_NAME];

		// get the token for the function which we will use to get its name
		hr = g_profiler->m_info->GetTokenAndMetaDataFromFunction(functionID, IID_IMetaDataImport, (LPUNKNOWN *)&pIMetaDataImport, &funcToken);
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
					printf("EnterMethod: %S.%S\n", szClass, szFunction);
				}
			}
			// release our reference to the metadata
			pIMetaDataImport->Release();
		}
	}
}

EXTERN_C void __stdcall TailcallMethod(FunctionID functionID)
{
	printf("TailcallMethod\n");
//	EnterMethod(functionID);
}

HRESULT CallProfiler::Initialize(IUnknown *pICorProfilerInfoUnk)
{
	//printf("cp::initialize");

	//return S_OK;

	printf("CallProfiler::Initialize\n");

	HRESULT             hr = S_OK;

	// Get current process name -> See if its one we want to profile

	TCHAR szFileName[MAX_PATH + 1];

	DWORD res = GetModuleFileName(NULL, szFileName, MAX_PATH + 1);

	std::wstring name(szFileName);

	printf("ProcessName: %d %S \n" , res, szFileName);

	printf("\n");

	bool wantToProfile = true;

	if(!wantToProfile)
		return S_FALSE;
	

	CALL_N_LOGONBADHR(pICorProfilerInfoUnk->QueryInterface(__uuidof(ICorProfilerInfo3), (void **)&m_info));

	// Even if we did not ask for them, turn on the profiler flags that can ONLY be done at startup. 
	DWORD oldFlags = 0;
	m_info->SetEventMask(COR_PRF_MONITOR_ENTERLEAVE);
	m_info->SetEnterLeaveFunctionHooks(EnterMethodNaked, 0, TailcallMethodNaked);

	g_profiler = this;

	//CALL_N_LOGONBADHR(m_info->GetEventMask(&oldFlags));
	//CALL_N_LOGONBADHR(m_info->SetEventMask(oldFlags | COR_PRF_ENABLE_OBJECT_ALLOCATED));

	// See if we asked for call sampling or not.  
	//DWORD keywords = 0;
	//DWORD keywordsSize = sizeof(keywords);
	//int hrRegGetValue = RegGetValue(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\.NETFramework", L"PerfView_Keywords", RRF_RT_DWORD, NULL, &keywords, &keywordsSize);
	//if (hrRegGetValue == ERROR_SUCCESS)
	//{
	//	if ((keywords & DisableInliningKeyword) != 0)
	//	{
	//		CALL_N_LOGONBADHR(m_info->GetEventMask(&oldFlags));
	//		CALL_N_LOGONBADHR(m_info->SetEventMask(oldFlags | COR_PRF_DISABLE_INLINING));
	//	}

	//	if ((keywords & (CallKeyword | CallSampledKeyword)) != 0)
	//	{
	//		// assert(s_tracer == NULL);	// Don't need any information passed around so I don't need this.  
	//		// s_tracer = this;

	//		// Turn on the Call entry and leave hooks.  
	//		CALL_N_LOGONBADHR(m_info->SetEnterLeaveFunctionHooks3(EnterMethodNaked, 0, TailcallMethodNaked));
	//		CALL_N_LOGONBADHR(m_info->GetEventMask(&oldFlags));
	//		CALL_N_LOGONBADHR(m_info->SetEventMask(oldFlags | COR_PRF_MONITOR_ENTERLEAVE));


	//		if ((keywords & CallSampledKeyword) != 0)
	//			CallSamplingRate = 997;		// TODO make it configurable.    We choose 997 because it is prime and thus likely to be uncorrelated with things.  
	//	}
	//}

	return S_FALSE;
	
	printf("Initialize() returns %x\n" + hr);
	return hr;

exit:
	printf("Initialize() returns %x\n" + hr);
	return hr;
	// In my Initialize() method I call InitializeForAttach with cbClientData == -1.   This is convenient 
	// since most of the logic is the same.  
	//m_profilerLoadedAtStartup = ((int)cbClientData < 0);

	// Initialize the ETW Provider.  
	//LOG_TRACE(L"Registering the ETW provider\n");
	//CALL_N_LOGONBADHR(EventRegisterETWClrProfiler(ProfilerControlCallback, this));

	////ofstream fout("E:\\afile.txt");
	////fout << "Init??????" << endl;
	////fout.close();

	//HRESULT queryInterfaceResult = pICorProfilerInfoUnk->QueryInterface(__uuidof(ICorProfilerInfo8), reinterpret_cast<void **>(&this->corProfilerInfo));

	//if (FAILED(queryInterfaceResult))
	//{
	//	return E_FAIL;
	//}

	//DWORD eventMask = COR_PRF_MONITOR_ENTERLEAVE | COR_PRF_ENABLE_FUNCTION_ARGS | COR_PRF_ENABLE_FUNCTION_RETVAL | COR_PRF_ENABLE_FRAME_INFO;

	//auto hr = this->corProfilerInfo->SetEventMask(eventMask);
	//if (hr != S_OK)
	//{
	//	printf("ERROR: Profiler SetEventMask failed (HRESULT: %d)", hr);
	//}

	//////hr = this->corProfilerInfo->SetEnterLeaveFunctionHooks3WithInfo(EnterNaked, LeaveNaked, TailcallNaked);

	////hr = corProfilerInfo->SetEnterLeaveFunctionHooks2((FunctionEnter2 *)EnterNaked2,
	////	(FunctionLeave2 *)LeaveNaked2,
	////	(FunctionTailcall2 *)TailcallNaked2);

	//if (hr != S_OK)
	//{
	//	printf("ERROR: Profiler SetEnterLeaveFunctionHooks3WithInfo failed (HRESULT: %d)", hr);
	//}

	//return S_OK;



}