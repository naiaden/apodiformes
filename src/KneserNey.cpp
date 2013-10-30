/*
 * KneserNey.cpp
 *
 *  Created on: Oct 28, 2013
 *      Author: lonrust
 */

#include "KneserNey.h"

#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>

KneserNey::KneserNey(const IndexedPatternModel<>& patternModel, boost::shared_ptr<ClassDecoder> classDecoder, Modification algorithm )
		: VectorSpaceModel(patternModel, classDecoder), algorithm(algorithm)
{
	n1 = 0;
	n2 = 0;
	n3 = 0;
	n4 = 0;

}

double KneserNey::computeSimularity(const Document& document)
{
	std::cerr << "KneserNey::computeSimularity is unimplemented" << std::endl;
	return 4.0;
}

/**
 * af
 */
double KneserNey::smoothedProbability(const Pattern& pattern, int indentation)
{
	std::cout << std::string(indentation, '\t') << ">" << indentation << " Computing smoothed probability" << std::endl;

	Pattern smallerPattern = Pattern(pattern, 1, pattern.n() - 1);
	double rValue = interpolationFactor(pattern, indentation + 1) * getSmoothedValue(smallerPattern, indentation + 1);

	std::cout << std::string(indentation, '\t') << "<" << indentation << " smoothed probability" << rValue << std::endl;

	return rValue;
}

/**
 * n1 is the number of n-grams that appear exactly one, n2 is ...
 */
