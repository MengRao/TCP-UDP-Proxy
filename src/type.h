#pragma once

#ifdef WIN32
#	ifndef NOMINMAX
#		define NOMINMAX
#	endif
#	include <winsock2.h>
#	pragma comment(lib, "ws2_32.lib")
#else
#	include <sys/types.h>
#	include <sys/socket.h>
#	include <netinet/in.h>
#	include <arpa/inet.h>
#	include <netdb.h>
#	include <sys/ioctl.h>
#	include <net/if.h>
#	include <errno.h>
#endif

//////////////////////////////////////////////////////////////////////////
////
////	about compiler
////

#define MACRO_STRING_(STR_) #STR_
#define MACRO_STRING(STR_) MACRO_STRING_(STR_)

#ifndef OUT
#define OUT
#endif
#ifndef IN
#define IN
#endif
#ifndef INOUT
#define INOUT
#endif

#define BOOST_ENABLE_ASSERT_HANDLER
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>

#ifdef WIN32
#	if !defined(_WIN32_WINNT)
#		define _WIN32_WINNT 0x0502
#	endif
#	pragma warning (disable: 4996)
#	pragma warning (disable: 4819)
#	pragma warning (disable: 4535)
#	pragma warning (disable: 4267)
#	pragma warning (disable: 4297)
#	pragma warning (disable: 4172)
#	pragma warning (disable: 4355)	//	warning C4355: 'this' : used in base member initializer list
#	define __PRETTY_FUNCTION__ __FUNCTION__
#endif

#define DEBUG_INFORMATION_ARGS			__PRETTY_FUNCTION__, __FILE__, __LINE__
#define DEBUG_INFORMATION_ARGS_FUNCDEFINE	const_char_string SRC_PRETTY_FUNCTION_, const_char_string SRC_FILE_, uint32 SRC_LINE_
#define DEBUG_INFORMATION_ARGS_FUNCUSE	SRC_PRETTY_FUNCTION_, SRC_FILE_, SRC_LINE_

#define DEBUG_INFORMATION_ARGS_INITIAL	SRC_PRETTY_FUNCTION(SRC_PRETTY_FUNCTION_), SRC_FILE(SRC_FILE_), SRC_LINE(SRC_LINE_)

#define DEBUG_INFORMATION_ARGS_DECLARE	const_char_string SRC_PRETTY_FUNCTION; const_char_string SRC_FILE; uint32 const SRC_LINE;
#define DEBUG_INFORMATION_ARGS_USE	SRC_PRETTY_FUNCTION, SRC_FILE, SRC_LINE

////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
////
////	about basic type
////
#ifndef __cplusplus
#	error g++ >=4 or msvc++ >=8 required.
#endif

#if (defined(_WIN32) || defined(_WIN64)) && (_MSC_VER >= 1400)
typedef		__int8				int8;
typedef		__int16				int16;
typedef		__int32				int32;
typedef		__int64				int64;
typedef		unsigned __int8		uint8;
typedef		unsigned __int16	uint16;
typedef		unsigned __int32	uint32;
typedef		unsigned __int64	uint64;
typedef		float				float32;
typedef		double				float64;

#elif (__GNUG__ >= 4) //&& (__GNUC_MINOR__ > 2 || (__GNUC_MINOR__ == 2 && __GNUC_PATCHLEVEL__ >= 2))
//	above g++ 4
#	include "asm/types.h"
typedef		__s8	int8;
typedef		__s16	int16;
typedef		__s32	int32;
typedef		__s64	int64;
typedef		__u8	uint8;
typedef		__u16	uint16;
typedef		__u32	uint32;
typedef		__u64	uint64;
typedef		float	float32;
typedef		double	float64;
#	if (__GNUC_MINOR__ < 2)
#		define __TIMESTAMP__ MACRO_STRING(__TIMESTAMPSTR__)
#	endif
#else
#	error g++ >=4 or msvc++ >=8 required.
#endif

#ifndef NULL
#	define NULL 0
#endif

int8 const int8_max		=	0x07f;
int8 const int8_min		=	-128;//0x080;
int16 const int16_max	=	0x07fff;
int16 const int16_min	=	-32768;//0x08000;
int32 const int32_max	=	0x07fffffff;
int32 const int32_min	=	0x080000000;
int64 const int64_max	=	0x07fffffffffffffffll;
int64 const int64_min	=	0x08000000000000000ll;

uint8 const uint8_max		=	~0;
uint8 const uint8_min		=	0;
uint16 const uint16_max		=	~0;
uint16 const uint16_min		=	0;
uint32 const uint32_max		=	~0;
uint32 const uint32_min		=	0;
uint64 const uint64_max		=	~0;
uint64 const uint64_min		=	0;

typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned char BYTE;

typedef signed int sint;
typedef signed char schar;

typedef int8	sint8;
typedef int16	sint16;
typedef int32	sint32;
typedef int64	sint64;


typedef		char const* const	const_char_string;
typedef		char const*			char_string;

template<typename T>
struct type
{
	static const_char_string name;
	static T const max_v;
	static T const min_v;
};

struct dummy{};
namespace global{}


extern const int NO_IDX;
extern const int NO_POS;

