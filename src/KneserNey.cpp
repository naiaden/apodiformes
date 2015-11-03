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
      //          tokens += patternModel->occurrencecount(iter.first);
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
     //           tokens += patternModel->occurrencecount(iter.first);
            }
        }
        LOG(INFO) << "Found " << tokens << " unigram tokens";
    }

}


void KneserNey::doSomething()
{
//        std::cout << "m contains " << m->size() << " items" << std::endl;
//        int ctr1 = 0;
//        for(const auto& iter : *m)
//        {
//            if(ctr1++ > 10) break;
//            
//                std::cout << iter.first.tostring(*classDecoder) << std::endl;
//                std::tuple<int, int, int, int> asd = iter.second;
//                std::cout << "(" << std::get<0>(asd) << "," << std::get<1>(asd) << "," << std::get<2>(asd) << "," << std::get<3>(asd) << ")" << std::endl;
//            
//        }

//        for (const auto& iter : *patternModel) 
//        {
//            Pattern pattern = iter.first;
//            if(pattern.size() == n)
//            {
//                std::cout << "[" << pkn(pattern) << "]\t" << pattern.tostring(*classDecoder)  << std::endl;
//            }
//        }
}

double KneserNey::pkn(const Pattern& pattern, bool debug)
{
    if(debug)
    {
        std::cout << "[" << n << "] Main pkn: " << pattern.tostring(*classDecoder) << std::endl;
    }

    return pkn(Pattern(pattern, order, 1), Pattern(pattern, 0, order), debug);
}

bool KneserNey::isOOVWord(const Pattern& word)
{
    return !patternModel->has(word);
}

bool KneserNey::isOOV(const Pattern& pattern)
{
    
    return !patternModel->has(Pattern(pattern, order, 1));
}

double KneserNey::pkn(const Pattern& word, const Pattern& history, bool debug)
{
    Pattern pattern = history+word;

    if(debug)
    {
        std::cout << "[" << n << "] Word: " << word.tostring(*classDecoder) << " History: " << history.tostring(*classDecoder) << "(" << pattern.tostring(*classDecoder) << ")" << std::endl;
    }

    double rv = 0.0;

    std::unordered_map<Pattern, double>::const_iterator iter = pkn_cache.find(pattern);
    if(iter == pkn_cache.end() )
    {
        if(n == 1) //unigram
        {
            if(debug) std::cout << "\t[" << n << "] " << patternModel->occurrencecount(word) << "/" << tokens << "=" << patternModel->occurrencecount(word)/tokens << std::endl;
            return patternModel->occurrencecount(word)/tokens;
        }

        int count = patternModel->occurrencecount(pattern);
        int marginalCount = std::get<3>((*m)[history]);
        double p1 = std::max(0.0, 1.0*(count - D(count))/marginalCount);
        double ptemp =bra->pkn(word, Pattern(history, 1, order-1), debug); 
        double gammaa = gamma(history, true);
        double p2 = gammaa*ptemp;
        if(debug) 
        {   
            std::cout << "\t[" << n << "] c(w^i_i-n+1) = c(" << pattern.tostring(*classDecoder) << ") = " << count << std::endl;
            std::cout << "\t[" << n << "] (" << count << "-" << D(count) << ")/" << marginalCount << " + " << gammaa << "*" << ptemp << std::endl;
            std::cout << "\t[" << n << "]\t" << p1 << "+" << p2 << "=" << p1+p2 << std::endl;
        }
//        std::cout << "[" << n << "] " << pattern.tostring(*classDecoder) << "(c" << count << ":m" << marginalCount << "/g" << gamma(history) << ")" << std::endl;
        rv = p1+p2;
        pkn_cache[pattern] = rv;
    } else
    {
        rv = iter->second;
    }

    return rv;
}

