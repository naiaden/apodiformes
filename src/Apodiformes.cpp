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

class ProbsWriter
{
	public:
	virtual void write(const std::string& s, bool newLine = false) = 0;
};

class ProbsScreenWriter : public ProbsWriter
{
	public:
	ProbsScreenWriter() {}
	void write(const std::string& s, bool newLine = false) { std::cout << s << (newLine ? "\n" : ""); }
};

class ProbsFileWriter : public ProbsWriter
{
	std::ofstream os;
	public:
	ProbsFileWriter(const std::string& fileName) 
	{
		os.open(fileName);
	}
	~ProbsFileWriter()
	{
		os.close();
	}
	void write(const std::string& s, bool newLine = false) { os << s << (newLine ? "\n" : ""); }	
};

class CommandLineOptions
{
    private:
    std::string applicationName = "Apodiformes";

    std::string corpusFile = "";
    std::string vocabularyFile = "";
    std::string patternModelFile = "";
	std::string modelFile = "";

    std::string inputDirectory = "/scratch/lonrust/apodiformes/input/";
    std::string outputDirectory = "/scratch/lonrust/apodiformes/generated/";
	std::string writeProbsFile = "";
    std::string collectionName = "";
    std::string pathToColibri = "~/Software/colibri-core/src/colibri-classencode";

    bool reverseIndex = false;
    bool doSkipgrams = false;
    int minTokens = 1;
    int maxLength = 4;

    bool debug = false;
    bool overwriteFiles = false;
    bool freshRun = false;
   
    std::vector<std::string> inputFiles;
    std::vector<std::string> testFiles;

    public:
    const std::string& getCorpusFile() const { return corpusFile; }
    const std::string& getVocabularyFile() const { return vocabularyFile; }
    const std::string& getPatternModelFile() const { return patternModelFile; }
	const std::string& getModelFile() const { return modelFile; }

    const std::string& getInputDirectory() const { return inputDirectory; }
    const std::string& getOutputDirectory() const { return outputDirectory; }
    const std::string& getWriteProbsFile() const { return writeProbsFile; }
    const std::string& getCollectionName() const { return collectionName; }
    const std::string& getPathToColibri() const { return pathToColibri; }

    bool getReverseIndex() { return reverseIndex; }
    bool getDoSkipgrams() { return doSkipgrams; }
    int getMinTokens() { return minTokens; }
    int getMaxLength() { return maxLength; }

    bool getDebug() { return debug; }
    bool getOverwriteFiles() { return overwriteFiles; }
    bool getFreshRun() { return freshRun; }

    const std::vector<std::string>& getInputFiles() const { return inputFiles; }
    const std::vector<std::string>& getTestFiles() const { return testFiles; }

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
	"-w, --writeprobs     write probs to file, default is to stdout\n"
        "-I, --inputdir       input directory for text files\n"
        "-O, --outputdir      output directory for generated files\n"
        "-m, --model          output model name\n"
 	"-M, --modelfile      model file\n"
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
               } else if(!key.compare("modelfile"))
		{
			modelFile = value;
		}else if(!key.compare("inputdir"))
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
               } else if(!key.compare("writeprobs"))
		{
			writeProbsFile = value;
		} else if(!key.compare("inputfile"))
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
            } else if(strcmp (argv[argnr],"-M") == 0 || strcmp (argv[argnr],"--modelfile") == 0)
            {
                modelFile = std::string(argv[++argnr]);
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
            } else if(strcmp (argv[argnr],"-T") == 0 || strcmp (argv[argnr],"--mintokens") == 0)
            {
                minTokens = atoi(argv[++argnr]);
            } else if(strcmp(argv[argnr],"-w") == 0 || strcmp(argv[argnr], "--writeprobs") == 0)
		{
			writeProbsFile = std::string(argv[++argnr]);
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
            } else if(strcmp (argv[argnr],"-t") == 0 || strcmp (argv[argnr],"--testfiles") == 0)
            {
                std::string list(argv[++argnr]);
                for(std::string s: tokenizer(list, ','))
                {
                    testFiles.push_back(s);
                }
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
		collectionClassEncoderPtr->build(inputFiles, true, 0); // 0=threshold
		collectionClassEncoderPtr->save(collectionCorpusFile);

		for(auto f: clo.getInputFiles())
		{
			collectionClassEncoderPtr->encodefile(f, collectionEncodedFile, false, false, false);
		}

            LOG(INFO) <<  "+ Creating collection files";
            LOG(INFO) <<  "Class encoding collection files...";

	    collectionClassDecoderPtr = new ClassDecoder(collectionCorpusFile);
            collectionIndexedModelPtr = new IndexedPatternModel<>();
            
            LOG(INFO) << "Indexing collection";
            collectionIndexedModelPtr->train(collectionEncodedFile, options);
            collectionIndexedModelPtr->write(collectionPatternFile);

            LOG(INFO) <<  "- Creating collection files";

	    kneserNeyPtr = new KneserNey(collectionIndexedModelPtr, collectionClassDecoderPtr);

            LOG(INFO) << "+ Computing frequency stats for KN";
            kneserNeyPtr->recursivePrecomputeContextValues();
            kneserNeyPtr->recursiveComputeFrequencyStats();
            LOG(INFO) << "- Computing frequency stats for KN";

	    collectionIndexedModelPtr->computecoveragestats(0,1);

            LOG(INFO) << "Writing Kneser Ney model to file";
            KneserNeyFactory::writeToFile(*kneserNeyPtr, modelFile, collectionClassDecoderPtr);
        } else // not so fresh run
        {
            collectionClassEncoderPtr = new ClassEncoder(collectionCorpusFile);
            collectionClassDecoderPtr = new ClassDecoder(collectionCorpusFile);
            collectionIndexedModelPtr = new IndexedPatternModel<>(collectionPatternFile, options); 


            LOG(INFO) << "Reading Kneser Ney model from file";
            kneserNeyPtr = KneserNeyFactory::readFromFile(modelFile, collectionIndexedModelPtr, collectionClassDecoderPtr);

        }

//        LOG(INFO) << "Processing testing files";

        int numberOfTestPatterns = 0;
        int numberOfOOV = 0;
        double totalLogProb = 0.0;

	{ 
		std::string testOutputDatFile = clo.getOutputDirectory() + "/" + clo.getCollectionName() + ".test.coco.dat";
		std::string testOutputClsFile = clo.getOutputDirectory() + "/" + clo.getCollectionName() + ".test.coco.cls";
		for(const auto& f: clo.getTestFiles())
		{
		    collectionClassEncoderPtr->encodefile(f, testOutputDatFile, 1, 0, 0, 1);
		}
		collectionClassEncoderPtr->save(testOutputClsFile);
		collectionClassDecoderPtr->load(testOutputClsFile);
	}	

	ProbsWriter* probsOut = nullptr;
	if(clo.getWriteProbsFile().empty())
	{
		probsOut = new ProbsScreenWriter();
	} else 
	{
		probsOut = new ProbsFileWriter(clo.getWriteProbsFile());
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


