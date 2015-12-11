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

#include <sstream>
#include <iomanip>
#include <stdlib.h>

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

    std::vector<std::string> tokenizer( const std::string& p_pcstStr, char delim )  {
        std::vector<std::string> tokens;
        std::stringstream   mySstream( p_pcstStr );
        std::string         temp;

        while( getline( mySstream, temp, delim ) ) {
            tokens.push_back( temp );
        }

        return tokens;
    }

inline std::string trim(const std::string &s)
{
   auto  wsfront=std::find_if_not(s.begin(),s.end(),[](int c){return std::isspace(c);});
      return std::string(wsfront,std::find_if_not(s.rbegin(),std::string::const_reverse_iterator(wsfront),[](int c){return std::isspace(c);}).base());
}

class CommandLineOptions
{
    private:
    std::string applicationName = "Apodiformes";

    std::string corpusFile = "";
    std::string vocabularyFile = "";
    std::string patternModelFile = "";

    std::string inputDirectory = "/scratch/lonrust/apodiformes/input/";
    std::string outputDirectory = "/scratch/lonrust/apodiformes/generated/";
    std::string collectionName = "";
    std::string pathToColibri = "~/Software/colibri-core/src/colibri-classencode";

    bool reverseIndex = true;
    bool doSkipgrams = false;
    int minTokens = 1;
    int maxLength = 4;

    bool debug = false;
    bool overwriteFiles = false;
    bool freshRun = true;
   
    std::vector<std::string> inputFiles;
    std::vector<std::string> testFiles;

    public:
    std::string getCorpusFile() { return corpusFile; }
    std::string getVocabularyFile() { return vocabularyFile; }
    std::string getPatternModelFile() { return patternModelFile; }

    std::string getInputDirectory() { return inputDirectory; }
    std::string getOutputDirectory() { return outputDirectory; }
    std::string getCollectionName() { return collectionName; }
    std::string getPathToColibri() { return pathToColibri; }

    bool getReverseIndex() { return reverseIndex; }
    bool getDoSkipgrams() { return doSkipgrams; }
    int getMinTokens() { return minTokens; }
    int getMaxLength() { return maxLength; }

    bool getDebug() { return debug; }
    bool getOverwriteFiles() { return overwriteFiles; }
    bool getFreshRun() { return freshRun; }

    std::vector<std::string> getInputFiles() { return inputFiles; }
    std::vector<std::string> getTestFiles() { return testFiles; }

    PatternModelOptions getPatternModelOptions()
    {
        PatternModelOptions options;
        options.DOREVERSEINDEX = getReverseIndex();
        options.DOSKIPGRAMS = getDoSkipgrams();
        options.MINTOKENS = getMinTokens();
        options.MAXLENGTH = getMaxLength();
        options.QUIET = true;

        return options;
    }

    void printHelp()
    {
        std::cout << 
        "APODIFORMES [options] inputfiles\n"
        "--config             read long-option '=' delimited options.\n"
        "                     Command-line options overwrite config options.\n"
        "-c, --corpus         colibri corpus file\n"
        "-v, --vocabulary     colibri class file\n"
        "-p, --patternmodel   colibri pattern model file\n"
        "-I, --inputdir       input directory for text files\n"
        "-O, --outputdir      output directory for generated files\n"
        "-m, --model          output model name\n"
        "-C, --pathtocolibri  path to colibri classencoder\n"
        "--reverseindex       use reverse index (idk why you would want this)\n"
        "-S, --skipgrams      use skipgrams\n"
        "-t, --mintokens      threshold on the unigrams\n"
        "-o, --order          order of the n-gram\n"
        "-f, --fresh          go for a fresh run\n"
        "-d, --debug          show debug output\n"
        "-h, -?, --help       print this ugly shit and exit\n" << std::endl;
    }

