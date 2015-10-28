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

#include "KneserNey.h"
#include "File.h"

#include <glog/logging.h>
#include <fstream>

#include "Common.h"

double perplexity(double sum, int instances)
{
	double dInstances = (double) instances;
	return exp(-1.0*sum+dInstances*log(dInstances));
}

bool freshtrain = true;

int main(int argc, char** argv)
{

        google::InitGoogleLogging(argv[0]);

	std::cout << "STRAK" << std::endl;

	const std::string inputDirectory = "docs/alice/";
	const std::string generatedDirectory = "docs/generated/";
	const std::string collectionName = "alice";
	const std::string colibriEncoder = "~/Software/colibri-core/src/colibri-classencode";

        LOG(INFO) << "Input directory: " << inputDirectory;
        LOG(INFO) << "Output directory: " << generatedDirectory;
        LOG(INFO) << "Collection name: " << collectionName;

	PatternModelOptions options;
	options.DOREVERSEINDEX = true;
	options.DOSKIPGRAMS = false;
	options.MINTOKENS = 1;
	options.MAXLENGTH = 4;
	options.QUIET = true;

        LOG(INFO) << "Creating " << options.MINTOKENS << "-" << options.MAXLENGTH << " " << (options.DOSKIPGRAMS ? "skipgram" : "ngram") << " model";

	int indentation = 0;

	ColibriFile collectionCorpusFile = ColibriFile(collectionName, "colibri.cls", generatedDirectory,
	        ColibriFile::Type::CORPUS);
	ColibriFile collectionEncodedFile = ColibriFile(collectionName, "colibri.dat", generatedDirectory,
	        ColibriFile::Type::ENCODED); //collectionInputFileName
	ColibriFile collectionPatternFile = ColibriFile(collectionName, "colibri.pattern", generatedDirectory,
	        ColibriFile::Type::PATTERNMODEL); //collectionOutputFileName

        LOG(INFO) << "Using corpus file: " << collectionCorpusFile.getPath();
        LOG(INFO) << "Using encoded file: " << collectionEncodedFile.getPath();
        LOG(INFO) << "Using pattern file: " << collectionPatternFile.getPath();

        IndexedPatternModel<>* collectionIndexedModelPtr;
        ClassEncoder* collectionClassEncoderPtr;
        ClassDecoder* collectionClassDecoderPtr;
        KneserNey* kneserNeyPtr;


        if(freshtrain)
        {
	    std::vector<TrainFile> trainInputFiles = std::vector<TrainFile>();
            trainInputFiles.push_back(TrainFile("train", "tok", inputDirectory));

            std::string allFileNames;
            BOOST_FOREACH( TrainFile f, trainInputFiles) // generate a list of all file names
            {	
                allFileNames += f.getPath() + " ";
                LOG(INFO) << "Adding to be processed: " << f.getPath();
            }

            LOG(INFO) <<  "+ Creating collection files";
            LOG(INFO) <<  "Class encoding collection files...";

            std::string clearGeneratedFiles = std::string("/bin/rm ") + generatedDirectory + "*";
            LOG(INFO) << "Executing command: " << clearGeneratedFiles;
            system(clearGeneratedFiles.c_str());

            std::string collectionClassEncodeCommand = colibriEncoder + " -d " + generatedDirectory + " -o "
                + collectionCorpusFile.getFileName(false) + " -u " + allFileNames;
            LOG(INFO) << "Executing command: " << collectionClassEncodeCommand;
            system(collectionClassEncodeCommand.c_str());

	    collectionClassEncoderPtr = new ClassEncoder(collectionCorpusFile.getPath());
	    collectionClassDecoderPtr = new ClassDecoder(collectionCorpusFile.getPath());
            collectionIndexedModelPtr = new IndexedPatternModel<>();
            
            LOG(INFO) << "Indexing collection";
            collectionIndexedModelPtr->train(collectionEncodedFile.getPath(), options);
            collectionIndexedModelPtr->write(collectionPatternFile.getPath());

            LOG(INFO) <<  "- Creating collection files";



            BOOST_FOREACH( TrainFile tf, trainInputFiles )
            {
                    LOG(INFO) << "+ " << tf.getPath();
                    LOG(INFO) << "+ Encoding document";
                    ColibriFile documentCorpusFile = ColibriFile(tf.getFileName(false), "colibri.cls"
                        , generatedDirectory, ColibriFile::Type::CORPUS); //documentClassFile
                    ColibriFile documentEncodedFile = ColibriFile(tf.getFileName(true), "colibri.dat"
                        , generatedDirectory, ColibriFile::Type::ENCODED);//inputFileName
                    ColibriFile documentPatternFile = ColibriFile(tf.getFileName(false), "colibri.pattern"
                        , generatedDirectory, ColibriFile::Type::PATTERNMODEL);


                    const std::string command = colibriEncoder + " -d " + generatedDirectory + " -o " 
                        + documentCorpusFile.getFileName(false) + " -c " + collectionCorpusFile.getPath() 
                        + " " + tf.getPath();
                    LOG(INFO) << "Executing command: " << command;
                    system( command.c_str() );
                    LOG(INFO) << "- Encoding done";


                    LOG(INFO) << "+ Training on file: " << documentEncodedFile.getPath();
                    IndexedPatternModel<> documentModel;
                    documentModel.train(documentEncodedFile.getPath(), options);
                    LOG(INFO) << "- Training on file";


                    LOG(INFO) << "- " << tf.getPath();
            }
            LOG(INFO) << "- Processing training files";


            LOG(INFO) << "+ Computing frequency stats for KN";
            kneserNeyPtr->recursiveComputeFrequencyStats(indentation+1);
            kneserNeyPtr->recursiveComputeAllN(indentation+1);
            LOG(INFO) << "- Computing frequency stats for KN";
        }

/*






	kneserNeyPtr = new KneserNey(collectionIndexedModelPtr, collectionClassDecoderPtr);
}  else 
{

	collectionClassEncoderPtr = new ClassEncoder(collectionCorpusFile.getPath());
	collectionClassDecoderPtr = new ClassDecoder(collectionCorpusFile.getPath());
        collectionIndexedModelPtr = new IndexedPatternModel<>(collectionPatternFile.getPath(), options); 


        kneserNeyPtr = KneserNeyFactory::readFromFile("kneserney.out", collectionIndexedModelPtr, collectionClassDecoderPtr);

}

        std::vector<TestFile> testInputFiles = std::vector<TestFile>();
	testInputFiles.push_back(TestFile("test", "tok", inputDirectory));






//	// ##################################################    Training
//	LOG(INFO) << indent(indentation++) << "+ Processing training files";
//
//	int docCntr = 0;
//
//        trainLanguageModel.Something(indentation);
        kneserNeyPtr->doSomething(indentation);
        //delete collectionClassDecoderPtr;


//      KneserNeyFactory::writeToFile(trainLanguageModel, "kneserney.out", collectionClassDecoderPtr);
//      if(collectionClassDecoderPtr == nullptr) std::cout << "Kankerpointer" << std::endl;
//      KneserNeyFactory::readFromFile("kneserney.out", collectionIndexedModelPtr, collectionClassDecoderPtr);

//	// ##################################################    Testing
//	std::cout << indent(indentation++) << "+ Processing testing files" << std::endl;
//
//	double corpusProbability = 0;
//	int numberOfTestPatterns = 0;
//
//	docCntr = 0;
//	BOOST_FOREACH( TestFile tf, testInputFiles )
//	{
//
//		std::cout << indent(indentation++) << "+ " << tf.getPath() << std::endl;
//		std::cout << indent(indentation) << "+ Encoding document" << std::endl;
//		Document document = Document(docCntr++, tf.getPath(), collectionClassDecoderPtr);
//
//		ColibriFile documentCorpusFile = ColibriFile(tf.getFileName(false), "colibri.cls", generatedDirectory, ColibriFile::Type::CORPUS); //documentClassFile
//		ColibriFile documentEncodedFile = ColibriFile(tf.getFileName(true), "colibri.dat", generatedDirectory, ColibriFile::Type::ENCODED);//inputFileName
//		ColibriFile documentPatternFile = ColibriFile(tf.getFileName(false), "colibri.pattern", generatedDirectory, ColibriFile::Type::PATTERNMODEL);
//
//		const std::string command = colibriEncoder + " -U" + " -d " + generatedDirectory + " -o " + documentCorpusFile.getFileName(false) + " -c " + collectionCorpusFile.getPath() + " " + tf.getPath();
//		std::cout << indent(indentation) << "Executing command: " << command << std::endl;
//		system( command.c_str() );
//		std::cout << indent(indentation) << "- Encoding done" << std::endl;
//
//		std::cout << indent(indentation) << "+ Testing on file: " << documentEncodedFile.getPath() << std::endl;
//		IndexedPatternModel<> documentModel;
//		documentModel.train(documentEncodedFile.getPath(), options);
//		std::cout << indent(indentation) << "- Testing on file" << std::endl;
//
//		std::cout << indent(indentation) << "Iterating over all patterns" << std::endl;
//		double documentProbability = 0.0;
//		int numberPatternsInDocument = 0;
//                for(const auto& iter: documentModel)
//		{
//			const Pattern pattern = iter.first;
//
//                        if(trainLanguageModel.isOOV(pattern))
//                        {
//                            std::cout << indent(indentation+1) << "***" << pattern.tostring(*collectionClassDecoderPtr) << std::endl;
//                        } else
//                        {
//                            ++numberOfTestPatterns;
//                            ++numberPatternsInDocument;
//                            //double patternProbability = log(trainLanguageModel.getSmoothedValue(pattern, indentation+1));
//                            double patternProbability = trainLanguageModel.pkn(pattern);
//                            std::cout << "pkn: " << patternProbability << std::endl;
//
//                            documentProbability += patternProbability;
//                            corpusProbability += documentProbability;
//                            std::cout << indent(indentation+1) << "log probability: " << patternProbability << " Perplexity: " << perplexity(patternProbability, 1) << " document perplexity(" << perplexity(documentProbability,numberPatternsInDocument) << ")" << std::endl;
//                        }
//		}
//
//		std::cout  << "Document perplexity: " << perplexity(documentProbability,numberPatternsInDocument) << std::endl;
//
//		std::cout << indent(--indentation) << "- " << tf.getPath() << std::endl;
//	}
//	std::cout << indent(--indentation) << "- Processing testing files" << std::endl;
//
//
//
//
//	std::cout << "Perplexity is " << perplexity(corpusProbability, double(numberOfTestPatterns)) << std::endl;



*/


	std::cout << "ALS EEN REIGER" << std::endl;
}


