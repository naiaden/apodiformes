/*
 * Apodiformes.cpp
 *
 *  Created on: Oct 24, 2013
 *      Author: lonrust
 */

#include <iostream>

#include <string>
#include <iostream>
#include <fstream>

#include <math.h>

#include <classencoder.h>
#include <classdecoder.h>
#include <patternmodel.h>

#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string/join.hpp>

#include "VectorSpaceModel.h"
#include "KneserNey.h"
#include "TFIDF.h"
#include "File.h"

#include <glog/logging.h>

#include "Common.h"

double perplexity(double sum, int instances)
{
	double dInstances = (double) instances;
	return exp(-1.0*sum+dInstances*log(dInstances));
}

int main(int argc, char** argv)
{

        google::InitGoogleLogging(argv[0]);

	std::cout << "STRAK" << std::endl;

	const std::string inputDirectory = "docs/alice/";
	const std::string generatedDirectory = "docs/generated/";
	const std::string collectionName = "alice";
	const std::string colibriEncoder = "~/Software/colibri-core/src/colibri-classencode";

	std::vector<TrainFile> trainInputFiles = std::vector<TrainFile>();
	trainInputFiles.push_back(TrainFile("train", "tok", inputDirectory));

	std::vector<TestFile> testInputFiles = std::vector<TestFile>();
	testInputFiles.push_back(TestFile("test", "tok", inputDirectory));

	std::string allFileNames;
	BOOST_FOREACH( TrainFile f, trainInputFiles) // generate a list of all file names
        {	
            allFileNames += f.getPath() + " ";
        }

	PatternModelOptions options;
	options.DOREVERSEINDEX = true;
	options.DOSKIPGRAMS = false;
	options.MINTOKENS = 1;
	options.MAXLENGTH = 5;
	options.QUIET = true;

	int indentation = 0;

	LOG(INFO) << indent(indentation++) << "+ Creating collection files";
	LOG(INFO) << indent(indentation) << "Class encoding collection files...";

	ColibriFile collectionCorpusFile = ColibriFile(collectionName, "colibri.cls", generatedDirectory,
	        ColibriFile::Type::CORPUS);
	ColibriFile collectionEncodedFile = ColibriFile(collectionName, "colibri.dat", generatedDirectory,
	        ColibriFile::Type::ENCODED); //collectionInputFileName
	ColibriFile collectionPatternFile = ColibriFile(collectionName, "colibri.pattern", generatedDirectory,
	        ColibriFile::Type::PATTERNMODEL); //collectionOutputFileName

	std::string clearGeneratedFiles = std::string("/bin/rm ") + generatedDirectory + "*";
	LOG(INFO) << indent(indentation) << "Executing command: " << clearGeneratedFiles;
	system(clearGeneratedFiles.c_str());

	std::string collectionClassEncodeCommand = colibriEncoder + " -d " + generatedDirectory + " -o "
	        + collectionCorpusFile.getFileName(false) + " -u " + allFileNames;
	LOG(INFO) << indent(indentation) << "Executing command: " << collectionClassEncodeCommand;
	system(collectionClassEncodeCommand.c_str());

	ClassEncoder collectionClassEncoder = ClassEncoder(collectionCorpusFile.getPath());
	boost::shared_ptr<ClassDecoder> collectionClassDecoderPtr(new ClassDecoder(collectionCorpusFile.getPath()));

	IndexedPatternModel<> collectionIndexedModel;
	LOG(INFO) << indent(indentation) << "Indexing collection";
	collectionIndexedModel.train(collectionEncodedFile.getPath(), options);
        collectionIndexedModel.write(collectionPatternFile.getPath());

	LOG(INFO) << indent(--indentation) << "- Creating collection files";

	KneserNey trainLanguageModel = KneserNey(collectionIndexedModel, collectionClassDecoderPtr);


	// ##################################################    Training
	LOG(INFO) << indent(indentation++) << "+ Processing training files";

	int docCntr = 0;
	BOOST_FOREACH( TrainFile tf, trainInputFiles )
	{
		LOG(INFO) << indent(indentation++) << "+ " << tf.getPath();
		LOG(INFO) << indent(indentation) << "+ Encoding document";
		Document document = Document(docCntr++, tf.getPath(), collectionClassDecoderPtr);

		ColibriFile documentCorpusFile = ColibriFile(tf.getFileName(false), "colibri.cls", generatedDirectory, ColibriFile::Type::CORPUS); //documentClassFile
		ColibriFile documentEncodedFile = ColibriFile(tf.getFileName(true), "colibri.dat", generatedDirectory, ColibriFile::Type::ENCODED);//inputFileName
		ColibriFile documentPatternFile = ColibriFile(tf.getFileName(false), "colibri.pattern", generatedDirectory, ColibriFile::Type::PATTERNMODEL);

		const std::string command = colibriEncoder + " -d " + generatedDirectory + " -o " + documentCorpusFile.getFileName(false) + " -c " + collectionCorpusFile.getPath() + " " + tf.getPath();
		LOG(INFO) << indent(indentation) << "Executing command: " << command;
		system( command.c_str() );
		LOG(INFO) << indent(indentation) << "- Encoding done";

		LOG(INFO) << indent(indentation) << "+ Training on file: " << documentEncodedFile.getPath();
		IndexedPatternModel<> documentModel;
		documentModel.train(documentEncodedFile.getPath(), options);
		LOG(INFO) << indent(indentation) << "- Training on file";

		LOG(INFO) << indent(indentation) << "Iterating over all patterns";
		for (IndexedPatternModel<>::iterator iter = documentModel.begin(); iter != documentModel.end(); iter++)
		{
			const Pattern pattern = iter->first;
			const IndexedData data = iter->second;

			double value = documentModel.occurrencecount(pattern);

			document.updateValue(pattern, value);
		}

		trainLanguageModel.addDocument(document);

		LOG(INFO) << indent(--indentation) << "- " << tf.getPath();
	}
	LOG(INFO) << indent(--indentation) << "- Processing training files";

	LOG(INFO) << indent(indentation) << "+ Computing frequency stats for KN";
	trainLanguageModel.computeFrequencyStats();
	LOG(INFO) << indent(indentation) << "- Computing frequency stats for KN";


/*
	// ##################################################    Testing
	std::cout << indent(indentation++) << "+ Processing testing files" << std::endl;

	double corpusProbability = 0;
	int numberOfTestPatterns = 0;

	docCntr = 0;
	BOOST_FOREACH( TestFile tf, testInputFiles )
	{

		std::cout << indent(indentation++) << "+ " << tf.getPath() << std::endl;
		std::cout << indent(indentation) << "+ Encoding document" << std::endl;
		Document document = Document(docCntr++, tf.getPath(), collectionClassDecoderPtr);

		ColibriFile documentCorpusFile = ColibriFile(tf.getFileName(false), "colibri.cls", generatedDirectory, ColibriFile::Type::CORPUS); //documentClassFile
		ColibriFile documentEncodedFile = ColibriFile(tf.getFileName(true), "colibri.dat", generatedDirectory, ColibriFile::Type::ENCODED);//inputFileName
		ColibriFile documentPatternFile = ColibriFile(tf.getFileName(false), "colibri.pattern", generatedDirectory, ColibriFile::Type::PATTERNMODEL);

		const std::string command = colibriEncoder + " -U" + " -d " + generatedDirectory + " -o " + documentCorpusFile.getFileName(false) + " -c " + collectionCorpusFile.getPath() + " " + tf.getPath();
		std::cout << indent(indentation) << "Executing command: " << command << std::endl;
		system( command.c_str() );
		std::cout << indent(indentation) << "- Encoding done" << std::endl;

		std::cout << indent(indentation) << "+ Testing on file: " << documentEncodedFile.getPath() << std::endl;
		IndexedPatternModel<> documentModel;
		documentModel.train(documentEncodedFile.getPath(), options);
		std::cout << indent(indentation) << "- Testing on file" << std::endl;

		std::cout << indent(indentation) << "Iterating over all patterns" << std::endl;
		double documentProbability = 0.0;
		int numberPatternsInDocument = 0;
		for (IndexedPatternModel<>::iterator iter = documentModel.begin(); iter != documentModel.end(); iter++)
		{
			const Pattern pattern = iter->first;

			++numberOfTestPatterns;
			++numberPatternsInDocument;
			double patternProbability = log(trainLanguageModel.getSmoothedValue(pattern, indentation+1));

			documentProbability += patternProbability;
			corpusProbability += documentProbability;
			std::cout << indent(indentation+1) << "log probability: " << patternProbability << " Perplexity: " << perplexity(patternProbability, 1) << " document perplexity(" << perplexity(documentProbability,numberPatternsInDocument) << ")" << std::endl;
		}

		std::cout  << "Document perplexity: " << perplexity(documentProbability,numberPatternsInDocument) << std::endl;

		std::cout << indent(--indentation) << "- " << tf.getPath() << std::endl;
	}
	std::cout << indent(--indentation) << "- Processing testing files" << std::endl;




	std::cout << "Perplexity is " << perplexity(corpusProbability, double(numberOfTestPatterns)) << std::endl;
*/





	std::cout << "ALS EEN REIGER" << std::endl;
}


