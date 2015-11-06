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


double KneserNey::pkn(const Pattern& pattern, const Pattern& word, const Pattern& history, bool debug)
{
    if(n == MAXLEVEL)
    {
        int count_WXYZ = patternModel->occurrencecount(pattern);
        int count_WXY = patternModel->occurrencecount(history);
        double prob = std::max(0.0, (1.0*count_WXYZ-D(count_WXYZ))/count_WXY);
        Pattern XY = Pattern(history, 1, n-1);

        double interpolation = gamma(history);
        if(count_WXY == 0) interpolation = 1.0; // as per personal communication with Stan Chen 
        double backoff = bra->pkn(XY+word, word, XY, debug); 
        std::cout << "[" << n << "] Count(" << pattern.tostring(*classDecoder) << "):" << count_WXYZ << " prob:" << prob << " interpolation:" << interpolation << " backoff:" << backoff << std::endl;
        return prob+interpolation*backoff;
    } else if(n == 1) // unigram
    {
        Pattern Z = word;

        double N1p_oZ = 0;
        const auto& ZIter = contextValues_n1p_oXYZ->find(Z);
        if(ZIter != contextValues_n1p_oXYZ->end()) N1p_oZ = ZIter->second;

        double N1p_oo = 0;
        const auto& Iter = contextValues_n1p_oXYo->find(Pattern());
        if(Iter != contextValues_n1p_oXYo->end()) N1p_oo = Iter->second;

        double interpolation = gamma(Pattern());


        double backoff = 1.0/patternModel->totalwordtypesingroup(0, 1);
        std::cout << "[" << n-1 << "] backoff = 1/" << patternModel->totalwordtypesingroup(0,1) << " = " << backoff << std::endl;


        std::cout << "[" << n << "] N1p(o" << Z.tostring(*classDecoder) << "):" << N1p_oZ << " N1p(oo):" << N1p_oo << std::endl;



        return std::max(0.0, (1.0*N1p_oZ - D(patternModel->occurrencecount(word)))/N1p_oo) + interpolation*backoff;
    } else
    {
        int count_XYZ = patternModel->occurrencecount(pattern);
        
        Pattern XY = history;
        int N1p_oXYo = 0;
        const auto& XYiter = contextValues_n1p_oXYo->find(XY);
        if(XYiter != contextValues_n1p_oXYo->end()) N1p_oXYo = XYiter->second;

        Pattern XYZ = pattern;
        int N1p_oXYZ = 0;
        const auto& XYZiter = contextValues_n1p_oXYZ->find(XYZ);
        if(XYZiter != contextValues_n1p_oXYZ->end()) N1p_oXYZ = XYZiter->second;
        
        
        double prob = std::max(0.0, (1.0*N1p_oXYZ-D(count_XYZ))/N1p_oXYo);
        double interpolation = gamma(history);
        Pattern Y = Pattern(history, 1, n-1);
        double backoff = bra->pkn(Y+word, word, Y, debug);
        std::cout << "[" << n << "] N1p(o" << XY.tostring(*classDecoder) << "o):" << N1p_oXYo << " prob:" << prob << " interpolation:" << interpolation << " backoff:" << backoff << std::endl;
        return prob+interpolation*backoff;
    }
}

double KneserNey::gamma(const Pattern& history, bool debug)
{
    std::tuple<int, int, int> values = (*contextValues)[history];
    double p1 = D1 * std::get<0>(values) + D2 * std::get<1>(values) + D3plus * std::get<2>(values);
    std::cout << "[" << n << "]\ty(" << history.tostring(*classDecoder) << ") = " << D1 << "*" << std::get<0>(values) << " + " << D2 << "*" << std::get<1>(values) << " + " << D3plus << "*" << std::get<2>(values) << " = " << p1 << std::endl;


    if(n == MAXLEVEL)
    {
        std::cout << "[" << n << "]\t\t/" << patternModel->occurrencecount(history) << " = " << std::max(0.0, p1/patternModel->occurrencecount(history)) << std::endl;
        return std::max(0.0, p1/patternModel->occurrencecount(history));
    } else
    {
        Pattern XY = history;
        int N1p_oXYo = 0;
        const auto& XYiter = contextValues_n1p_oXYo->find(XY);
        if(XYiter != contextValues_n1p_oXYo->end()) N1p_oXYo = XYiter->second;

        std::cout << "[" << n << "]\t\t/" << N1p_oXYo << " = " << p1/N1p_oXYo << std::endl;
        return std::max(0.0, p1/N1p_oXYo);
    }
}   

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
    if(n > 1)
    {
        bra->recursivePrecomputeContextValues();
    }
}

