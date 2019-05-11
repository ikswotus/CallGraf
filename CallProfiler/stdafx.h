// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

#define TRACE_LOGGER            Logger :: s_pTraceLogger

#ifndef DEBUG 
#define LOG_TRACE(...)
#define OPEN_LOG(logger, path)
#define CLOSE_LOG(logger)
#else
#define LOG_TRACE(...)          if (TRACE_LOGGER != NULL) { TRACE_LOGGER->Trace(__VA_ARGS__); }

#define OPEN_LOG(logger, path)                                                  \
{                                                                               \
    logger = new Logger(path);                                                  \
}

#define CLOSE_LOG(logger)                                                       \
{                                                                               \
    if (logger != NULL)                                                         \
    {                                                                           \
        logger->Close();                                                        \
        delete(logger);                                                         \
        logger = NULL;                                                          \
    }                                                                           \
}

//=============================================================
//  Logger class declaration
//=============================================================
class Logger
{
public:
	Logger(wchar_t * logFileName);
	~Logger();

	void                                Open();
	void                                Close();
	void                                Flush();

	void __cdecl                        Trace(wchar_t * format, ...);
	void                                TraceString(wchar_t * lpsz);

	void                                Print();

	static Logger * s_pTraceLogger;
	static Logger * s_pErrorLogger;

private:
	struct _iobuf * /* FILE */ m_logFileStream;
};
#endif // DEBUG

// TODO: reference additional headers your program requires here

#define LOGHR(CALL)                                                             \
{                                                                               \
    LOG_TRACE(L"Error code: [HR]    %d[0x%x]\n", hr, hr   );                    \
    LOG_TRACE(L"    @ " L#CALL L"\n");                                          \
    goto exit;                                                                  \
}

#define LOGEC(ec, CALL)                                                         \
{                                                                               \
    LPVOID lpMsgBuf;                                                            \
    FormatMessage(                                                              \
        FORMAT_MESSAGE_ALLOCATE_BUFFER |                                        \
        FORMAT_MESSAGE_FROM_SYSTEM |                                            \
        FORMAT_MESSAGE_IGNORE_INSERTS,                                          \
        NULL,                                                                   \
        ec,                                                                     \
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),                              \
        (LPTSTR) &lpMsgBuf,                                                     \
        0,                                                                      \
        NULL );                                                                 \
    LOG_TRACE(L"Error code: [EC]    %u : %s\n", ec, lpMsgBuf);                  \
    LocalFree(lpMsgBuf);                                                        \
    hr = HRESULT_FROM_WIN32(ec);                                                \
    LOGHR(CALL);                                                                \
}

#define CALL_N_LOGHRONCOND( CALL, COND )                                        \
{                                                                               \
    hr = CALL;                                                                  \
    if (COND)                                                                   \
    {                                                                           \
        LOGHR(CALL);                                                            \
    }                                                                           \
}

#define CALL_N_LOGECONCOND( CALL, COND )                                        \
{                                                                               \
    DWORD ec = CALL;                                                            \
    if (COND)                                                                   \
    {                                                                           \
        LOGEC(ec, CALL);                                                        \
    }                                                                           \
}

#define CALL_N_LOGLASTECONCOND( CALL, COND )                                    \
{                                                                               \
    if ((CALL) COND)                                                            \
    {                                                                           \
        DWORD ec = GetLastError();                                              \
        LOGEC(ec, CALL);                                                        \
    }                                                                           \
}

#define CALL_N_LOGONBADHR( CALL )       CALL_N_LOGHRONCOND(     CALL, FAILED(hr) )
#define CALL_N_LOGONNOTOK( CALL )       CALL_N_LOGHRONCOND(     CALL, hr != S_OK )
#define CALL_N_LOGONBADEC( CALL )       CALL_N_LOGECONCOND(     CALL, ec != ERROR_SUCCESS )
#define CALL_N_LOGONNULL( CALL )        CALL_N_LOGLASTECONCOND( CALL, == NULL )

