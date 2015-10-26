/*
 * KneserNey.h
 *
 *  Created on: Oct 28, 2013
 *      Author: lonrust
 */

#ifndef KNESERNEY_H_
#define KNESERNEY_H_

#include "VectorSpaceModel.h"
#include <pattern.h>

#include <cereal/types/unordered_map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/binary.hpp>
#include <fstream>

/**
 * EVERYTHING IN LOG SPACE
 */
class KneserNey: public VectorSpaceModel
{
public:
	static constexpr double epsilon = 0.000001;

        const int order;
        const int n;
//
// template <class Archive>
//   void serialize( Archive & ar )
//    {
//        ar( order, n, algorithm, n1, n2, n3, n4, Y, D1, D2, D3plus, tokens, m );
//    }

	/**
	 * Modified Kneser-Ney
	 * Modified Kneser-Ney Count
	 * Modified Kneser-Ney Extend
	 * Modified Kneser-Ney Discount
 * Modified Kneser-Ney Flex
	 */
	enum Modification
	{
		MKN, MKNC, MKNE, MKND, MKNF
	};

        void doSomething(int indentation = 0);

	Modification algorithm;

	KneserNey(IndexedPatternModel<>* patternModel, ClassDecoder* classDecoder, Modification algorithm = Modification::MKN);

	void computeFrequencyStats(int indentation = 0);
        void recursiveComputeFrequencyStats(int indentation = 0);
        void computeAllN(int indentation = 0);
        void recursiveComputeAllN(int indentation = 0);
        void iterativeComputeAllN(int indentation = 0);

        double gamma(const Pattern& pattern);
       
        bool isOOV(const Pattern& pattern, int indentation = 0);

        double D(int c);


        double pkn(const Pattern& word, const Pattern& history, int indentation = 0);
        double pkn(const Pattern& pattern, int indentation = 0);
protected:
	KneserNey(int order, IndexedPatternModel<>* patternModel, ClassDecoder* classDecoder, Modification algorithm = Modification::MKN);
        
        KneserNey* bra;
private:
	double n1, n2, n3, n4;
        double Y, D1, D2, D3plus;
	double tokens;

        std::unordered_map<Pattern, std::tuple<int, int, int, int> > m;
        std::unordered_map<Pattern, double> pkn_cache;


	/**
	 * This is a congregated function that determines N_1, N_2, and N_3+ for
	 * the pattern at one go. N_x is the number of n-grams of which the last
	 * word is a wildcard, and occur exactly x times.
	 */
	double N(const Pattern& pattern, int& N1, int& N2, int& N3, int& marginalCount);

};

#endif /* KNESERNEY_H_ */
