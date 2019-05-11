#pragma once


#ifndef __CALL_PROFILER_H__
#define __CALL_PROFILER_H__

//#include "mscoree.h"
//#include "avlnode.h"
//#include "basehlp.h"
//#include "container.hpp"
////#include "stdafx.h"
//
//#include <Windows.h>

#include <cor.h>
#include <corprof.h>
#include <corhlpr.h>

//[Guid("D1EEF7F2-7F41-4C97-98B1-8E483D0CE3E6")]
//extern const GUID __declspec(selectany) CLSID_CALLPROFILER =
//{ 0xd1eef7f2, 0x7f41, 0x4c97,{ 0x98, 0xb1, 0x8e, 0x48, 0x3d, 0x0c, 0xe3, 0xe6 } };

#define PROFILER_GUID "{D1EEF7F2-7F41-4C97-98B1-8E483D0CE3E6}"
#define PROFILER_GUID_WCHAR L"{D1EEF7F2-7F41-4C97-98B1-8E483D0CE3E6}"

#define COCLASS_DESCRIPTION "CallProfiler Test"

//#define COM_METHOD( TYPE ) TYPE STDMETHODCALLTYPE

class CallProfiler
	: public ICorProfilerCallback3
{
private:
	//ICorProfilerInfo3* corProfilerInfo;

public:
	CallProfiler();
	~CallProfiler();

	STDMETHODIMP Initialize(IUnknown* pICorProfilerInfoUnk);
	STDMETHODIMP Shutdown();

	HRESULT STDMETHODCALLTYPE InitializeForAttach(
		/* [in] */ IUnknown *pCorProfilerInfoUnk,
		/* [in] */ void *pvClientData,
		/* [in] */ UINT cbClientData);
	HRESULT STDMETHODCALLTYPE ProfilerAttachComplete(void) { return S_OK; };
	HRESULT STDMETHODCALLTYPE ProfilerDetachSucceeded(void)
	{
		//LOG_TRACE(L"ProfilerDetachSucceeded\n");
		return Shutdown();
	}

	STDMETHODIMP_(ULONG) AddRef() { return InterlockedIncrement(&m_refCount); }
	STDMETHODIMP_(ULONG) Release() { auto ret = InterlockedDecrement(&m_refCount); if (ret <= 0) delete(this); return ret; }
	STDMETHODIMP QueryInterface(REFIID riid, void **ppInterface);

	/* OTHER STUFF */
	STDMETHODIMP AppDomainCreationStarted(AppDomainID appDomainId) { return S_OK; };
	STDMETHODIMP AppDomainCreationFinished(AppDomainID appDomainId, HRESULT hrStatus) { return S_OK; };
	STDMETHODIMP AppDomainShutdownStarted(AppDomainID appDomainId) { return S_OK; };
	STDMETHODIMP AppDomainShutdownFinished(AppDomainID appDomainId, HRESULT hrStatus) { return S_OK; };
	STDMETHODIMP AssemblyLoadStarted(AssemblyID assemblyId) { return S_OK; };
	STDMETHODIMP AssemblyLoadFinished(AssemblyID assemblyId, HRESULT hrStatus) { return S_OK; };
	STDMETHODIMP AssemblyUnloadStarted(AssemblyID assemblyId) { return S_OK; };
	STDMETHODIMP AssemblyUnloadFinished(AssemblyID assemblyId, HRESULT hrStatus) { return S_OK; };
	STDMETHODIMP ModuleLoadStarted(ModuleID moduleId) { return S_OK; };
	STDMETHODIMP ModuleLoadFinished(ModuleID moduleId, HRESULT hrStatus) { return S_OK; };
	STDMETHODIMP ModuleUnloadStarted(ModuleID moduleId) { return S_OK; };
	STDMETHODIMP ModuleUnloadFinished(ModuleID moduleId, HRESULT hrStatus) { return S_OK; };
	STDMETHODIMP ModuleAttachedToAssembly(ModuleID moduleId, AssemblyID assemblyId) { return S_OK; };
	STDMETHODIMP ClassLoadStarted(ClassID classId) { return S_OK; };
	STDMETHODIMP ClassLoadFinished(ClassID classId, HRESULT hrStatus) { return S_OK; };
	STDMETHODIMP ClassUnloadStarted(ClassID classId) { return S_OK; };
	STDMETHODIMP ClassUnloadFinished(ClassID classId, HRESULT hrStatus) { return S_OK; };
	STDMETHODIMP FunctionUnloadStarted(FunctionID functionId) { return S_OK; };
	STDMETHODIMP JITCompilationStarted(FunctionID functionId, BOOL fIsSafeToBlock) { return S_OK; };
	STDMETHODIMP JITCompilationFinished(FunctionID functionId, HRESULT hrStatus, BOOL fIsSafeToBlock) { return S_OK; };
	STDMETHODIMP JITCachedFunctionSearchStarted(FunctionID functionId, BOOL * pbUseCachedFunction) { return S_OK; };
	STDMETHODIMP JITCachedFunctionSearchFinished(FunctionID functionId, COR_PRF_JIT_CACHE result) { return S_OK; };
	STDMETHODIMP JITFunctionPitched(FunctionID functionId) { return S_OK; };
	STDMETHODIMP JITInlining(FunctionID callerId, FunctionID calleeId, BOOL * pfShouldInline) { return S_OK; };
	STDMETHODIMP ThreadCreated(ThreadID threadId) { return S_OK; };
	STDMETHODIMP ThreadDestroyed(ThreadID threadId) { return S_OK; };
	STDMETHODIMP ThreadAssignedToOSThread(ThreadID managedThreadId, ULONG osThreadId) { return S_OK; };
	STDMETHODIMP RemotingClientInvocationStarted() { return S_OK; };
	STDMETHODIMP RemotingClientSendingMessage(GUID * pCookie, BOOL fIsAsync) { return S_OK; };
	STDMETHODIMP RemotingClientReceivingReply(GUID * pCookie, BOOL fIsAsync) { return S_OK; };
	STDMETHODIMP RemotingClientInvocationFinished() { return S_OK; };
	STDMETHODIMP RemotingServerReceivingMessage(GUID * pCookie, BOOL fIsAsync) { return S_OK; };
	STDMETHODIMP RemotingServerInvocationStarted() { return S_OK; };
	STDMETHODIMP RemotingServerInvocationReturned() { return S_OK; };
	STDMETHODIMP RemotingServerSendingReply(GUID * pCookie, BOOL fIsAsync) { return S_OK; };
	STDMETHODIMP UnmanagedToManagedTransition(FunctionID functionId, COR_PRF_TRANSITION_REASON reason) { return S_OK; };
	STDMETHODIMP ManagedToUnmanagedTransition(FunctionID functionId, COR_PRF_TRANSITION_REASON reason) { return S_OK; };
	STDMETHODIMP RuntimeSuspendStarted(COR_PRF_SUSPEND_REASON suspendReason) { return S_OK; };
	STDMETHODIMP RuntimeSuspendFinished() { return S_OK; };
	STDMETHODIMP RuntimeSuspendAborted() { return S_OK; };
	STDMETHODIMP RuntimeResumeStarted() { return S_OK; };
	STDMETHODIMP RuntimeResumeFinished() { return S_OK; };
	STDMETHODIMP RuntimeThreadSuspended(ThreadID threadId) { return S_OK; };
	STDMETHODIMP RuntimeThreadResumed(ThreadID threadId) { return S_OK; };
	STDMETHODIMP MovedReferences(ULONG cMovedObjectIDRanges, ObjectID oldObjectIDRangeStart[], ObjectID newObjectIDRangeStart[], ULONG cObjectIDRangeLength[]);
	STDMETHODIMP ObjectAllocated(ObjectID objectId, ClassID classId);
	STDMETHODIMP ObjectsAllocatedByClass(ULONG cClassCount, ClassID classIds[], ULONG cObjects[]) { return S_OK; };
	STDMETHODIMP ObjectReferences(ObjectID objectId, ClassID classId, ULONG cObjectRefs, ObjectID objectRefIds[]);
	STDMETHODIMP RootReferences(ULONG cRootRefs, ObjectID rootRefIds[]) { return S_OK; }
	STDMETHODIMP ExceptionThrown(ObjectID thrownObjectId) { return S_OK; };
	STDMETHODIMP ExceptionSearchFunctionEnter(FunctionID functionId) { return S_OK; };
	STDMETHODIMP ExceptionSearchFunctionLeave() { return S_OK; };
	STDMETHODIMP ExceptionSearchFilterEnter(FunctionID functionId) { return S_OK; };
	STDMETHODIMP ExceptionSearchFilterLeave() { return S_OK; };
	STDMETHODIMP ExceptionSearchCatcherFound(FunctionID functionId) { return S_OK; };
	STDMETHODIMP ExceptionOSHandlerEnter(FunctionID functionId) { return S_OK; };
	STDMETHODIMP ExceptionOSHandlerLeave(FunctionID functionId) { return S_OK; };
	STDMETHODIMP ExceptionUnwindFunctionEnter(FunctionID functionId) { return S_OK; };
	STDMETHODIMP ExceptionUnwindFunctionLeave() { return S_OK; };
	STDMETHODIMP ExceptionUnwindFinallyEnter(FunctionID functionId) { return S_OK; };
	STDMETHODIMP ExceptionUnwindFinallyLeave() { return S_OK; };
	STDMETHODIMP ExceptionCatcherEnter(FunctionID functionId, ObjectID objectId) { return S_OK; };
	STDMETHODIMP ExceptionCatcherLeave() { return S_OK; };
	STDMETHODIMP COMClassicVTableCreated(ClassID wrappedClassId, REFGUID implementedIID, void *pVTable, ULONG cSlots) { return S_OK; };
	STDMETHODIMP COMClassicVTableDestroyed(ClassID wrappedClassId, REFGUID implementedIID, void *pVTable) { return S_OK; };
	STDMETHODIMP ExceptionCLRCatcherFound(void) { return S_OK; };
	STDMETHODIMP ExceptionCLRCatcherExecute(void) { return S_OK; };

	// End of ICorProfilerCallback interface implementation

	// ICorProfilerCallback2 interface implementation

	STDMETHODIMP ThreadNameChanged(ThreadID threadId, ULONG cchName, WCHAR* name) { return S_OK; };
	STDMETHODIMP GarbageCollectionStarted(int cGenerations, BOOL generationCollected[], COR_PRF_GC_REASON reason);
	STDMETHODIMP SurvivingReferences(ULONG cSurvivingObjectIDRanges, ObjectID objectIDRangeStart[], ULONG cObjectIDRangeLength[]);

	STDMETHODIMP GarbageCollectionFinished(void);
	STDMETHODIMP FinalizeableObjectQueued(DWORD finalizerFlags, ObjectID objectID);
	STDMETHODIMP RootReferences2(ULONG cRootRefs, ObjectID rootRefIds[], COR_PRF_GC_ROOT_KIND rootKinds[], COR_PRF_GC_ROOT_FLAGS rootFlags[], UINT_PTR rootIds[]);
	STDMETHODIMP HandleCreated(GCHandleID handleId, ObjectID initialObjectId);
	STDMETHODIMP HandleDestroyed(GCHandleID handleId);
private :
	struct ICorProfilerInfo3* m_info;
	LONG                      m_refCount;
};

//extern CallProfiler *g_pCallbackObject;

#endif //__CALL_PROFILER_H__