#pragma once

#include <string>
#include <sstream>

namespace Utilities
{
	template<typename T>
	std::string NumberToString(T number)
	{
		std::ostringstream oss;		
		oss << number;
		return oss.str();
	}
};