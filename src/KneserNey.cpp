/*
 * KneserNey.cpp
 *
 *  Created on: Oct 28, 2013
 *      Author: lonrust
 */

#include "KneserNey.h"

KneserNey::KneserNey(const IndexedPatternModel<>& patternModel)
		: VectorSpaceModel(patternModel)
{
	// TODO Auto-generated constructor stub

}

KneserNey::~KneserNey()
{
	// TODO Auto-generated destructor stub
}

/**
 * P(w_i | w_{i-n+1}^i-1)
 */
double KneserNey::wordChanceForOrder(const Pattern& pattern, int order)
{

}

/**
 * c(w_{i-n+1}^i)
 * How often does the pattern occur?
 */
int KneserNey::patternCount(const Pattern& pattern)
{
	int frequency = 0;

	for (VectorSpaceModel::documentItr docItr = begin(); docItr != end(); ++docItr)
	{
		boost::shared_ptr<ClassDecoder> decoder = docItr->getClassDecoder();
		for (Document::featureItr featItr = docItr->begin(); featItr != docItr->end(); ++featItr)
		{
			if (featItr->first == pattern)
			{
				frequency += featItr->second;
			}
		}
	}

	return frequency;
}

/**
 * n1 is the number of n-grams that appear exactly one, n2 is ...
 */
void KneserNey::computeFrequencyStats()
{
	int nulls = 0;

	for (IndexedPatternModel<>::iterator iter = getPatternModel().begin(); iter != getPatternModel().end(); iter++)
	{
		unsigned char* data = iter->first.data;
		if (data == NULL)
		{
			std::cout << "data is NULL" << std::endl;
			++nulls;
		}
		else
		{
//		iter->first.out();

			Pattern pattern = iter->first;

			int value = getPatternModel().occurrencecount(pattern);

			if (value < 0)
			{
				std::cerr << "Unvalid occurence count value " << value << std::endl;
			}

			switch (value)
			{
				case 0:
					break;
				case 1:
					++n1;
					break;
				case 2:
					++n2;
					break;
				default:
					++n3;
					break;
			}
		}
	}

	std::cout << "1:" << n1 << " 2:" << n2 << " 3+:" << n3 << "(" << nulls << ")" << std::endl;
}

double KneserNey::y()
{
	return n1 / (n1 + 2 * n2);
}

double KneserNey::discount(int count)
{
	if (count < 0)
	{
		std::cerr << "Unvalid count value " << count << std::endl;
	}

	switch (count)
	{
		case 0:
			return 0;
		case 1:
			return 1 - 2 * y() * (n2 / n1);
		case 2:
			return 2 - 3 * y() * (n2 / n1);
		default:
			return 3 - 4 * y() * (n2 / n1);
	}
}
