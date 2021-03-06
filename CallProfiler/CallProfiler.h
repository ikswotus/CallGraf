#pragma once


#ifndef __CALL_PROFILER_H__
#define __CALL_PROFILER_H__

#include "ProfilerImpl.h"
#include "FunctionInfo.h"

#include <stdio.h>
#include <time.h>
#include <map>
#include <thread>
#include <vector>
#include <mutex>

#include <atomic>

#define PROFILER_GUID "{D1EEF7F2-7F41-4C97-98B1-8E483D0CE3E6}"
#define PROFILER_GUID_WCHAR L"{D1EEF7F2-7F41-4C97-98B1-8E483D0CE3E6}"

#define COCLASS_DESCRIPTION "CallProfiler"


class CallProfiler
	: public ICorProfilerCallback3
{
private:

public:
	CallProfiler();
	~CallProfiler();

	ThreadID GetThreadID();

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

	static UINT_PTR _stdcall FunctionMapper(FunctionID functionId, BOOL *pbHookFunction);


	STDMETHODIMP_(ULONG) AddRef()
	{
		//printf("AddRef %ld\n", m_refCount);
		return InterlockedIncrement(&m_refCount);
	}
	STDMETHODIMP_(ULONG) Release() 
	{
		//printf("Release - %ld\n", m_refCount);
		auto ret = InterlockedDecrement(&m_refCount); if (ret <= 0) delete(this); return ret;
	}
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

	// Function Mapping - Static because they are callbacks
	static void Enter(FunctionID functionID, UINT_PTR clientData, COR_PRF_FRAME_INFO frameInfo, COR_PRF_FUNCTION_ARGUMENT_INFO *retvalRange);
	static void Leave(FunctionID functionID);
	static void Tailcall(FunctionID functionID);
	
	void EnterHandler(FunctionID functionID);
	void EnterHandler(FunctionID functionID, UINT_PTR clientData, COR_PRF_FRAME_INFO frameInfo, COR_PRF_FUNCTION_ARGUMENT_INFO *retvalRange);

	void LeaveHandler(FunctionID functionID);
	void TailHandler(FunctionID functionID);


	// Map function id => name
	void AddFunction(FunctionID functionID);

	struct ICorProfilerInfo3* m_info;

private:
	// Handle flushing queued function timing events to disk
	void WriteLogsToDisk();
	void Flush();

private :
	
	LONG m_refCount;
	int m_callStackSize;


	// Flag to stop -- needs to be declared above writeThread
	std::atomic<bool> m_stop;

	/** Start time of the application (Seconds since epoch) */
	time_t m_startTime;

	// Perform mapping of function IDs to names
	std::map<FunctionID, CFunctionInfo*> m_functionMap;

	// Writes buffered info to disk
	std::thread m_writeThread;

	// Stores method timing info prior to flushing to disk
	std::vector<std::string> m_buffer;

	//Sync access to _buffer
	std::mutex m_sync;


};


#endif //__CALL_PROFILER_H__