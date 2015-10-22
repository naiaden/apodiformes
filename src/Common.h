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


	const std::string currentDateTime();
	std::string indent(int level = 0, bool timestamp = true, char indentChar = '\t');


#endif /* COMMON_H_ */
