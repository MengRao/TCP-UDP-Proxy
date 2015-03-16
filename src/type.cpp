#include "type.h"
#include "trackerr.h"

template<> const_char_string type<bool>::name		=	"bool";
template<> bool const type<bool>::min_v				=	false < true ? false : true;
template<> bool const type<bool>::max_v				=	false < true ? true : false;

template<> const_char_string type<float32>::name	=	"float32";
template<> float32 const type<float32>::min_v		=	0;
template<> float32 const type<float32>::max_v		=	0;

template<> const_char_string type<float64>::name	=	"float64";
template<> float64 const type<float64>::min_v		=	0;
template<> float64 const type<float64>::max_v		=	0;

template<> const_char_string type<int8>::name		=	"int8";
template<> int8 const type<int8>::min_v				=	int8_min;
template<> int8 const type<int8>::max_v				=	int8_max;

template<> const_char_string type<int16>::name		=	"int16";
template<> int16 const type<int16>::min_v			=	int16_min;
template<> int16 const type<int16>::max_v			=	int16_max;

template<> const_char_string type<int32>::name		=	"int32";
template<> int32 const type<int32>::min_v			=	int32_min;
template<> int32 const type<int32>::max_v			=	int32_max;

template<> const_char_string type<int64>::name		=	"int64";
template<> int64 const type<int64>::min_v			=	int64_min;
template<> int64 const type<int64>::max_v			=	int64_max;


template<> const_char_string type<uint8>::name		=	"uint8";
template<> uint8 const type<uint8>::min_v			=	uint8_min;
template<> uint8 const type<uint8>::max_v			=	uint8_max;

template<> const_char_string type<uint16>::name		=	"uint16";
template<> uint16 const type<uint16>::min_v			=	uint16_min;
template<> uint16 const type<uint16>::max_v			=	uint16_max;

template<> const_char_string type<uint32>::name		=	"uint32";
template<> uint32 const type<uint32>::min_v			=	uint32_min;
template<> uint32 const type<uint32>::max_v			=	uint32_max;

template<> const_char_string type<uint64>::name		=	"uint64";
template<> uint64 const type<uint64>::min_v			=	uint64_min;
template<> uint64 const type<uint64>::max_v			=	uint64_max;

const int NO_IDX = -1;
const int NO_POS = -1;

void boost::assertion_failed(char const * expr, char const * function, char const * file, long line)
{
	throw_trackerr("boost::assertion_failed");
}

#ifndef NDEBUG
void boost::assertion_failed_msg(char const * expr, char const * msg,
						  char const * function, char const * file, long line)
{
	throw_trackerr("boost::assertion_failed_msg");
}
#endif