void KneserNey::precomputeContextValues()
{
    std::cout << "+ Precomputing context values for n " << n << " (of " << MAXLEVEL << ")" << std::endl;

    if(n == 1)
    {
        int N1p_oZ = 0;
        int N1p_oo = 0;
        int N1_o = 0;
        int N2_o = 0;
        int N3p_o = 0;

        for(const auto& iter: *patternModel)
        {
            if(iter.first.size() == 1)
            {
                Pattern Z = iter.first;
                
                const auto& ZIter = contextValues->find(Pattern());
                if(ZIter == contextValues->end())
                {
                    N1_o = 0;
                    N2_o = 0;
                    N3p_o = 0;
                } else
                {
                    std::tuple<int, int, int> values = ZIter->second;
                    N1_o = std::get<0>(values);
                    N2_o = std::get<1>(values);
                    N3p_o = std::get<2>(values);
                }

                switch(patternModel->occurrencecount(Z))
                {
                    case 0: break;
                    case 1: ++N1_o; break;
                    case 2: ++N2_o; break;
                    default: ++N3p_o; break;
                }

                (*contextValues)[Pattern()] = std::tuple<int, int, int>(N1_o, N2_o, N3p_o);
                
            } else if(iter.first.size() == 2)
            {
                Pattern YZ = iter.first;
                Pattern Z = Pattern(YZ, 1, 1);

                const auto& ZIter = contextValues_n1p_oXYZ->find(Z);
                if(ZIter == contextValues_n1p_oXYZ->end())
                {
                    N1p_oZ = 0;
                } else 
                {
                    N1p_oZ = ZIter->second;
                }
                
                int freq_oZ = patternModel->occurrencecount(YZ);
                if(freq_oZ) ++N1p_oZ;
                (*contextValues_n1p_oXYZ)[Z] = N1p_oZ;

                const auto& Iter = contextValues_n1p_oXYo->find(Pattern());
                if(Iter == contextValues_n1p_oXYo->end())
                {
                    N1p_oo = 0;
                } else
                {
                    N1p_oo = Iter->second;
                }

                int freq_oo = patternModel->occurrencecount(YZ);
                if(freq_oo) ++N1p_oo;
                (*contextValues_n1p_oXYo)[Pattern()] = N1p_oo;

            }
        }
    } else if(n == MAXLEVEL)
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
//        for(const auto& iter: *contextValues)
//        {
//            std::tuple<int, int, int> values = iter.second;
//            std::cout << iter.first.tostring(*classDecoder) << ": [" << std::get<0>(values) << "," << std::get<1>(values) << "," << std::get<2>(values) << ";" << (*contextValues_n1p_oXYZ)[iter.first] << "," << (*contextValues_n1p_oXYo)[iter.first] << "]" << std::endl;
//        }
    }
    std::cout << "- Precomputing context values for n " << n << " (of " << MAXLEVEL << ")" << std::endl;
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

    return pkn(pattern, Pattern(pattern, order, 1), Pattern(pattern, 0, order), debug);
}

bool KneserNey::isOOVWord(const Pattern& word)
{
    return !patternModel->has(word);
}

bool KneserNey::isOOV(const Pattern& pattern)
{
    
    return !patternModel->has(Pattern(pattern, order, 1));
}