    void parseArguments(std::string fileName)
    {
        std::ifstream config(fileName);
        for( std::string line; getline( config, line ); )
        {
            std::vector<std::string> tokens = tokenizer(line, '=');
            if(tokens.size() == 2)
            {
               std::string key = trim(tokens[0]);
               std::string value = trim(tokens[1]);

               if(!key.compare("corpus"))
               {
                    corpusFile = value;
               } else if(!key.compare("vocabulary"))
               {
                    vocabularyFile = value;
               }else if(!key.compare("patternmodel"))
               {
                    patternModelFile = value;
               } else if(!key.compare("inputdir"))
               {
                    inputDirectory = value;
               }else if(!key.compare("outputdir"))
               {
                    outputDirectory = value;
               }else if(!key.compare("model"))
               {
                    collectionName = value;
               }else if(!key.compare("pathtocolibri"))
               {
                    pathToColibri = value;
               }else if(!key.compare("reverseindex"))
               {
                    reverseIndex = value.compare("true") ? false : true;
               }else if(!key.compare("skipgrams"))
               {
                    vocabularyFile = value.compare("true") ? false : true;
               }else if(!key.compare("mintokens"))
               {
                    minTokens = atoi(value.c_str());
               }else if(!key.compare("order"))
               {
                    maxLength = atoi(value.c_str());
               }else if(!key.compare("debug"))
               {
                    debug = value.compare("true") ? false : true;
               }else if(!key.compare("freshrun"))
               {
                    freshRun = value.compare("true") ? false : true;
               }else if(!key.compare("overwrite"))
               {
                    overwriteFiles = value.compare("true") ? false : true;
               }else if(!key.compare("inputfile"))
               {
                    inputFiles.push_back(value);
               }else if(!key.compare("inputfiles"))
               {
                    for(std::string f: tokenizer(value, ','))
                    {
                        inputFiles.push_back(f);
                    }
               }else if(!key.compare("testfile"))
               {
                    testFiles.push_back(value);
               }else if(!key.compare("testfiles"))
               {
                    for(std::string f: tokenizer(value, ','))
                    {
                        testFiles.push_back(f);
                    }
               }

            } else
            {
                std::cout << "Error in line: " << line << std::endl;
            }
        }
    }

    void parseArguments(int argc, char** argv)
    {

        for(int argnr = 0; argnr < argc; ++argnr)
        {
            if(argnr == 0) 
            {
                applicationName = argv[0];
            } else if(strcmp (argv[argnr],"-h") == 0 || strcmp (argv[argnr],"--help") == 0 || strcmp (argv[argnr],"-?") == 0)
            {
                printHelp();
                exit(0);
            } else if(strcmp (argv[argnr],"-c") == 0 || strcmp (argv[argnr],"--corpus") == 0)
            {
                corpusFile = std::string(argv[++argnr]);
            } else if(strcmp (argv[argnr],"-v") == 0 || strcmp (argv[argnr],"--vocabulary") == 0)
            {
                vocabularyFile = std::string(argv[++argnr]);
            } else if(strcmp (argv[argnr],"-p") == 0 || strcmp (argv[argnr],"--patternmodel") == 0)
            {
                patternModelFile = std::string(argv[++argnr]);
            } else if(strcmp (argv[argnr],"-I") == 0 || strcmp (argv[argnr],"--inputdir") == 0)
            {
                inputDirectory = std::string(argv[++argnr]);
            } else if(strcmp (argv[argnr],"-O") == 0 || strcmp (argv[argnr],"--outputdir") == 0)
            {
                outputDirectory = std::string(argv[++argnr]);
            } else if(strcmp (argv[argnr],"-m") == 0 || strcmp (argv[argnr],"--model") == 0)
            {
                collectionName = std::string(argv[++argnr]);
            } else if(strcmp (argv[argnr],"-C") == 0 || strcmp (argv[argnr],"--pathtocolibri") == 0)
            {
                pathToColibri = std::string(argv[++argnr]);
            } else if(strcmp (argv[argnr],"--reverseindex") == 0)
            {
                reverseIndex = true;
            } else if(strcmp (argv[argnr],"-S") == 0 || strcmp (argv[argnr],"--skipgrams") == 0)
            {
                doSkipgrams = true;
            } else if(strcmp (argv[argnr],"-t") == 0 || strcmp (argv[argnr],"--mintokens") == 0)
            {
                minTokens = atoi(argv[++argnr]);
            } else if(strcmp (argv[argnr],"-o") == 0 || strcmp (argv[argnr],"--order") == 0)
            {
                maxLength = atoi(argv[++argnr]);
            }  else if(strcmp (argv[argnr],"--config") == 0)
            {
                ++argnr;
            } else if(strcmp (argv[argnr],"-f") == 0 || strcmp (argv[argnr],"--fresh") == 0)
            {
                freshRun = true;
            } else if(strcmp (argv[argnr],"-d") == 0 || strcmp (argv[argnr],"--debug") == 0)
            {
                debug = true;
            } else if(strcmp (argv[argnr],"-i") == 0 || strcmp (argv[argnr],"--inputfiles") == 0)
            {
                std::string list(argv[++argnr]);
                for(std::string s: tokenizer(list, ','))
                {
                    inputFiles.push_back(s);
                }
            } else
            {
                inputFiles.push_back(std::string(argv[argnr]));
            }
        }
    }

