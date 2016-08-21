/*
 * Apodiformes.cpp
 *
 *  Created on: Oct 24, 2013
 *      Author: lonrust
 */

#include <iostream>

#include <string>
#include <iostream>
#include <fstream>

#include <math.h>

#include <classencoder.h>
#include <classdecoder.h>
#include <patternmodel.h>

#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string/join.hpp>

#include <map>

#include "KneserNey.h"
#include "File.h"

#include <glog/logging.h>
#include <fstream>

#include "Common.h"
#include "CommandLineOptions.h"
#include "Writers.h"

#include <sstream>
#include <iomanip>
#include <stdlib.h>



int main(int argc, char** argv)
{
        google::InitGoogleLogging(argv[0]);

	std::cout << "STRAK" << std::endl;

        CommandLineOptions clo(argc, argv);
	PatternModelOptions options = clo.getPatternModelOptions();
	return 42;
}

