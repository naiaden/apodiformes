/*
 * KneserNey.cpp
 *
 *  Created on: Oct 28, 2013
 *      Author: lonrust
 */

#include "KneserNey.h"

#include <boost/foreach.hpp>

#include "Common.h"
#include <algorithm>

#include <glog/logging.h>

constexpr double KneserNey::epsilon;

KneserNey::KneserNey(IndexedPatternModel<>* patternModel, ClassDecoder* classDecoder, Modification algorithm )
                : KneserNey(3, patternModel, classDecoder, algorithm)
{
}

KneserNey::KneserNey(int order, IndexedPatternModel<>* patternModel, ClassDecoder* classDecoder, Modification algorithm )
                : VectorSpaceModel(patternModel, classDecoder), algorithm(algorithm), order(order)
                , n(order+1), n1(0), n2(0), n3(0), n4(0), tokens(0)
                , Y(0),  D1(0), D2(0), D3plus(0)
{
    std::cout << "Creating KN with ORDER:" << order << std::endl;
    if(order >= 1)
    {
        bra = new KneserNey(order-1, patternModel, classDecoder, algorithm);
    }
}


void KneserNey::doSomething(int indentation)
{
        int ctr = 0;

        for (auto& iter : *patternModel) 
        {
            ++ctr;

            if (ctr > 20) break;


            Pattern pattern = iter.first;


            if(pattern.size() == n)
            {

                
                std::cout << pattern.tostring(*classDecoder) << std::endl;

                std::cout << "pkn: " << pkn(pattern) << std::endl;
            }

        }
    
}

double KneserNey::pkn(const Pattern& pattern, int indentation)
{
    return pkn(Pattern(pattern, order, 1), Pattern(pattern, 0, order), indentation);
}

double KneserNey::pkn(const Pattern& word, const Pattern& history, int indentation)
{
    std::cout << indent(indentation) << "<" << n << "> Computing for word[" << word.tostring(*classDecoder)
                                     << "] and history[" << history.tostring(*classDecoder) << "]" << std::endl;

    Pattern pattern = history+word;

    if(n == 1) //unigram
    {
        return 0.5;
    }

    int count = patternModel->occurrencecount(pattern);
    int marginalCount = std::get<3>(m[pattern]);
    double p1 = 1.0*(count - D(count))/marginalCount;

    double p2 = gamma(history)*bra->pkn(word, Pattern(history, 1, order-1), indentation);
    return p1+p2;
}

double KneserNey::gamma(const Pattern& pattern)
{
    std::tuple<int, int, int, int> NValues = m[pattern];
    double p1 = D1 * std::get<0>(NValues) + D2 * std::get<1>(NValues) + D3plus * std::get<2>(NValues);
    return std::max(0.0, p1/std::get<3>(NValues));
}   

/**
 * Discount
 */
double KneserNey::D(int c)
{
    if(c < 0) 
    {
        LOG(FATAL) << "c cannot be smaller than 0";
    }
    switch(c)
    {
        case 0:
            return 0;
        case 1:
            return D1;
        case 2:
            return D2;
        default:
            return D3plus;
    }
}


void KneserNey::recursiveComputeFrequencyStats(int indentation)
{
    computeFrequencyStats(indentation);
    if(order > 1)
    {
        bra->recursiveComputeFrequencyStats(indentation);
    }
}

/**
 * n1 is the number of n-grams that appear exactly one, n2 is ...
 */
void KneserNey::computeFrequencyStats(int indentation)
{
	LOG(INFO) << indent(indentation++) << "+ Entering computeFrequencyStats";

	int nulls = 0;

	int total = 0;

	//IndexedPatternModel<>& ipm = getPatternModel();
        for (auto& iter : *patternModel) 
        {
            ++total;
            Pattern pattern = iter.first;

            if(pattern.size() == order) 
            {

                int value = patternModel->occurrencecount(pattern);
                if (value < 0)
                {
                        std::cerr << "Unvalid occurence count value " << value << std::endl;
                }

                tokens += value;

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
                        case 3:
                                ++n3;
                                break;
                        case 4:
                                ++n4;
                                break;
                        default:
                                break;
                }
            }
        }

        Y = n1/(n1+2*n2);
        D1 = 1- 2*Y*(n2/n1);
        D2 = 2- 3*Y*(n3/n2);
        D3plus = 3- 4*Y*(n4/n3);
    
