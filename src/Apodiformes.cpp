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
        return pow(10, -1.0/dInstances * sum); // 10^(−1÷4×−5.80288)
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
        bool debug = false;

	std::cout << "STRAK" << std::endl;

	const std::string inputDirectory = "/scratch/lonrust/apodiformes/input/";
	const std::string generatedDirectory = "/scratch/lonrust/apodiformes/generated/";
	const std::string collectionName = "apodiformes-aiw";
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
            trainInputFiles.push_back(TrainFile("aiw", "train", inputDirectory));

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
            kneserNeyPtr->recursiveComputeFrequencyStats();
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
        std::vector<TestFile> testInputFiles = std::vector<TestFile>();
	testInputFiles.push_back(TestFile("aiw", "test", inputDirectory));

        int numberOfTestPatterns = 0;
        int numberOfOOV = 0;
        double totalLogProb = 0.0;

        for(const auto& testFile: testInputFiles)
        {
            // bool allowunknown, bool autoaddunknown=false, bool append=false, bool quiet=false);
            collectionClassEncoderPtr->encodefile(testFile.getPath(), "TESTING.dat", 1, 0, 0, 1);
        }
        collectionClassEncoderPtr->save("TESTING.cls");
        collectionClassDecoderPtr->load("TESTING.cls");

        for(const auto& testFile: testInputFiles)
        {
            std::ifstream file(testFile.getPath());

            std::string retrievedString;
            while(std::getline(file, retrievedString))
            {
                std::vector<std::string> words = split(retrievedString);

                if(words.size() < options.MAXLENGTH)
                {
                    //
                } else
                {
for(int i = 1/*(options.MAXLENGTH-1)*/; i < words.size(); ++i)
     {
         Pattern context;
         Pattern focus;
         double prob;

         if(i < options.MAXLENGTH-1)
         {
             if(debug) std::cout << "%%%%%%%%%%\n%%%    %%%\n%%%%%%%%%%" << std::endl;
             std::stringstream contextStream;
             if(debug) std::cout << "Adding BOS" << std::endl;
             contextStream << "<s> ";
             for(int ii = 1; ii < i; ++ii)
             {
                 contextStream << " " << words[ii];
             }   
             context = collectionClassEncoderPtr->buildpattern(contextStream.str());
             focus = collectionClassEncoderPtr->buildpattern(words[i]);
             prob = kneserNeyPtr->pknFromLevel(context.size()+1, context+focus, focus, context,debug);
         } else                
         {
                if(debug) std::cout << std::endl;

                std::stringstream contextStream;
                if(debug) std::cout << " STRING: ";

                // bool allowunknown=false, bool autoaddunknown = false
                if(debug) std::cout << words[i-(options.MAXLENGTH-1)] << " [" << collectionClassEncoderPtr->buildpattern(words[i-(options.MAXLENGTH-1)], 1, 0).tostring(*collectionClassDecoderPtr) << "] ";
                contextStream << words[i-(options.MAXLENGTH-1)];
                for(int ii= 1; ii < (options.MAXLENGTH-1); ++ii)
                {
                     if(debug) std::cout << words[i-(options.MAXLENGTH-1)+ii] << " [" << collectionClassEncoderPtr->buildpattern(words[i-(options.MAXLENGTH-1)+ii], 1, 0).tostring(*collectionClassDecoderPtr) << "] ";
                     contextStream << " " << words[i-(options.MAXLENGTH-1)+ii];
                }

                if(debug) std::cout << std::endl << contextStream.str() << " " << words[i] << std::endl;

                context  = collectionClassEncoderPtr->buildpattern(contextStream.str(), 1, 0);
                focus = collectionClassEncoderPtr->buildpattern(words[i]);

                prob = kneserNeyPtr->pkn(context+focus, focus, context,debug);

                        }
                        ++numberOfTestPatterns;
                        double logProb = log10(prob);
                        std::string focusString = focus.tostring(*collectionClassDecoderPtr);
                        if(kneserNeyPtr->isOOVWord(focus))
                        {
                            focusString = words[i];
                            
                            std::cout << "*** ";
                            ++numberOfOOV;
                            prob = 0.0;
                            logProb = 0.0;
                        }
                        totalLogProb += logProb;
                        //std::cout << "p(" << focus.tostring(*collectionClassDecoderPtr) << "|";
                        std::cout << "p(" << focusString << "|";
                        std::cout << context.tostring(*collectionClassDecoderPtr) << "): ";
                        std::cout << prob << " [" << logProb << "]" << std::endl;
                    }
                }
            }
        }
	std::cout << "Perplexity is " << perplexity(totalLogProb, numberOfTestPatterns,numberOfOOV) << std::endl;
	std::cout << "ALS EEN REIGER" << std::endl;
}


