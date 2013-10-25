/*
 * VectorSpaceModel.cpp
 *
 *  Created on: Oct 24, 2013
 *      Author: lonrust
 */

#include "VectorSpaceModel.h"

VectorSpaceModel::VectorSpaceModel() : PatternMap<double>() {
	// TODO Auto-generated constructor stub

}

VectorSpaceModel::~VectorSpaceModel() {
	// TODO Auto-generated destructor stub
}

/**
 * If pattern exists in the vector space model, double returns the old value of pattern
 * otherwise the behaviour is undefined
 */
double VectorSpaceModel::updateValue(const Pattern& pattern, double newValue)
{
	 std::unordered_map<const Pattern,double>::const_iterator iter = data.find (pattern);

	  if ( iter != data.end() )
	  {
		  double oldValue = iter->second;
	     data[pattern] = newValue;

	    return oldValue;
	  }

	return 0;
}

