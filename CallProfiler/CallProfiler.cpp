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


CallProfiler::CallProfiler() //: corProfilerInfo(NULL)
{
	printf("cpt");
	m_info = NULL;
}

CallProfiler::~CallProfiler()
{
	if (m_info != NULL)
		m_info->Release();
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
	printf("cp::initializeforattach");
	return hr;
}

HRESULT CallProfiler::QueryInterface(
	REFIID  riid,
	void ** ppInterface)
{
	printf("cp::QueryInterface");
	if (riid == IID_IUnknown)
	{
		printf("--IID_IUNKNOWN");
		*ppInterface = static_cast<ICorProfilerCallback*>(this);

	}
	else if (riid == IID_ICorProfilerCallback)
	{
		printf("--IID_ICorProfilerCallback");
		*ppInterface = static_cast<ICorProfilerCallback*>(this);
	}
	else if (riid == IID_ICorProfilerCallback2)
	{
		printf("--IID_ICorProfilerCallback2");
		*ppInterface = static_cast<ICorProfilerCallback2*>(this);
	}
	else if (riid == IID_ICorProfilerCallback3)
	{
		printf("--IID_ICorProfilerCallback3");
		*ppInterface = static_cast<ICorProfilerCallback3*>(this);
	}
	// TODO FIX NOW add support or ICorProfilerCallback4 (for large objects)
	else
	{
		printf("--IID_NOTIMPL");
		*ppInterface = NULL;
		return E_NOTIMPL;
	}
	reinterpret_cast<IUnknown *>(*ppInterface)->AddRef();
	printf("--returning s_OK");
	return S_OK;
}

HRESULT CallProfiler::Shutdown()
{
//	printf("Shutdown");
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

HRESULT CallProfiler::Initialize(IUnknown *pICorProfilerInfoUnk)
{
	printf("cp::initialize");

	return S_OK;

	//printf("Initialized???");


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