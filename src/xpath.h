#pragma once

#include "xtokens.h"
#include <string>

namespace global
{
#ifdef WIN32
	const char PATH_SEPARATOR = '\\';
#else
	const char PATH_SEPARATOR = '/';
#endif

	inline std::string auto_standardize_dirname(std::string const& dirname_)
	{	//	保证最后一个字符为 PATH_SEPARATOR 以方便连接路径
		if (dirname_.size() == 0)
			return std::string(".") + PATH_SEPARATOR;
		else
		{
			if (dirname_[dirname_.size() - 1] != PATH_SEPARATOR)
				return dirname_ + PATH_SEPARATOR;
			else
				return dirname_;
		}
	}

	inline std::string auto_concate_pathname(std::string const& pathname_, std::string const& workdir_)
	{	//	如果 pathname_ 不是绝对路径，则在其前面增加 workdir_ 使其成为绝对路径
		if (pathname_.size() == 0)
			return std::string("");
		else
		{
			if (pathname_[0] == PATH_SEPARATOR
#ifdef WIN32
				|| (pathname_.size() >=2 && pathname_[1] == ':')
#endif
				)
				return pathname_;
			else
				return auto_standardize_dirname(workdir_) + pathname_;
		}
	}

	inline std::string format_dirname(std::string const& dirname_, std::string const& workdir_)
	{
		return auto_standardize_dirname( auto_concate_pathname(dirname_, workdir_) );
	}

	inline std::string format_filename(std::string const& filename_, std::string const& workdir_)
	{
		return auto_concate_pathname(filename_, workdir_);
	}

	inline std::string separate_filename(std::string const& filename_)
	{
		strings_t strs;
		separate_tokens(filename_, strs, std::string() + PATH_SEPARATOR);
		return strs[ strs.size() - 1 ];
	}
	
}
