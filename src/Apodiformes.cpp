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

#include <sstream>
#include <iomanip>

double perplexity(double sum, int instances, int numberOfOOV)
{
        std::cout << "S:" << sum << " #:" << instances << " O:" << numberOfOOV << std::endl;
	double dInstances = (double) (instances-numberOfOOV);
	return exp(-1.0*sum+dInstances*log(dInstances));
}

std::vector<std::string> split(std::string const &input) { 
    std::istringstream buffer(input);
    std::vector<std::string> ret{std::istream_iterator<std::string>(buffer), 
                                 std::istream_iterator<std::string>()};
    return ret;
}

bool freshtrain = true;

int main(int argc, char** argv)
{

        google::InitGoogleLogging(argv[0]);
        bool debug = true;

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

	    kneserNeyPtr = new KneserNey(collectionIndexedModelPtr, collectionClassDecoderPtr);

            LOG(INFO) << "+ Computing frequency stats for KN";
            kneserNeyPtr->recursivePrecomputeContextValues();
 //           kneserNeyPtr->recursiveComputeFrequencyStats();
 //           kneserNeyPtr->recursiveComputeAllN(collectionClassEncoderPtr->buildpattern(". '"));
            LOG(INFO) << "- Computing frequency stats for KN";


            LOG(INFO) << "Writing Kneser Ney model to file";
//            KneserNeyFactory::writeToFile(*kneserNeyPtr, "alice-kneserney.out", collectionClassDecoderPtr);
//        } else 
//        {
//            collectionClassEncoderPtr = new ClassEncoder(collectionCorpusFile.getPath());
//            collectionClassDecoderPtr = new ClassDecoder(collectionCorpusFile.getPath());
//            collectionIndexedModelPtr = new IndexedPatternModel<>(collectionPatternFile.getPath(), options); 
//
//            LOG(INFO) << "Reading Kneser Ney model from file";
//            kneserNeyPtr = KneserNeyFactory::readFromFile("alice-kneserney.out", collectionIndexedModelPtr, collectionClassDecoderPtr);
        }

//        LOG(INFO) << "Processing testing files";
//        std::vector<TestFile> testInputFiles = std::vector<TestFile>();
//	testInputFiles.push_back(TestFile("test1", "tok", inputDirectory));
//
//        int numberOfTestPatterns = 0;
//        int numberOfOOV = 0;
//        double totalProbs = 0.0;
//
//        for(const auto& testFile: testInputFiles)
//        {
//            collectionClassEncoderPtr->encodefile(testFile.getPath(), "TESTING.dat", 1, 1, 0, 1);
//        }
//        collectionClassEncoderPtr->save("TESTING.cls");
//        collectionClassDecoderPtr->load("TESTING.cls");
//
//        for(const auto& testFile: testInputFiles)
//        {
//            std::ifstream file(testFile.getPath());
//
//            std::string retrievedString;
//            while(std::getline(file, retrievedString))
//            {
//                std::vector<std::string> words = split(retrievedString);
//
//                if(words.size() < options.MAXLENGTH)
//                {
//                    //
//                } else
//                {
//                    for(int i = (options.MAXLENGTH-1); i < words.size(); ++i)
//                    {
//                        std::stringstream contextStream;
//                        contextStream << words[i-(options.MAXLENGTH-1)];
//                        for(int ii= 1; ii < (options.MAXLENGTH-1); ++ii)
//                        {
//                            contextStream << " " << words[i-(options.MAXLENGTH-1)+ii];
//                        }
//
//                        Pattern context  = collectionClassEncoderPtr->buildpattern(contextStream.str());
//                        Pattern focus = collectionClassEncoderPtr->buildpattern(words[i]);
//
//                        double prob = kneserNeyPtr->pkn(context+focus,debug);
//
//                        ++numberOfTestPatterns;
//                        if(kneserNeyPtr->isOOVWord(focus))
//                        {
//                            std::cout << "*** ";
//                            ++numberOfOOV;
//                            prob = 0.0;
//                        }
//                        totalProbs += prob;
//                        std::cout << "p(" << focus.tostring(*collectionClassDecoderPtr) << "|";
//                        std::cout << context.tostring(*collectionClassDecoderPtr) << "): ";
//                        std::cout << prob << std::endl;
//                    }
//                }
//            }
//        }
//
//	std::cout << "Perplexity is " << perplexity(totalProbs, numberOfTestPatterns,numberOfOOV) << std::endl;
//        
/*


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



*/


	std::cout << "ALS EEN REIGER" << std::endl;
}


