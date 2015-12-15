#ifndef COMMANDLINEOPTIONS_H
#define COMMANDLINEOPTIONS_H

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

#endif
