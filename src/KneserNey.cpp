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
        LOG(INFO) << "Found " << tokens << " unigram tokens";
    }

}


double KneserNey::pkn(const Pattern& word, const Pattern& history, bool debug)
{
    return 0.0;
//    Pattern pattern = history+word;
//
//    if(debug)
//    {
//        std::cout << "[" << n << "] Word: " << word.tostring(*classDecoder) << " History: " << history.tostring(*classDecoder) << "(" << pattern.tostring(*classDecoder) << ")" << std::endl;
//    }
//
//    double rv = 0.0;
//
//    std::unordered_map<Pattern, double>::const_iterator iter = pkn_cache.find(pattern);
//    if(iter == pkn_cache.end() )
//    {
//        if(n == 1) //unigram
//        {
//            if(debug) std::cout << "\t[" << n << "] " << patternModel->occurrencecount(word) << "/" << tokens << "=" << patternModel->occurrencecount(word)/tokens << std::endl;
//            return patternModel->occurrencecount(word)/tokens;
//        }
//
//        if(debug) 
//        {   
//            int count = patternModel->occurrencecount(pattern);
//            int marginalCount = std::get<3>((*m)[history]);
//            double p1 = std::max(0.0, 1.0*(count - D(count))/marginalCount);
//            double ptemp =bra->pkn(word, Pattern(history, 1, order-1), debug); 
//            double gammaa = gamma(history, true);
//            double p2 = gammaa*ptemp;
//                std::cout << "\t[" << n << "] c(w^i_i-n+1) = c(" << pattern.tostring(*classDecoder) << ") = " << count << std::endl;
//                std::cout << "\t[" << n << "] (" << count << "-" << D(count) << ")/" << marginalCount << " + " << gammaa << "*" << ptemp << std::endl;
//                std::cout << "\t[" << n << "]\t" << p1 << "+" << p2 << "=" << p1+p2 << std::endl;
//            rv = p1+p2;
//            pkn_cache[pattern] = rv;
//        } else
//        {
//            int count = patternModel->occurrencecount(pattern);
//            double p1 = std::max(0.0, 1.0*(count - D(count))/std::get<3>((*m)[history]));
//            double p2 = gamma(history, debug)*bra->pkn(word, Pattern(history, 1, order-1), debug);
//            rv = p1 + p2;
//            pkn_cache[pattern] = rv;
//        }
//    } else
//    {
//        rv = iter->second;
//    }
//
//    return rv;
}

