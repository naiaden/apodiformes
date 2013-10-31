/*
 * Common.cpp
 *
 *  Created on: Oct 31, 2013
 *      Author: lonrust
 */

#include "Common.h"

#include <sstream>

std::string indent(int level, bool timestamp, char indentChar)
{
	std::stringstream ss;

	ss << "[" << (timestamp ? currentDateTime() : "") << "] " << std::string(level, indentChar);

	return ss.str();
}

const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);

    return buf;
}