//	std::cout << std::endl;

	std::cout << indent(indentation+1) << "Order: " << order << " [" << total << "] 1:" << n1 << " 2:" << n2 << " 3:" << n3 << " 4:" << n4 << std::endl;
        std::cout << indent(indentation+1) << "Order: " << order << " Y: " << Y << " D1: " << D1 << " D2: " << D2 << " D3+: " << D3plus << std::endl;
	LOG(INFO) << indent(--indentation) << "- Leaving computeFrequencyStats";
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
		//ClassDecoder* decoder = docItr->getClassDecoder();
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
 * af
 */
double KneserNey::rawProbability(const Pattern& pattern, int indentation)
{
	std::cout << indent(indentation) << ">" << indentation << " Computing raw probability" << std::endl;

	double pCount = patternCount(pattern);
	double rValue = 0.0;

//	if (pattern.n() == 1)
//	{
//		rValue = pCount / tokens;
//		std::cout << indent(indentation+1) << "pCount(" << pCount << ") and tokens("
//		        << tokens << ")" << std::endl;
//	} else
//	{
		rValue = (pCount - D(pCount)) / tokens;
		std::cout << indent(indentation+1) << "pCount(" << pCount << "), discount(" << D(pCount)
		        << "), and tokens(" << tokens << ")" << std::endl;
//	}

	std::cout << indent(indentation) << "<" << indentation << " raw probability = " << rValue << std::endl;

	return std::max(rValue, epsilon);
}

void KneserNey::recursiveComputeAllN(int indentation)
{
    computeAllN();
    if(order > 1)
    {
        bra->recursiveComputeAllN();
    }
}

void KneserNey::computeAllN(int indentation)
{
    std::cout << indent(indentation) << "Computing N values for order " << order << std::endl;

        int N1 = 0;
        int N2 = 0;
        int N3plus = 0;
        int marginalCount = 0;

    int ctr = 0;
    for( auto& iter: *patternModel )
    {
        ctr++;
        if(ctr>15) break;
        
        if(iter.first.size() == order)
        {
            N1 = 0; N2 = 0; N3plus = 0; marginalCount = 0;
            N(iter.first, N1, N2, N3plus, marginalCount);
            m[iter.first] = std::tuple<int, int, int, int>(  N1, N2, N3plus, marginalCount);
        }
    }
    
//    std::cout << indent(indentation+1) << "<" << order << "> N1: " << N1

//    ctr = 0;
//    for(auto& iter: m)
//    {
//        ctr++;
//        if(ctr>15) break;
//        std::cout << iter.first.tostring(*classDecoder) << std::endl;
//    }
}

/**
 * pattern is at least length 2, because it will take length-1 as beginning, and the last 1 as the wildcard
 */
double KneserNey::N(const Pattern& pattern, int& N1, int& N2, int& N3plus, int& marginalCount)
{
	int patternLength = pattern.n();

	Pattern newPattern = Pattern(pattern, 0, patternLength - 1);

        for(auto& iter : *patternModel)
	{
		const Pattern patternFromIndex = iter.first;
		const IndexedData data = iter.second;

		if (patternFromIndex.n() == patternLength && newPattern == Pattern(patternFromIndex, 0, patternLength - 1))
		{
			int frequency = patternModel->occurrencecount(patternFromIndex);
                        marginalCount += frequency;

			switch (frequency)
			{
				case 0:
					break;
				case 1:
					N1 += frequency;
					break;
				case 2:
					N2 += frequency;
					break;
				default:
					N3plus += frequency;
					break;
			}
		}

	}
}

