/*
 * KneserNey.cpp
 *
 *  Created on: Oct 28, 2013
 *      Author: lonrust
 */

#include "KneserNey.h"

#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>

typedef boost::shared_ptr<ClassDecoder> ClassDecoder_ptr;

KneserNey::KneserNey(const IndexedPatternModel<>& patternModel)
		: VectorSpaceModel(patternModel)
{
	n1 = 0;
	n2 = 0;
	n3 = 0;
	n4 = 0;

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
		}
		else
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

/**
 * af
 */
double KneserNey::rawProbability(const Pattern& pattern)
{
//	int patternCount = patternCount(pattern);

	int patternCount = 0;

	return (patternCount-discount(patternCount))/getPatternModel().tokens();
}

/**
 * niet af
 */
double KneserNey::smoothedProbability(const Pattern& pattern)
{
	return interpolationFactor(pattern) * rawProbability(pattern);
}

/**
 * gamma, en niet af
 * N_1(w_{i-n+1}^{i-1} •) = N_1(w_{i-n+1}, w_{i-n+2}, ..., w_{i-2}, •)
 * which is like taking the n-gram, and replacing the last word with a wildcard
 * And then take all the patterns that occur once (hence N_1)
 */
double KneserNey::interpolationFactor(const Pattern& pattern)
{
	double term1 = 0.0;
	term1 = 1 - 2 * y() * (n2 / n1); // D1
	term1 *= 1;

	double term2 = 0.0;
	term2 = 2 - 3 * y() * (n3 / n2); // D2
	term2 *= 1;

	double term3 = 0.0;
	term3 = 3 - 4 * y() * (n4 / n3); // D3+
	term3 *= 1;

	return (term1 + term2 + term3) / getPatternModel().tokens();
}

/**
 * pattern is at least length 2, because it will take length-1 as beginning, and the last 1 as the wildcard
 */
double KneserNey::N(const Pattern& pattern)
{
//	for (IndexedPatternModel<>::iterator iter = patternModel.begin(); iter != patternModel.end(); iter++)
//	{
//		const Pattern pattern = iter->first;
//		const IndexedData data = iter->second;
//
//		double value = patternModel.occurrencecount(pattern);
//
//
//	}
}

double KneserNey::getSmoothedValue(const Pattern& pattern)
{
	int currentNInNgram = pattern.n();
	int nextN = currentNInNgram - 1;

	return rawProbability(pattern) + smoothedProbability(Pattern(pattern, pattern.n() - nextN,nextN));
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





int main(int argc, char** argv)
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

	ClassDecoder_ptr collectionClassDecoderPtr(new ClassDecoder(collectionClassFile));

	std::string collectionInputFileName = "docs/aiw.tok.colibri.dat";
	std::string collectionOutputFileName = "docs/aiw.tok.colibri.patternmodel";

	IndexedPatternModel<> collectionIndexedModel;
	collectionIndexedModel.train(collectionInputFileName, options);



	std::cout << "Iterating over all patterns in all docs" << std::endl;
	for (IndexedPatternModel<>::iterator iter = collectionIndexedModel.begin(); iter != collectionIndexedModel.end(); iter++)
	{
		const Pattern pattern = iter->first;
		const IndexedData data = iter->second;

		double value = collectionIndexedModel.occurrencecount(pattern);
		std::cout << ">" << pattern.tostring(*collectionClassDecoderPtr) << "," << value << std::endl;

	}

	KneserNey vsm = KneserNey(collectionIndexedModel);

	std::vector< std::string> documentInputFiles = std::vector< std::string>();
	documentInputFiles.push_back(std::string("docs/aiw-1.tok"));
	documentInputFiles.push_back(std::string("docs/aiw-2.tok"));
	documentInputFiles.push_back(std::string("docs/aiw-3.tok"));

	int docCntr = 0;
	BOOST_FOREACH( std::string fileName, documentInputFiles )
	{
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
	for(VectorSpaceModel::documentItr docItr = vsm.begin(); docItr != vsm.end(); ++docItr)
	{
		std::cout << docItr->toString() << std::endl;
		boost::shared_ptr< ClassDecoder> decoder = docItr->getClassDecoder();
		for(Document::featureItr featItr = docItr->begin(); featItr != docItr->end(); ++featItr)
		{
//			std::cout << docItr->toString(featItr) << "[" << vsm.getTFIDF(featItr->first, *docItr) << "]" << std::endl;
		}
		std::cout << std::endl;
	}


	vsm.computeFrequencyStats();
//	vsm.test();


	std::cout << "ALS EEN REIGER" << std::endl;
}