void KneserNey::computeFrequencyStats()
{
	int nulls = 0;

	int total = 0;

	IndexedPatternModel<> ipm = getPatternModel();

	for (IndexedPatternModel<>::iterator iter = ipm.begin(); iter != ipm.end(); iter++)
	{

		++total;

		unsigned char* data = iter->first.data;
		if (data == NULL)
		{
			std::cout << "data is NULL" << std::endl;
			++nulls;
		} else
		{
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

	std::cout << "[" << total << "] 1:" << n1 << " 2:" << n2 << " 3+:" << n3 << "(" << nulls << ")" << std::endl;
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
 * af
 */
double KneserNey::rawProbability(const Pattern& pattern, int indentation)
{
	std::cout << std::string(indentation, '\t') << ">" << indentation << " Computing raw probability" << std::endl;

	double pCount = patternCount(pattern);
	double rValue = 0.0;
	double tokens = getPatternModel().tokens();

	if (pattern.n() == 1)
	{
		rValue = pCount / tokens;
		std::cout << std::string(indentation + 1, '\t') << "pCount(" << pCount << ") and tokens("
		        << getPatternModel().tokens() << ")" << std::endl;
	} else
	{
		rValue = (pCount - discount(pCount)) / tokens;
		std::cout << std::string(indentation + 1, '\t') << "pCount(" << pCount << "), discount(" << discount(pCount)
		        << "), and tokens(" << getPatternModel().tokens() << ")" << std::endl;
	}

	std::cout << std::string(indentation, '\t') << "<" << indentation << " raw probability = " << rValue << std::endl;

	return rValue;
}

/**
 * gamma, af
 * N_1(w_{i-n+1}^{i-1} •) = N_1(w_{i-n+1}, w_{i-n+2}, ..., w_{i-2}, •)
 * which is like taking the n-gram, and replacing the last word with a wildcard
 * And then take all the patterns that occur once (hence N_1)
 */
double KneserNey::interpolationFactor(const Pattern& pattern, int indentation)
{
	std::cout << std::string(indentation, '\t') << ">" << indentation << " Computing interpolation factor" << std::endl;

	int N1 = 0;
	int N2 = 0;
	int N3 = 0;

	N(pattern, N1, N2, N3);

	double term1 = 0.0;
	term1 = 1 - 2 * y() * (n2 / n1); // D1
	term1 *= N1;

	double term2 = 0.0;
	term2 = 2 - 3 * y() * (n3 / n2); // D2
	term2 *= N2;

	double term3 = 0.0;
	term3 = 3 - 4 * y() * (n4 / n3); // D3+
	term3 *= N3;

	double rValue = (term1 + term2 + term3) / getPatternModel().tokens();

	std::cout << std::string(indentation, '\t') << "<" << indentation << " interpolation factor: " << rValue << " with N1(" << N1
	        << ") N2(" << N2 << ") N3(" << N3 << ")" << std::endl;

	return rValue;
}

/**
 * pattern is at least length 2, because it will take length-1 as beginning, and the last 1 as the wildcard
 */
double KneserNey::N(const Pattern& pattern, int& N1, int& N2, int& N3)
{
	int patternLength = pattern.n();

	Pattern newPattern = Pattern(pattern, 0, patternLength - 1);

	IndexedPatternModel<> patternModel = getPatternModel();
	for (IndexedPatternModel<>::iterator iter = patternModel.begin(); iter != patternModel.end(); iter++)
	{
		const Pattern patternFromIndex = iter->first;
		const IndexedData data = iter->second;

		if (patternFromIndex.n() == patternLength && newPattern == Pattern(patternFromIndex, 0, patternLength - 1))
		{
			int frequency = patternModel.occurrencecount(patternFromIndex);

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
					N3 += frequency;
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
	std::cout << std::string(indentation, '\t') << "+" << indentation << " Computing getSmoothedValue for pattern ["
	        << pattern.tostring(*classDecoder) << "]" << std::endl;

	int currentNInNgram = pattern.n();
	int nextN = currentNInNgram - 1;

	double rValue = 0.0;

	if (currentNInNgram == 1)
	{
		rValue = rawProbability(pattern, indentation + 1);
		std::cout << std::string(indentation, '\t') << "<" << indentation << " smoothed value for unigram: " << rValue
		        << std::endl;
	} else
	{

		double rawProb = rawProbability(pattern, indentation + 1);
		double smoProb = smoothedProbability(Pattern(pattern, pattern.n() - nextN - 1, currentNInNgram),
		        indentation + 1);
		rValue = rawProb + smoProb;

		std::cout << std::string(indentation, '\t') << "<" << indentation << " smoothed value = " << rValue
		        << " with rawProb(" << rawProb << ") and smoProb(" << smoProb << ")" << std::endl;
	}

	return rValue;
}

double KneserNey::y()
{
	return n1 / (n1 + 2 * n2);
}

double KneserNey::discount(int count)
{
	if (count < 0)
	{
		std::cerr << "Invalid count value " << count << std::endl;
	}

	switch (count)
	{
		case 0:
			return 0;
		case 1:
			return 1 - 2 * y() * (n2 / n1);
		case 2:
			return 2 - 3 * y() * (n3 / n2);
		default:
			return 3 - 4 * y() * (n4 / n3);
	}
}

int main1(int argc, char** argv)
{
	std::cout << "STRAK" << std::endl;

	std::cerr << "Class encoding corpus..." << std::endl;
	system("colibri-classencode docs/aiw.tok");

	PatternModelOptions options;
	options.DOREVERSEINDEX = true;
	options.DOSKIPGRAMS = true;
	options.MINTOKENS = 1;
	options.MAXLENGTH = 5;

	const std::string collectionClassFile = "docs/aiw.tok.colibri.cls";

	ClassEncoder collectionClassEncoder = ClassEncoder(collectionClassFile);

	boost::shared_ptr<ClassDecoder> collectionClassDecoderPtr(new ClassDecoder(collectionClassFile));

	std::string collectionInputFileName = "docs/aiw.tok.colibri.dat";
	std::string collectionOutputFileName = "docs/aiw.tok.colibri.patternmodel";

	IndexedPatternModel<> collectionIndexedModel;
	collectionIndexedModel.train(collectionInputFileName, options);

//	std::cout << "Iterating over all patterns in all docs" << std::endl;
//	for (IndexedPatternModel<>::iterator iter = collectionIndexedModel.begin(); iter != collectionIndexedModel.end();
//	        iter++)
//	{
//		const Pattern pattern = iter->first;
//		const IndexedData data = iter->second;
//
//		double value = collectionIndexedModel.occurrencecount(pattern);
//		std::cout << ">" << pattern.tostring(*collectionClassDecoderPtr) << "," << value << std::endl;
//
//	}

	KneserNey vsm = KneserNey(collectionIndexedModel, collectionClassDecoderPtr);

	std::vector<std::string> documentInputFiles = std::vector<std::string>();
	documentInputFiles.push_back(std::string("docs/aiw-1.tok"));
	documentInputFiles.push_back(std::string("docs/aiw-2.tok"));
	documentInputFiles.push_back(std::string("docs/aiw-3.tok"));

	int docCntr = 0;
	BOOST_FOREACH( std::string fileName, documentInputFiles ){
	Document document = Document(docCntr++, fileName, collectionClassDecoderPtr);

	const std::string command = std::string("colibri-classencode -c docs/aiw.tok.colibri.cls ") + fileName;
	system( command.c_str() );

	const std::string documentClassFile = fileName + ".cls";
	const std::string inputFileName = fileName + ".colibri.dat";
	const std::string outputFileName = fileName + ".colibri.patternmodel";

	ClassDecoder documentClassDecoder = ClassDecoder(documentClassFile);

	IndexedPatternModel<> documentModel;
	documentModel.train(inputFileName, options);

	int k = 0;

	std::cout << "Iterating over all patterns in " << fileName << std::endl;
	for (IndexedPatternModel<>::iterator iter = documentModel.begin(); iter != documentModel.end(); iter++)
	{
		const Pattern pattern = iter->first;
		const IndexedData data = iter->second;

		double value = documentModel.occurrencecount(pattern);

		document.updateValue(pattern, value);

//			std::cout << "-" << document.toString(pattern) << "," << document.getValue(pattern) << std::endl;

		++k;
	}

	std::cout << ">>> " << k << std::endl;

	vsm.addDocument(document);

}

	std::cout << "The vector space contains " << vsm.numberOfDocuments() << " documents" << std::endl;
	for (VectorSpaceModel::documentItr docItr = vsm.begin(); docItr != vsm.end(); ++docItr)
	{
		std::cout << docItr->toString() << std::endl;
		boost::shared_ptr<ClassDecoder> decoder = docItr->getClassDecoder();
		for (Document::featureItr featItr = docItr->begin(); featItr != docItr->end(); ++featItr)
		{
//			std::cout << docItr->toString(featItr) << "[" << vsm.getTFIDF(featItr->first, *docItr) << "]" << std::endl;
		}
		std::cout << std::endl;
	}

	vsm.computeFrequencyStats();

	std::cout << "Iterating over all patterns in all docs" << std::endl;
	for (IndexedPatternModel<>::iterator iter = collectionIndexedModel.begin(); iter != collectionIndexedModel.end();
	        iter++)
	{
		const Pattern pattern = iter->first;

		std::cout << "====================================================" << std::endl;
		double value = vsm.getSmoothedValue(pattern);

//		std::cout << pattern.out() << " - " << value << std::endl;

	}

	std::cout << "ALS EEN REIGER" << std::endl;
}
