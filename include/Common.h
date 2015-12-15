/*
 * Common.h
 *
 *  Created on: Oct 31, 2013
 *      Author: lonrust
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <iostream>
#include <string>
#include <stdio.h>
#include <time.h>
#include <vector>
#include <algorithm>


	const std::string currentDateTime();
	std::string indent(int level = 0, bool timestamp = true, char indentChar = '\t');

	double perplexity(double sum, int instances, int numberOfOOV);

	std::vector<std::string> split(std::string const &input);
	std::vector<std::string> tokenizer(const std::string& p_pcstStr, char delim);
	inline std::string trim(const std::string &s)
	{  
		auto  wsfront=std::find_if_not(s.begin(),s.end(),[](int c){return std::isspace(c);});
		return std::string(wsfront,std::find_if_not(s.rbegin(),std::string::const_reverse_iterator(wsfront),[](int c){return std::isspace(c);}).base());
	}



#endif /* COMMON_H_ */