    CommandLineOptions(int argc, char** argv)
    {

        for(int argnr = 0; argnr < argc; ++argnr)
        {
            if(argnr == 0) 
            {
                applicationName = argv[0];
            } else if(strcmp (argv[argnr],"-h") == 0 || strcmp (argv[argnr],"--help") == 0 || strcmp (argv[argnr],"-?") == 0)
            {
                printHelp();
                exit(0);
            } else if(strcmp (argv[argnr],"--config") == 0)
            {
                parseArguments(std::string(argv[++argnr]));
            }
        }

        parseArguments(argc, argv);

        if(collectionName.empty())
        {
            std::cerr << "Provide a collection name with -m. Run again with " << applicationName << " --help for more info" << std::endl;
            exit(1);
        }
    }
};

int main(int argc, char** argv)
{
        google::InitGoogleLogging(argv[0]);

	std::cout << "STRAK" << std::endl;

        CommandLineOptions clo(argc, argv);
	PatternModelOptions options = clo.getPatternModelOptions();

        // implement support for using these files from command line options
	ColibriFile collectionCorpusFile = ColibriFile(clo.getCollectionName(), "colibri.cls", clo.getOutputDirectory(),
	        ColibriFile::Type::CORPUS);
	ColibriFile collectionEncodedFile = ColibriFile(clo.getCollectionName(), "colibri.dat", clo.getOutputDirectory(),
	        ColibriFile::Type::ENCODED); //collectionInputFileName
	ColibriFile collectionPatternFile = ColibriFile(clo.getCollectionName(), "colibri.pattern", clo.getOutputDirectory(),
	        ColibriFile::Type::PATTERNMODEL); //collectionOutputFileName

        LOG(INFO) << "Using corpus file: " << collectionCorpusFile.getPath();
        LOG(INFO) << "Using encoded file: " << collectionEncodedFile.getPath();
        LOG(INFO) << "Using pattern file: " << collectionPatternFile.getPath();

        IndexedPatternModel<>* collectionIndexedModelPtr;
        ClassEncoder* collectionClassEncoderPtr;
        ClassDecoder* collectionClassDecoderPtr;
        KneserNey* kneserNeyPtr;


        if(clo.getFreshRun())
        {
            std::cout << "Processing " << clo.getInputFiles().size() << " files" << std::endl;

            std::string allFileNames;
            BOOST_FOREACH( auto f, clo.getInputFiles()) // generate a list of all file names
            {	
                allFileNames += f + " ";
                LOG(INFO) << "Adding to be processed: " << f;
            }

            LOG(INFO) <<  "+ Creating collection files";
            LOG(INFO) <<  "Class encoding collection files...";

            std::string clearGeneratedFiles = std::string("/bin/rm ") + clo.getOutputDirectory() + "*";
            LOG(INFO) << "Executing command: " << clearGeneratedFiles;
            system(clearGeneratedFiles.c_str());

            std::string collectionClassEncodeCommand = clo.getPathToColibri() + " -d " + clo.getOutputDirectory() + " -o "
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
        } else // not so fresh run
        {

        }

//        LOG(INFO) << "Processing testing files";

        int numberOfTestPatterns = 0;
        int numberOfOOV = 0;
        double totalLogProb = 0.0;

        for(const auto& f: clo.getTestFiles())
        {
            // bool allowunknown, bool autoaddunknown=false, bool append=false, bool quiet=false);
            collectionClassEncoderPtr->encodefile(f, "TESTING.dat", 1, 0, 0, 1);
        }
        collectionClassEncoderPtr->save("TESTING.cls");
        collectionClassDecoderPtr->load("TESTING.cls");

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

                // bool allowunknown=false, bool autoaddunknown = false
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


