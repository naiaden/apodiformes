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

KneserNey::KneserNey(KneserNey* kneserNey, int order, IndexedPatternModel<>* patternModel, ClassDecoder* classDecoder, Modification algorithm )
                : classDecoder(classDecoder), patternModel(patternModel), algorithm(algorithm), order(order)
                , n(order+1), n1(0), n2(0), n3(0), n4(0), tokens(0)
                , Y(0),  D1(0), D2(0), D3plus(0)
{
    LOG(INFO) << "Special creation of KN with ORDER:" << order << "(n=" << n << ")";
    bra = kneserNey;

    if(!bra)
    {
        for(const auto& iter: *patternModel)
        {
            if(iter.first.size() == 1)
            {
                tokens += patternModel->occurrencecount(iter.first);
            }
        }
    }

}

KneserNey::KneserNey(int order, IndexedPatternModel<>* patternModel, ClassDecoder* classDecoder, Modification algorithm )
                : classDecoder(classDecoder), patternModel(patternModel), algorithm(algorithm), order(order)
                , n(order+1), n1(0), n2(0), n3(0), n4(0), tokens(0)
                , Y(0),  D1(0), D2(0), D3plus(0)
{
    LOG(INFO) << "Creating KN with ORDER:" << order << "(n=" << n << ")";
    if(order >= 1)
    {
        bra = new KneserNey(order-1, patternModel, classDecoder, algorithm);
    } else
    {
        bra = nullptr;
        for(const auto& iter: *patternModel)
        {
            if(iter.first.size() == 1)
            {
                tokens += patternModel->occurrencecount(iter.first);
            }
        }
        LOG(INFO) << "Found " << tokens << " unigram tokens";
    }

}


void KneserNey::doSomething(int indentation)
{
        std::cout << "m contains " << m->size() << " items" << std::endl;
        for(const auto& iter : *m)
        {
            if(iter.first.size() == 2)
            {
                std::cout << iter.first.tostring(*classDecoder) << std::endl;
                std::tuple<int, int, int, int> asd = iter.second;
                std::cout << "(" << std::get<0>(asd) << "," << std::get<1>(asd) << "," << std::get<2>(asd) << "," << std::get<3>(asd) << ")" << std::endl;
            }
        }
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
        int marginalCount = std::get<3>((*m)[pattern]);
        double p1 = 1.0*(count - D(count))/marginalCount;
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
    std::tuple<int, int, int, int> NValues = (*(bra->m))[pattern];
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
	LOG(INFO) << "+ Entering computeFrequencyStats for n=" << n;

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
                    LOG(FATAL) << "Unvalid occurence count value " << value;
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
    
	LOG(INFO) << "n: " << n << " [" << total << "] 1:" << n1 << " 2:" << n2 << " 3:" << n3 << " 4:" << n4;
        LOG(INFO) << "n: " << n << " Y: " << Y << " D1: " << D1 << " D2: " << D2 << " D3+: " << D3plus;
	LOG(INFO) << "- Leaving computeFrequencyStats";
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
    LOG(INFO) << "+ Computing N values for n " << n;

    int N1 = 0;
    int N2 = 0;
    int N3plus = 0;
    int marginalCount = 0;

    int ctr = 0;
    for(const auto& iter: *patternModel )
    {
        if(iter.first.size() == n)
        {
            //if(ctr++ > 10) break;
//            if(ctr < 10) { std::cout << iter.first.tostring(*classDecoder) << std::endl;}

            N1 = 0; N2 = 0; N3plus = 0; marginalCount = 0;
            N(iter.first, N1, N2, N3plus, marginalCount);

//            std::cout << std::endl << iter.first.tostring(*classDecoder) << std::endl;
//std::cout << "y[" << N1 << "," << N2 << "," << N3plus << "," << marginalCount << "]" << std::endl;                                                                                     


            (*m)[iter.first] = std::tuple<int, int, int, int>(  N1, N2, N3plus, marginalCount);
//std::tuple<int, int, int, int> NValues = (*m)[iter.first];
//std::cout << "G[" << std::get<0>(NValues) << "," << std::get<1>(NValues) << "," << std::get<2>(NValues) << "," << std::get<3>(NValues) << "]" << std::endl;                                                                                     

            gamma(iter.first);

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

