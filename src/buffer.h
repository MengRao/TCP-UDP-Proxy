#pragma once

#include "type.h"
#include <cstdlib>
#include <boost/shared_array.hpp>

namespace global
{

	class Buffer
	{
	public:
		Buffer()
			: buf()
			, buf_size(0)
		{
		}

		Buffer(int buf_size_)
			: buf_size(buf_size_)
		{
			buf = boost::shared_array<BYTE>(new BYTE[buf_size]);			
		}	

		Buffer(boost::shared_array<BYTE> buf_, int buf_size_)
			: buf(buf_)
			, buf_size(buf_size_)
		{
		}

		Buffer(const BYTE* buf_, int buf_size_)
			: buf_size(buf_size_)
		{
			buf = boost::shared_array<BYTE>(new BYTE[buf_size]);
			::memcpy(buf.get(), buf_, buf_size);
		}

		~Buffer()
		{
		}

		void trace_str(std::ostream& os) const
		{
			os << "buf_size=" << buf_size;
		}		

		// dangerous!!
		void resize(int new_size)
		{
			buf_size = new_size;
		}

		void reset(int buf_size_ = 0)
		{
			if (buf_size_ > 0)
			{
				buf_size = buf_size_;
				buf = boost::shared_array<BYTE>(new BYTE[buf_size]);
			}
			else
			{
				buf_size = 0;
				buf = boost::shared_array<BYTE>();
			}
		}		

		boost::shared_array<BYTE> get_sbuf() const
		{
			return buf;
		}

		BYTE* get() const
		{
			return buf.get();
		}

		BYTE* get_buf() const
		{
			return buf.get();
		}

		int get_size() const
		{
			return buf_size;
		}	

		operator bool() const
		{
			return (bool)buf;
		}		

	private:

		boost::shared_array<BYTE> buf;

		int buf_size;
	};

}

