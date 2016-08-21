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

#include <map>

#include "KneserNey.h"
#include "File.h"

#include <glog/logging.h>
#include <fstream>

#include "Common.h"
#include "CommandLineOptions.h"
#include "Writers.h"

#include <sstream>
#include <iomanip>
#include <stdlib.h>



int main(int argc, char** argv)
{
        google::InitGoogleLogging(argv[0]);

	std::cout << "STRAK" << std::endl;

        CommandLineOptions clo(argc, argv);
	PatternModelOptions options = clo.getPatternModelOptions();


	std::string collectionCorpusFile = clo.getVocabularyFile();
	if(collectionCorpusFile.empty())
	{
		collectionCorpusFile = clo.getOutputDirectory() + "/" + clo.getCollectionName() + ".coco.cls";
	}

	std::string collectionEncodedFile = clo.getCorpusFile();
	if(collectionEncodedFile.empty())
	{
		collectionEncodedFile = clo.getOutputDirectory() + "/" + clo.getCollectionName() + ".coco.dat";
	}

	std::string collectionPatternFile = clo.getCollectionName();
	if(collectionPatternFile.empty())
	{
		collectionPatternFile = clo.getOutputDirectory() + "/" + clo.getCollectionName() + ".coco.model";
	}

	std::string modelFile = clo.getCollectionName();
	 if(modelFile.empty()) 
	{
		 modelFile = clo.getCollectionName() + ".mkn." + std::to_string(clo.getMaxLength()) + ".model";
	}

        LOG(INFO) << "Using corpus file: " << collectionCorpusFile;
        LOG(INFO) << "Using encoded file: " << collectionEncodedFile;
        LOG(INFO) << "Using pattern file: " << collectionPatternFile;

        IndexedPatternModel<>* collectionIndexedModelPtr;
        ClassEncoder* collectionClassEncoderPtr;
        ClassDecoder* collectionClassDecoderPtr;
        KneserNey* kneserNeyPtr;


        if(clo.getFreshRun())
        {
            std::cout << "Processing " << clo.getInputFiles().size() << " files" << std::endl;

		collectionClassEncoderPtr = new ClassEncoder();
		std::vector<std::string> inputFiles = clo.getInputFiles();
		LOG(INFO) << "+ Encoding collection files";
		collectionClassEncoderPtr->build(inputFiles, true, 0); // 0=threshold
		LOG(INFO) << "- Encoding collection files";
		LOG(INFO) << "+ Save collection files to " << collectionCorpusFile;
		collectionClassEncoderPtr->save(collectionCorpusFile);
		LOG(INFO) << "- Save collection files";

		LOG(INFO) << "+ Encode files to " << collectionEncodedFile;
		for(auto f: clo.getInputFiles())
		{
			collectionClassEncoderPtr->encodefile(f, collectionEncodedFile, false, false, false);
		}
		LOG(INFO) << "- Encode files";

	    collectionClassDecoderPtr = new ClassDecoder(collectionCorpusFile);
            collectionIndexedModelPtr = new IndexedPatternModel<>();
            
            LOG(INFO) << "+ Indexing collection";
            collectionIndexedModelPtr->train(collectionEncodedFile, options);
		LOG(INFO) << "- Indexing collection";
		LOG(INFO) << "+ Write pattern file to " << collectionPatternFile;
            collectionIndexedModelPtr->write(collectionPatternFile);
		LOG(INFO) << "- Write pattern file";

	    kneserNeyPtr = new KneserNey(clo.getMaxLength(), collectionIndexedModelPtr, collectionClassDecoderPtr);

            LOG(INFO) << "+ Computing frequency stats for KN";
            kneserNeyPtr->recursivePrecomputeContextValues();
            kneserNeyPtr->recursiveComputeFrequencyStats();
            LOG(INFO) << "- Computing frequency stats for KN";

		LOG(INFO) << "+ Computing coco coverage stats";
	    collectionIndexedModelPtr->computecoveragestats(0,1);
		LOG(INFO) << "- Computing coco coverage stats";
	
            LOG(INFO) << "+ Writing Kneser Ney model to file " << modelFile;
            KneserNeyFactory::writeToFile(*kneserNeyPtr, modelFile, collectionClassDecoderPtr);
		LOG(INFO) << "- Writing Kneser Ney model to file";
        } else // not so fresh run
        {
            collectionClassEncoderPtr = new ClassEncoder(collectionCorpusFile);
            collectionClassDecoderPtr = new ClassDecoder(collectionCorpusFile);
            collectionIndexedModelPtr = new IndexedPatternModel<>(collectionPatternFile, options); 


            LOG(INFO) << "+ Reading Kneser Ney model from file " << modelFile;
            kneserNeyPtr = KneserNeyFactory::readFromFile(modelFile, collectionIndexedModelPtr, collectionClassDecoderPtr);
		LOG(INFO) << "- Reading Kneser Ney model";
        }

//        LOG(INFO) << "Processing testing files";

        int numberOfTestPatterns = 0;
        int numberOfOOV = 0;
        double totalLogProb = 0.0;

	{ 
		std::string testOutputDatFile = clo.getOutputDirectory() + "/" + clo.getCollectionName() + ".test.coco.dat";
		std::string testOutputClsFile = clo.getOutputDirectory() + "/" + clo.getCollectionName() + ".test.coco.cls";
	
		LOG(INFO) << "+ Encoding test files into " << testOutputDatFile;
		for(const auto& f: clo.getTestFiles())
		{
		    collectionClassEncoderPtr->encodefile(f, testOutputDatFile, 1, 0, 0, 1);
		}
		LOG(INFO) << "- Encoding test files";
		LOG(INFO) << "+ Writing and loading class encoder from and to " << testOutputClsFile;
		collectionClassEncoderPtr->save(testOutputClsFile);
		collectionClassDecoderPtr->load(testOutputClsFile);
		LOG(INFO) << "- Writing and loading class encoder";
	}	

	Writer* probsOut = nullptr;
	if(clo.getWriteProbsFile().empty())
	{
		probsOut = new ScreenWriter();
		LOG(INFO) << "  Writing probs to screen";
	} else 
	{
		probsOut = new FileWriter(clo.getWriteProbsFile());
		LOG(INFO) << "  Writing probs to " << clo.getWriteProbsFile();
	}

        for(const auto& f: clo.getTestFiles())
        {
            std::ifstream file(f);

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
             if(clo.getDebug()) std::cout << "%%%%%%%%%%\n%%%    %%%\n%%%%%%%%%%" << std::endl;
             std::stringstream contextStream;
             if(clo.getDebug()) std::cout << "Adding BOS" << std::endl;
             contextStream << "<s> ";
             for(int ii = 1; ii < i; ++ii)
             {
                 contextStream << " " << words[ii];
             }   
             context = collectionClassEncoderPtr->buildpattern(contextStream.str());
             focus = collectionClassEncoderPtr->buildpattern(words[i]);
             prob = kneserNeyPtr->pknFromLevel(context.size()+1, context+focus, focus, context,clo.getDebug());
         } else                
         {
                if(clo.getDebug()) std::cout << std::endl;

                std::stringstream contextStream;
                if(clo.getDebug()) std::cout << " STRING: ";

                if(clo.getDebug()) std::cout << words[i-(options.MAXLENGTH-1)] << " [" << collectionClassEncoderPtr->buildpattern(words[i-(options.MAXLENGTH-1)], 1, 0).tostring(*collectionClassDecoderPtr) << "] ";
                contextStream << words[i-(options.MAXLENGTH-1)];
                for(int ii= 1; ii < (options.MAXLENGTH-1); ++ii)
                {
                     if(clo.getDebug()) std::cout << words[i-(options.MAXLENGTH-1)+ii] << " [" << collectionClassEncoderPtr->buildpattern(words[i-(options.MAXLENGTH-1)+ii], 1, 0).tostring(*collectionClassDecoderPtr) << "] ";
                     contextStream << " " << words[i-(options.MAXLENGTH-1)+ii];
                }

                if(clo.getDebug()) std::cout << std::endl << contextStream.str() << " " << words[i] << std::endl;

                context  = collectionClassEncoderPtr->buildpattern(contextStream.str(), 1, 0);
                focus = collectionClassEncoderPtr->buildpattern(words[i]);

                prob = kneserNeyPtr->pkn(context+focus, focus, context,clo.getDebug());

                        }
                        ++numberOfTestPatterns;
                        double logProb = log10(prob);
                        std::string focusString = focus.tostring(*collectionClassDecoderPtr);
                        if(kneserNeyPtr->isOOVWord(focus))
                        {
                            focusString = words[i];
                            
                            probsOut->write("*** ");
                            ++numberOfOOV;
                            prob = 0.0;
                            logProb = 0.0;
                        }
                        totalLogProb += logProb;
                        probsOut->write("p(" + focusString + "|");
                        probsOut->write(context.tostring(*collectionClassDecoderPtr) + "): ");
                        probsOut->write(std::to_string(prob) + " [" + std::to_string(logProb) + "]", true);
                    }
                }
            }
        }
	std::cout << "Perplexity is " << perplexity(totalLogProb, numberOfTestPatterns,numberOfOOV) << std::endl;
	std::cout << "ALS EEN REIGER" << std::endl;
}


