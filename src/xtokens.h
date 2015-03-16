#pragma once

#include "type.h"

#include <string>
#include <algorithm>
#include <iterator>
#include <boost/tokenizer.hpp>

namespace global
{
	typedef std::vector<std::string> strings_t;

	inline void separate_tokens(std::string const& wholestr, strings_t& tokens, std::string const& sepchars)
	{
		boost::char_separator<char> sep(sepchars.c_str());
		boost::tokenizer<boost::char_separator<char> > tok(wholestr, sep);
		tokens.clear();
		std::copy(tok.begin(), tok.end(), std::back_inserter(tokens));
	}

	inline std::string assemble_tokens(strings_t const& tokens, std::string const& sepchars)
	{
		if (tokens.size() > 0)
		{
			std::string str(tokens[0]);
			for (size_t i = 1; i < tokens.size(); ++ i)
			{
				str += sepchars + tokens[i];
			}
			return str;
		}
		else
		{
			return std::string();
		}
	}

	inline bool parse_configstring(std::string const& wholestr, std::string& leader, strings_t& tokens)
	{
		separate_tokens(wholestr, tokens, " \t");

		if (tokens.size() >= 1)
		{
			strings_t TK;
			separate_tokens(tokens[0], TK, "=");

			if (TK.size() == 2)
			{
				leader = TK[0];
				tokens[0] = TK[1];

				for (size_t i = 0; i < tokens.size(); ++ i)
				{
					if (tokens[i].length() > 0 && tokens[i][0] == '#')
					{
						tokens.resize(i);
						break;
					}
				}

				return true;

			}
			else
			{
				leader.clear();
				tokens.clear();

				return false;
			}

		}
		else
		{
			leader.clear();
			tokens.clear();

			return false;
		}

	}

	inline std::string assemble_configstring(std::string const& leader, strings_t const& tokens)
	{
		return leader + std::string("=") + assemble_tokens(tokens, " ");
	}

}
