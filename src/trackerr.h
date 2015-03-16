#pragma once

#include "type.h"
#include <stdexcept>
#include <string>
#include <vector>
#include <sstream>
#include <boost/format.hpp>


#define throw_trackerr(TA1_) \
	throw global::trackerr(DEBUG_INFORMATION_ARGS, TA1_)
#define throw_trackerr2(TA1_, TA2_) \
	throw global::trackerr(DEBUG_INFORMATION_ARGS, TA1_, TA2_)

#define throw_trackerr_str(TA1_) \
	throw_trackerr(boost::str(TA1_))
#define throw_trackerr_str2(TA1_, TA2_) \
	throw_trackerr(boost::str(TA1_, TA2_))


//////////////////////////////////////////////////////////////////////////
#define IGNORE_THROW_ALL(statement_, except_statement_) {try{statement_;}catch(...){except_statement_;}}
#define IGNORE_THROW_QUITE(statement_)	IGNORE_THROW_ALL(statement_, ;)
#define IGNORE_THROW(statement_)	IGNORE_THROW_ALL(statement_, LErr0)

namespace stacktrace 
{	
	struct entry 
	{		
		entry () : line(0) {}

		std::string file;
		size_t      line;
		std::string function;

		std::string to_string () const 
		{
			std::ostringstream os;
			os << file << " (" << line << "): " << function;
			return os.str();
		}
	};

	class call_stack 
	{
	public:
		call_stack (const size_t num_discard = 0);
		virtual ~call_stack () throw();

		std::string to_string () const 
		{
			std::ostringstream os;
			for (size_t i = 0; i < stack.size(); i++)
				os << stack[i].to_string() << "; ";
			return os.str();
		}

		std::vector<entry> stack;
	};

} // namespace stacktrace

namespace global
{
	struct trackerr:
		public std::exception
	{
		explicit trackerr(DEBUG_INFORMATION_ARGS_FUNCDEFINE,
			std::string const& msg_,
			int64 error_code_ = 0)
			: DEBUG_INFORMATION_ARGS_INITIAL
			, error_code(error_code_)
		{
#if (defined(WIN32) && defined(NDEBUG))
			msg = boost::str(boost::format("%s %s@[%s]: %s; error_code=%d") % SRC_PRETTY_FUNCTION % SRC_FILE % SRC_LINE % msg_ % error_code_);	
#else
			stacktrace::call_stack st;
			msg = boost::str(boost::format("%s %s@[%s]: %s; error_code=%d; stacktrace: {%s}") % SRC_PRETTY_FUNCTION % SRC_FILE % SRC_LINE % msg_ % error_code_ % st.to_string());	
#endif
		}		

		explicit trackerr(std::string const& msg_, int64 error_code_ = 0)
			: msg(msg_)
			, error_code(error_code_)
		{
		}
		
		explicit trackerr(int64 error_code_)
			: error_code(error_code_)
		{
		}

		virtual ~trackerr() throw ()
		{
		}

		virtual char_string what() const throw ()
		{
			return msg.c_str();
		}

		virtual int64 get_error_code() const
		{
			return error_code;
		}

		std::string SRC_PRETTY_FUNCTION; 
		std::string SRC_FILE; 
		uint32 SRC_LINE;

		std::string msg;
		int64 error_code;

	};

}

