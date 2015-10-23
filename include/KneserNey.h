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


/**
 * EVERYTHING IN LOG SPACE
 */
class KneserNey: public VectorSpaceModel
{
public:
	static constexpr double epsilon = 0.000001;

        const int order;

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


	Modification algorithm;

	KneserNey(IndexedPatternModel<>* patternModel, ClassDecoder* classDecoder, Modification algorithm = Modification::MKN);
	//virtual ~KneserNey();
	double getSmoothedValue(const Pattern& pattern, int indentation = 0);

	void computeFrequencyStats(int indentation = 0);
        void recursiveComputeFrequencyStats(int indentation = 0);
        void computeAllN(int indentation = 0);
        void recursiveComputeAllN(int indentation = 0);
	double computeSimularity(const Document& document);

        double gamma(const Pattern& pattern);
        
        double D(int c);
protected:
	KneserNey(int order, IndexedPatternModel<>* patternModel, ClassDecoder* classDecoder, Modification algorithm = Modification::MKN);
        
        KneserNey* bra;
private:

	double n1, n2, n3, n4;
        double Y, D1, D2, D3plus;
	double tokens;

        std::unordered_map<Pattern, std::tuple<int, int, int, int> > m;
        double sumPatternCounts = 1;

	/**
	 * The raw probability is a discounted probability. The n-gram counts are
	 * discounted based on their frequency, and normalised by the number of
	 * tokens.
	 */
	double rawProbability(const Pattern& pattern, int indentation = 0);

	/**
	 * The back-off part of the formula, where the back-off part is smoothed
	 * with an interpolation factor.
	 */
	double smoothedProbability(const Pattern& pattern, int indentation = 0);

	/**
	 * The interpolation factor determines the impact of the lower order (back
	 * off) on the result.
	 */
	double interpolationFactor(const Pattern& pattern, int indentation = 0);

	/**
	 * This is a congregated function that determines N_1, N_2, and N_3+ for
	 * the pattern at one go. N_x is the number of n-grams of which the last
	 * word is a wildcard, and occur exactly x times.
	 */
	double N(const Pattern& pattern, int& N1, int& N2, int& N3, int& marginalCount);

	double wordChanceForOrder(const Pattern& pattern, int order);
	int patternCount(const Pattern& pattern);
};

#endif /* KNESERNEY_H_ */
