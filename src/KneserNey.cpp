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
                : KneserNey(4, patternModel, classDecoder, algorithm)
{
}

KneserNey::KneserNey(int order, IndexedPatternModel<>* patternModel, ClassDecoder* classDecoder, Modification algorithm )
                : VectorSpaceModel(patternModel, classDecoder), algorithm(algorithm), order(order)
                , n1(0), n2(0), n3(0), n4(0), tokens(0)
                , Y(0),  D1(0), D2(0), D3plus(0)
{
    std::cout << "Creating KN with ORDER:" << order << std::endl;
    if(order > 0)
    {
        bra = new KneserNey(order-1, patternModel, classDecoder, algorithm);
    }
}

//KneserNey::~KneserNey()
//{
//	// TODO Auto-generated destructor stub
//}

double KneserNey::computeSimularity(const Document& document)
{
	std::cerr << "KneserNey::computeSimularity is unimplemented" << std::endl;
	return 4.0;
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

/**
 * af
 */
double KneserNey::smoothedProbability(const Pattern& pattern, int indentation)
{
	std::cout << indent(indentation) << ">" << indentation << " Computing smoothed probability" << std::endl;

	Pattern smallerPattern = Pattern(pattern, 1, pattern.n() - 1);
	double ipF = interpolationFactor(pattern, indentation + 1);
	double smV = getSmoothedValue(smallerPattern, indentation + 1);
	double rValue = ipF * smV;

	std::cout << indent(indentation+1) << "Interpolation factor(" << ipF << ") smoothed value(" << smV << ")" << std::endl;

	std::cout << indent(indentation) << "<" << indentation << " smoothed probability" << rValue << std::endl;

	return rValue;
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

/**
 * gamma, af
 * N_1(w_{i-n+1}^{i-1} •) = N_1(w_{i-n+1}, w_{i-n+2}, ..., w_{i-2}, •)
 * which is like taking the n-gram, and replacing the last word with a wildcard
 * And then take all the patterns that occur once (hence N_1)
 */
double KneserNey::interpolationFactor(const Pattern& pattern, int indentation)
{
	std::cout << indent(indentation) << ">" << indentation << " Computing interpolation factor" << std::endl;

	int N1 = 0;
	int N2 = 0;
	int N3 = 0;
        int marginalCount = 0;

	N(pattern, N1, N2, N3, marginalCount);

	double term1 = D1 * N1;
	double term2 = D2 * N2;
	double term3 = D3plus * N3;

	double rValue = (term1 + term2 + term3) / tokens;

	std::cout << indent(indentation) << "<" << indentation << " interpolation factor: " << rValue << " with N1(" << N1
	        << ") N2(" << N2 << ") N3(" << N3 << ") and D1(" << D1
	        << ") D2(" << D2 << ") D3+(" << D3plus << ") y(" << Y << ") n1(" << n1 << ") n2(" << n2 << ") n3(" << n3 << ") n4(" << n4 << ")" << std::endl;

	return rValue;
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
//
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

	//IndexedPatternModel<> patternModel = getPatternModel();
//	for (IndexedPatternModel<>::iterator iter = patternModel.begin(); iter != patternModel->end(); iter++)
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

/**
 * af?
 */
double KneserNey::getSmoothedValue(const Pattern& pattern, int indentation)
{
	std::cout << indent(indentation) << "+" << indentation << " Computing getSmoothedValue for pattern ["
	        << pattern.tostring(*classDecoder) << "]" << std::endl;

	int currentNInNgram = pattern.n();
	int nextN = currentNInNgram - 1;

	double rValue = 0.0;

	if (currentNInNgram == 1)
	{
		rValue = rawProbability(pattern, indentation + 1);
		std::cout << indent(indentation) << "<" << indentation << " smoothed value for unigram: " << rValue
		        << std::endl;
	} else
	{

		double rawProb = rawProbability(pattern, indentation + 1);
		double smoProb = smoothedProbability(Pattern(pattern, pattern.n() - nextN - 1, currentNInNgram),
		        indentation + 1);
		rValue = rawProb + smoProb;

		std::cout << indent(indentation) << "-" << indentation << " smoothed value = " << rValue
		        << " with rawProb(" << rawProb << "/" << exp(rawProb) << ") and smoProb(" << smoProb << ")" << std::endl;
	}

	return rValue;
}

