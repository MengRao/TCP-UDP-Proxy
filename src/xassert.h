#pragma once

#include "type.h"
#include "trackerr.h"
#ifdef WIN32
#include <windows.h>
#else
#include <errno.h> 
#endif

#ifdef WIN32
#define XASSERT(_Expression) if(!(_Expression))throw_trackerr2(#_Expression, GetLastError());
#else
#define XASSERT(_Expression) if(!(_Expression))throw_trackerr2(#_Expression, errno);
#endif

#define XEXPECT(_Expression, _ErrorCode) if(!(_Expression))throw (int)_ErrorCode;

