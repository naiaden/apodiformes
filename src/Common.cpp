/*
 * Common.cpp
 *
 *  Created on: Oct 31, 2013
 *      Author: lonrust
 */

#include "Common.h"

#include <sstream>
#include <iomanip>

#include <iostream>

#include <string>
#include <fstream>

#include <math.h>
#include <algorithm>
#include <boost/algorithm/string/join.hpp>

#include <map>



#include <stdlib.h>


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

double perplexity(double sum, int instances, int numberOfOOV)
{       
        std::cout << "S:" << sum << " #:" << instances << " O:" << numberOfOOV << std::endl;
        double dInstances = (double) (instances-numberOfOOV);
        return pow(10, -1.0/dInstances * sum); // 10^(�~H~R1÷4�~W�~H~R5.80288)
}

std::vector<std::string> split(std::string const &input) {
    std::istringstream buffer(input);
    std::vector<std::string> ret{std::istream_iterator<std::string>(buffer),
                                 std::istream_iterator<std::string>()};
    return ret;
}

    std::vector<std::string> tokenizer( const std::string& p_pcstStr, char delim )  {
        std::vector<std::string> tokens;
        std::stringstream   mySstream( p_pcstStr );
        std::string         temp;

        while( getline( mySstream, temp, delim ) ) {
            tokens.push_back( temp );
        }

        return tokens;
    }