double KneserNey::gamma(const Pattern& pattern, bool lowerOrder, bool debug)
{
    return 0.0;
//    std::tuple<int, int, int, int> NValues = (*m)[pattern];
//    double p1 = D1 * std::get<0>(NValues) + D2 * std::get<1>(NValues) + D3plus * std::get<2>(NValues);
//
//    if(debug)
//    {
//        std::cout << "[" << n << "]\t [" << pattern.tostring(*classDecoder) << "] gamma (" << D1 << "*" << std::get<0>(NValues) << "+" << D2 << "*" << std::get<1>(NValues) << "+" << D3plus << "*" << std::get<2>(NValues) << ")/" << std::get<3>(NValues) << std::endl;
//    }
//    
//    if(lowerOrder)
//    {
//        return std::max(0.0, p1/Pattern) // (D_1*N_1(w(i-1,i-n+1) .) + D_2*N_2(w(i-1,i-n+1) .) + D_3*N_3(w(i-1,i-n+1) .)) / N_1+(. w(i-1,i-n+1) .)
//    }
//    
//    return std::max(0.0, p1/std::get<3>(NValues)); // D_1*N_1(w(i-1,i-n+1) .) + D_2*N_2(w(i-1,i-n+1) .) + D_3*N_3(w(i-1),i-n+1) .)) / c(w(i-1,i-n+1)
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

void KneserNey::recursivePrecomputeContextValues()
{
    precomputeContextValues();
    if(n > 2)
    {
        bra->recursivePrecomputeContextValues();
    }
}

void KneserNey::precomputeContextValues()
{
    std::cout << "+ Precomputing context values for n " << n << " (of " << MAXLEVEL << ")" << std::endl;

    if(n == MAXLEVEL)
    {
        // als n=4
        int N1_WXYo = 0;
        int N2_WXYo = 0;
        int N3p_WXYo = 0;

        for(const auto& iter: *patternModel)
        {
            Pattern WXYZ = iter.first;
            if(WXYZ.size() == n)
            {

                Pattern WXY = Pattern(WXYZ, 0, n-1);
//                std::cout << "\tProcessing " << WXYZ.tostring(*classDecoder) << " (" << WXY.tostring(*classDecoder) << ")" << std::endl;
                std::unordered_map<Pattern, std::tuple<int, int, int> >::const_iterator WXYIter = contextValues->find(WXY);
                if(WXYIter == contextValues->end())
                {
                    N1_WXYo = 0;
                    N2_WXYo = 0;
                    N3p_WXYo = 0;
                } else
                {
                    std::tuple<int, int, int> values = WXYIter->second;
                    N1_WXYo = std::get<0>(values);
                    N2_WXYo = std::get<1>(values);
                    N3p_WXYo = std::get<2>(values);
                }

                switch(patternModel->occurrencecount(WXYZ))
                {
                    case 0: break;
                    case 1: ++N1_WXYo; break;
                    case 2: ++N2_WXYo; break;
                    default: ++N3p_WXYo; break;
                }

                (*contextValues)[WXY] = std::tuple<int, int, int>(N1_WXYo, N2_WXYo, N3p_WXYo);
            }
        }
//        for(const auto& iter: *contextValues)
//        {
//            std::tuple<int, int, int> values = iter.second;
//            std::cout << iter.first.tostring(*classDecoder) << ": [" << std::get<0>(values) << "," << std::get<1>(values) << "," << std::get<2>(values) << "]" << std::endl;
//        }
    } else
    {
        // anders
        int N1p_oXYZ = 0;
        int N1p_oXYo = 0;
        int N1_XYo = 0;
        int N2_XYo = 0;
        int N3p_XYo = 0;
        
        for(const auto& iter: *patternModel)
        {
            if(iter.first.size() == n+1) // dus oXYZ of oXYo
            {
                Pattern WXYZ = iter.first;
                Pattern XYZ = Pattern(WXYZ, 1, n);
                Pattern XY = Pattern(WXYZ, 1, n-1);

                //std::cout << "WXYZ:" << WXYZ.tostring(*classDecoder) << " XYZ:" << XYZ.tostring(*classDecoder) << " XY:" << XY.tostring(*classDecoder) << std::endl;

                std::unordered_map<Pattern, int>::const_iterator XYZIter = contextValues_n1p_oXYZ->find(XYZ);
                if(XYZIter == contextValues_n1p_oXYZ->end())
                {
                    N1p_oXYZ = 0;
                } else 
                {
                    N1p_oXYZ = XYZIter->second;
                }
                
                // hier zeker maken dat XYZ op het einde zit van WXYZ
                int freq_oXYZ = patternModel->occurrencecount(WXYZ);

                std::unordered_map<Pattern, int>::const_iterator XYIter = contextValues_n1p_oXYo->find(XY);
                if(XYIter == contextValues_n1p_oXYo->end())
                {
                    N1p_oXYo = 0;
                } else
                {
                    N1p_oXYo = XYIter->second;
                }

                // hier zeker maken dat XY in het midden zit van WXYZ
                int freq_oXYo = patternModel->occurrencecount(WXYZ);

                if(freq_oXYZ) ++N1p_oXYZ;
                if(freq_oXYo) ++N1p_oXYo;

                (*contextValues_n1p_oXYZ)[XYZ] = N1p_oXYZ;
                (*contextValues_n1p_oXYo)[XY] = N1p_oXYo;
            }
       
            if(iter.first.size() == n)
            {
                Pattern XYZ = iter.first;
                Pattern XY = Pattern(XYZ, 0, n-1);
                
                std::unordered_map<Pattern, std::tuple<int, int, int> >::const_iterator XYIter = contextValues->find(XY);
                if(XYIter == contextValues->end())
                {
                    N1_XYo = 0;
                    N2_XYo = 0;
                    N3p_XYo = 0;
                } else 
                {
                    std::tuple<int, int, int> values = XYIter->second;
                    N1_XYo = std::get<0>(values);
                    N2_XYo = std::get<1>(values);
                    N3p_XYo = std::get<2>(values);
                }

                switch(patternModel->occurrencecount(XYZ))
                {
                    case 0: break;
                    case 1: ++N1_XYo;
                    case 2: ++N2_XYo;
                    default: ++N3p_XYo;
                }

                (*contextValues)[XY] = std::tuple<int, int, int>(N1_XYo, N2_XYo, N3p_XYo);
            }
        }
        for(const auto& iter: *contextValues)
        {
            std::tuple<int, int, int> values = iter.second;
            std::cout << iter.first.tostring(*classDecoder) << ": [" << std::get<0>(values) << "," << std::get<1>(values) << "," << std::get<2>(values) << ";" << (*contextValues_n1p_oXYZ)[iter.first] << "," << (*contextValues_n1p_oXYo)[iter.first] << "]" << std::endl;
        }
    }
    std::cout << "- Precomputing context values for n " << n << " (of " << MAXLEVEL << ")" << std::endl;
}

double KneserNey::N(const Pattern& pattern, int& N1, int& N2, int& N3plus, int& marginalCount, const Pattern& p)
{
    for(const auto& iter : *patternModel)
    {
    	const Pattern patternFromIndex = iter.first;
        if(patternFromIndex.size() == n-1 )
        {
            Pattern smallerPattern = Pattern(pattern, 0, n-1);

            if (patternFromIndex == smallerPattern)
            {
                 if(p == smallerPattern) std::cout << "Processing pattern:" << pattern.tostring(*classDecoder) << " smallerPattern:" << smallerPattern.tostring(*classDecoder) << " p:" << p.tostring(*classDecoder) << " pfi:" << patternFromIndex.tostring(*classDecoder) << std::endl;
                

                int frequency = patternModel->occurrencecount(pattern);
                marginalCount += frequency;
                
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
                //if(p == pattern) { 
                if(p == smallerPattern) std::cout << "freq:" << frequency << " N1:" << N1 << " N2:" << N2 << " N3+:" << N3plus << " M:" << marginalCount << std::endl; 
                //}
            }
        }
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
   
	LOG(INFO) << "n: " << n << " [" << total << "] 1:" << n1 << " 2:" << n2 << " 3:" << n3 << " 4:" << n4;
        LOG(INFO) << "n: " << n << " Y: " << Y << " D1: " << D1 << " D2: " << D2 << " D3+: " << D3plus;
	LOG(INFO) << "- Leaving computeFrequencyStats";
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
