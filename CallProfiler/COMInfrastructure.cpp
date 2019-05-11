#include "stdafx.h"

#include <Unknwnbase.h>

#include "CallProfiler.h"

#include <stdio.h>

//[Guid("D1EEF7F2-7F41-4C97-98B1-8E483D0CE3E6")]
const GUID CLSID_CALLPROFILER = { 0xd1eef7f2, 0x7f41, 0x4c97,{ 0x98, 0xb1, 0x8e, 0x48, 0x3d, 0x0c, 0xe3, 0xe6 } };

class CClassFactory : public IClassFactory
{
	public:
		CClassFactory() { m_refCount = 0; }
		ULONG __stdcall AddRef() { return InterlockedIncrement(&m_refCount); }
		ULONG __stdcall Release() {auto ret = InterlockedDecrement(&m_refCount); if (ret <= 0) delete(this); return ret;}

		HRESULT __stdcall QueryInterface(REFIID  riid, void ** ppInterface);
		HRESULT __stdcall  LockServer(BOOL bLock) { return S_OK; }
		HRESULT __stdcall  CreateInstance(IUnknown * pUnkOuter, REFIID riid, void** ppInterface);
private:
	long m_refCount;
};



//=============================================================
//  Dll entry point definition, keeps linker happy. 
//=============================================================
// For VS 2012 they use main()
int main()
{
	return 0;
}

// For VS 2010 they use DllMain
//BOOL WINAPI DllMain(
//	HINSTANCE   hInstance,
//	DWORD       dwReason,
//	LPVOID      lpReserved)
//{
//	switch (dwReason)
//	{
//	case DLL_PROCESS_ATTACH:
//		DisableThreadLibraryCalls(hInstance);     // Don't need the thread callbacks.  
//		break;
//	}
//	return TRUE;
//}




//=============================================================
STDAPI DllGetClassObject(
	REFCLSID    rclsid,
	REFIID      riid,
	LPVOID FAR  * ppv)
{
	HRESULT hr = E_FAIL;
	if (rclsid == CLSID_CALLPROFILER)
	{
		// Create class factory.
		printf("COM::DllGetClassObject\n");
		CClassFactory* pClassFactory = new CClassFactory;
		if (pClassFactory == NULL)
			return E_OUTOFMEMORY;
		hr = pClassFactory->QueryInterface(riid, ppv);
	}
	return (hr);
}

//=============================================================
STDAPI DllCanUnloadNow(void)
{
	return S_OK;
}

//=============================================================
//  Class factory definition
//=============================================================

HRESULT CClassFactory::QueryInterface(
	REFIID  riid,
	void ** ppInterface)
{
	if (IID_IUnknown == riid)
	{
		printf("COM::QueryInterface::IUNKnown\n");
		*ppInterface = static_cast<IUnknown *> (this);
	}
	else if (IID_IClassFactory == riid)
	{
		printf("COM::QueryInterface::IIDClassFactory\n");
		*ppInterface = static_cast<IClassFactory *> (this);
	}
	else
	{
		printf("COM::QueryInterface::Returning null");
		*ppInterface = NULL;
		return (E_NOTIMPL);
	}
	printf("COM::QueryInterface::Returning S(OK)\n");
	reinterpret_cast<IUnknown *>(*ppInterface)->AddRef();
	return (S_OK);
}

HRESULT CClassFactory::CreateInstance(
	IUnknown * pUnkOuter,
	REFIID     riid,
	void **    ppInterface)
{
	if (NULL != pUnkOuter)
		return (CLASS_E_NOAGGREGATION);

	printf("CClassFactory::CreateInstance\n");
	CallProfiler *pProfilerCallback = new CallProfiler();
	if (pProfilerCallback == NULL)
		return E_OUTOFMEMORY;
	return pProfilerCallback->QueryInterface(riid, ppInterface);
}