/*
 * TFIDF.cpp
 *
 *  Created on: Oct 28, 2013
 *      Author: lonrust
 */

#include "TFIDF.h"

#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>

typedef boost::shared_ptr<ClassDecoder> ClassDecoder_ptr;

TFIDF::TFIDF(const IndexedPatternModel<>& patternModel, boost::shared_ptr<ClassDecoder> classDecoder)
		: VectorSpaceModel(patternModel, classDecoder)
{

}

double TFIDF::getTFIDF(const Pattern& pattern, const Document& document)
{
	double tf = getTF(pattern, document);
	double idf = getIDF(pattern);

	return tf * idf;
}

TFIDF::~TFIDF()
{
	// TODO Auto-generated destructor stub
}

int TFIDF::getFrequency(const Pattern& pattern)
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

int TFIDF::getFrequency(const Pattern& pattern, const Document& document)
{
	return static_cast<int>(document.getValue(pattern));
}

double TFIDF::getTF(const Pattern& pattern, const Document& document)
{
	return log(getFrequency(pattern, document) + 1);
}

double TFIDF::getIDF(const Pattern& pattern)
{
	const int numberOfDocuments = this->numberOfDocuments();
	double numberOfDocumentsWithPattern = 0.0;

	for (documentItr docItr = begin(); docItr != end(); ++docItr)
	{
		if (getFrequency(pattern, *docItr) > 0)
		{
			++numberOfDocumentsWithPattern;
		}
	}

	return log(numberOfDocuments / numberOfDocumentsWithPattern);
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
	for (IndexedPatternModel<>::iterator iter = collectionIndexedModel.begin(); iter != collectionIndexedModel.end();
	        iter++)
	{
		const Pattern pattern = iter->first;
		const IndexedData data = iter->second;

		double value = collectionIndexedModel.occurrencecount(pattern);
		std::cout << ">" << pattern.tostring(*collectionClassDecoderPtr) << "," << value << std::endl;

	}

	TFIDF vsm = TFIDF(collectionIndexedModel, collectionClassDecoderPtr);

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
			std::cout << docItr->toString(featItr) << "[" << vsm.getTFIDF(featItr->first, *docItr) << "]" << std::endl;
		}
		std::cout << std::endl;
	}

	std::cout << "ALS EEN REIGER" << std::endl;
}
