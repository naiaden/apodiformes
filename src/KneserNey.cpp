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
    std::cout << "Creating KN with ORDER:" << order << "(n=" << n << ")" <<  std::endl;
    if(order >= 1)
    {
        bra = new KneserNey(order-1, patternModel, classDecoder, algorithm);
    } else
    {
        for(const auto& iter: *patternModel)
        {
            if(iter.first.size() == 1)
            {
                tokens += patternModel->occurrencecount(iter.first);
            }
        }
        std::cout << "UNIGRAM TOKENS: " << tokens << std::endl;
    }

}


void KneserNey::doSomething(int indentation)
{
        int ctr = 0;

        for (const auto& iter : *patternModel) 
        {
            if (ctr++ > 20) break;

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

bool KneserNey::isOOV(const Pattern& pattern, int indentation)
{
    
    return !patternModel->has(Pattern(pattern, order, 1));
}

double KneserNey::pkn(const Pattern& word, const Pattern& history, int indentation)
{
    std::cout << indent(indentation) << "<" << n << "> Computing for word[" << word.tostring(*classDecoder)
                                     << "] and history[" << history.tostring(*classDecoder) << "]" << std::endl;

    Pattern pattern = history+word;

    double rv = 0.0;

    std::unordered_map<Pattern, double>::const_iterator iter = pkn_cache.find(pattern);
    if(iter == pkn_cache.end() )
    {
        if(n == 1) //unigram
        {
            return patternModel->occurrencecount(word)/tokens;
        }

        int count = patternModel->occurrencecount(pattern);
        int marginalCount = std::get<3>(m[pattern]);
        double p1 = 1.0*(count - D(count))/marginalCount;
        std::cout << indent(indentation+1) << "prob: " << p1 << " count: " << count << " marginal count: " << marginalCount << std::endl;

        double p2 = gamma(history)*bra->pkn(word, Pattern(history, 1, order-1), indentation);
        rv = p1+p2;
        pkn_cache[pattern] = rv;
    } else
    {
        std::cout << indent(indentation+1) << "Retrieving " << pattern.tostring(*classDecoder) << " from cache!" << std::endl;
        rv = iter->second;
    }

    return rv;
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
    if(n > 1)
    {
        bra->recursiveComputeFrequencyStats(indentation);
    }
}

/**
 * n1 is the number of n-grams that appear exactly one, n2 is ...
 */
void KneserNey::computeFrequencyStats(int indentation)
{
	std::cout << indent(indentation++) << "+ Entering computeFrequencyStats for n=" << n << std::endl;

	int nulls = 0;
	int total = 0;

        for (const auto& iter : *patternModel) 
        {
            Pattern pattern = iter.first;

            if(pattern.size() == n) 
            {
                ++total;

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
    
	std::cout << indent(indentation+1) << "n: " << n << " [" << total << "] 1:" << n1 << " 2:" << n2 << " 3:" << n3 << " 4:" << n4 << std::endl;
        std::cout << indent(indentation+1) << "n: " << n << " Y: " << Y << " D1: " << D1 << " D2: " << D2 << " D3+: " << D3plus << std::endl;
	LOG(INFO) << indent(--indentation) << "- Leaving computeFrequencyStats";
}

void KneserNey::iterativeComputeAllN(int indentation)
{
/*
    std::cout << indent(indentation) << "+ Collapsed computation of N values" << std::endl;

    std::vector<std::unordered_map<Pattern, std::tuple<int, int, int, int> > > maps;
    for(int i = 1; i <= n; ++i)
    {
        maps.push_back(std::unordered_map<Pattern, std::tuple<int, int, int, int> >());
    }

    for(const auto& iter: *patternModel)
    {
       for(int i = 1; i <= n; ++i)
       {
            

            if(patternFI.n() == i && history == Pattern(patternFI, 0, i-1))
            {

            }
       }
    }

    std::cout << indent(indentation) << "- Collapsed computation of N values" << std::endl;
*/
}

void KneserNey::computeAllN(int indentation)
{
    std::cout << indent(indentation) << "+ Computing N values for n " << n << std::endl;

    int N1 = 0;
    int N2 = 0;
    int N3plus = 0;
    int marginalCount = 0;

    int ctr = 0;
    for(const auto& iter: *patternModel )
    {
        if(iter.first.size() == n)
        {
//            if(ctr < 10) { std::cout << iter.first.tostring(*classDecoder) << std::endl;}

            N1 = 0; N2 = 0; N3plus = 0; marginalCount = 0;
            N(iter.first, N1, N2, N3plus, marginalCount);
            m[iter.first] = std::tuple<int, int, int, int>(  N1, N2, N3plus, marginalCount);
        }
    }
}

double KneserNey::N(const Pattern& pattern, int& N1, int& N2, int& N3plus, int& marginalCount)
{
    Pattern history = Pattern(pattern, 0, n - 1);
    
    int ctr = 0;

    

    for(const auto& iter : *patternModel)
    {
    	const Pattern patternFromIndex = iter.first;
    
    	if (patternFromIndex.n() == n && history == Pattern(patternFromIndex, 0, n - 1))
    	{
            int frequency = patternModel->occurrencecount(patternFromIndex);
            marginalCount += frequency;
            
//             if(ctr < 10) { std::cout << "Patt [" << pattern.tostring(*classDecoder) << "] "
//                                      << "newP [" << history.tostring(*classDecoder) << "] "
//                                      << "Pfin [" << patternFromIndex.tostring(*classDecoder) << "] "
//                                      << "Freq [" << frequency << "]" << std::endl;
//                          }
            
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

void KneserNey::recursiveComputeAllN(int indentation)
{
    computeAllN();
    if(n > 1)
    {
        bra->recursiveComputeAllN();
    }
}