double KneserNey::gamma(const Pattern& pattern, bool debug)
{
    std::tuple<int, int, int, int> NValues = (*m)[pattern];
    double p1 = D1 * std::get<0>(NValues) + D2 * std::get<1>(NValues) + D3plus * std::get<2>(NValues);

    if(debug)
    {
        std::cout << "[" << n << "]\t [" << pattern.tostring(*classDecoder) << "] gamma (" << D1 << "*" << std::get<0>(NValues) << "+" << D2 << "*" << std::get<1>(NValues) << "+" << D3plus << "*" << std::get<2>(NValues) << ")/" << std::get<3>(NValues) << std::endl;
    }
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


void KneserNey::recursiveComputeFrequencyStats()
{
    computeFrequencyStats();
    if(n > 1)
    {
        bra->recursiveComputeFrequencyStats();
    }
}

/**
 * n1 is the number of n-grams that appear exactly one, n2 is ...
 */
void KneserNey::computeFrequencyStats()
{
	LOG(INFO) << "+ Entering computeFrequencyStats for n=" << n;
	std::cout << "+ Entering computeFrequencyStats for n=" << n << std::endl;

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
    
	std::cout << "n: " << n << " [" << total << "] 1:" << n1 << " 2:" << n2 << " 3:" << n3 << " 4:" << n4 << std::endl;
        std::cout << "n: " << n << " Y: " << Y << " D1: " << D1 << " D2: " << D2 << " D3+: " << D3plus << std::endl;
	std::cout << "- Leaving computeFrequencyStats" << std::endl;
	LOG(INFO) << "n: " << n << " [" << total << "] 1:" << n1 << " 2:" << n2 << " 3:" << n3 << " 4:" << n4;
        LOG(INFO) << "n: " << n << " Y: " << Y << " D1: " << D1 << " D2: " << D2 << " D3+: " << D3plus;
	LOG(INFO) << "- Leaving computeFrequencyStats";
}

void KneserNey::iterativeComputeAllN()
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

void KneserNey::computeAllN()
{
    LOG(INFO) << "+ Computing N values for n " << n;
    std::cout << "+ Computing N values for n " << std::endl;

    int N1 = 0;
    int N2 = 0;
    int N3plus = 0;
    int marginalCount = 0;

    int ctr = 0;
    for(const auto& iter: *patternModel )
    {
        
        std::unordered_map<Pattern, std::tuple<int, int, int, int> >::const_iterator miter = m->find(iter.first);
        if(miter == m->end() )
        {
            N1 = 0;
            N2 = 0;
            N3plus = 0;
            marginalCount = 0;
            (*m)[iter.first] = std::tuple<int, int, int, int>(N1, N2, N3plus, marginalCount);
        } else
        {
            std::tuple<int, int, int, int> t = miter->second;
            N1 = std::get<0>(t);
            N2 = std::get<1>(t);
            N3plus = std::get<2>(t);
            marginalCount = std::get<3>(t);
        }

        if(iter.first.size() == n-1)
        {
            //if(ctr++ > 10) break;
//            if(ctr < 10) { std::cout << iter.first.tostring(*classDecoder) << std::endl;}

//            N1 = 0; N2 = 0; N3plus = 0; marginalCount = 0;
            N(iter.first, N1, N2, N3plus, marginalCount);

//            std::cout << std::endl << iter.first.tostring(*classDecoder) << std::endl;
//std::cout << "y[" << N1 << "," << N2 << "," << N3plus << "," << marginalCount << "]" << std::endl;                                                                                     


            (*m)[iter.first] = std::tuple<int, int, int, int>(  N1, N2, N3plus, marginalCount);
//std::tuple<int, int, int, int> NValues = (*m)[iter.first];
//std::cout << "G[" << std::get<0>(NValues) << "," << std::get<1>(NValues) << "," << std::get<2>(NValues) << "," << std::get<3>(NValues) << "]" << std::endl;                                                                                     

//            gamma(iter.first);

        }
    }
}

double KneserNey::N(const Pattern& pattern, int& N1, int& N2, int& N3plus, int& marginalCount)
{
//    Pattern history = Pattern(pattern, 0, n - 1);
    
    int ctr = 0;

    

//        std::cout << "P in: " << pattern.tostring(*classDecoder) << std::endl;
    for(const auto& iter : *patternModel)
    {
    	const Pattern patternFromIndex = iter.first;
 


//        std::cout << "Created pattern: " << intoA.tostring(*collectionClassDecoderPtr) << std::endl;
//        double pknSum = 0.0;
//        for(const auto& iter: *collectionIndexedModelPtr)
//        {
//            if(iter.first.size() == 1)
//            {
//                Pattern newP = intoA + iter.first;
//                double pkn = kneserNeyPtr->pkn(newP, true);
//                pknSum += pkn;
//                std::cout << pkn << "\t" << newP.tostring(*collectionClassDecoderPtr) << std::endl;
//            }
//        }
//        std::cout << "SUM: " << pknSum <<std::endl;





    	if (patternFromIndex.n() == n-1 && pattern == Pattern(patternFromIndex, 0, n - 1))
    	{
//        std::cout << "\t" << patternFromIndex.tostring(*classDecoder) << std::endl;
            int frequency = patternModel->occurrencecount(patternFromIndex);
            marginalCount += frequency;
            
//            if(!patternFromIndex.tostring(*classDecoder).compare("a"))
//            {
//                std::cout << "Computing N for a. Freq " << frequency << " and marg count " << marginalCount << std::endl;
//            }
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
            	    N1 += 1;//frequency;
            	    break;
            	case 2:
            	    N2 += 1;//frequency;
            	    break;
            	default:
            	    N3plus += 1;//frequency;
            	    break;
            }
    	}
    }
//    std::cout << pattern.tostring(*classDecoder) << " N1: " << N1 << " N2: " << N2 << " N3+: " << N3plus << " marginal: " << marginalCount << std::endl;
}

void KneserNey::recursiveComputeAllN()
{
    computeAllN();
    if(n > 1)
    {
        bra->recursiveComputeAllN();
    }
}

