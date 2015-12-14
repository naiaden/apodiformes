/*
 * KneserNey.h
 *
 *  Created on: Oct 28, 2013
 *      Author: lonrust
 */

#ifndef KNESERNEY_H_
#define KNESERNEY_H_

#include <pattern.h>
#include <patternmodel.h>
#include <classdecoder.h>

#include <fstream>
#include <glog/logging.h> 

/**
 * EVERYTHING IN LOG SPACE
 */
class KneserNey
{
friend class KneserNeyFactory;
public:
	static constexpr double epsilon = 0.000001;

        const int order;
        const int n;

	/**
	 * Modified Kneser-Ney
	 * Modified Kneser-Ney Count
	 * Modified Kneser-Ney Extend
	 * Modified Kneser-Ney Discount
 * Modified Kneser-Ney Flex
	 */
	enum Modification
	{
		MKN, MKNC, MKNE, MKND, MKNF
	};

        void doSomething();

	Modification algorithm;

	KneserNey(IndexedPatternModel<>* patternModel, ClassDecoder* classDecoder, Modification algorithm = Modification::MKN);
	KneserNey(KneserNey* kneserNey, int order, IndexedPatternModel<>* patternModel, ClassDecoder* classDecoder, Modification algorithm = Modification::MKN);
	KneserNey(int order, IndexedPatternModel<>* patternModel, ClassDecoder* classDecoder, Modification algorithm = Modification::MKN);

	void computeFrequencyStats();
        void recursiveComputeFrequencyStats();
        void precomputeContextValues();
        void recursivePrecomputeContextValues();

        double gamma(const Pattern& pattern, bool debug = false);
        double pknFromLevel(int level, const Pattern& pattern, const Pattern& word, const Pattern& history, bool debug = false);
       
        bool isOOV(const Pattern& pattern);
        bool isOOVWord(const Pattern& word);

        double D(int c);

        double pkn(const Pattern& pattern, const Pattern& word, const Pattern& history, bool debug = false);
        double pkn(const Pattern& pattern, bool debug = false);
        
protected:
        ClassDecoder* classDecoder;
        IndexedPatternModel<>* patternModel;
        KneserNey* bra;
private:
        std::unordered_map<Pattern, int >* contextValues_n1p_oXYo = new std::unordered_map<Pattern, int >();
        std::unordered_map<Pattern, int >* contextValues_n1p_oXYZ = new std::unordered_map<Pattern, int >();
        std::unordered_map<Pattern, std::tuple<int, int, int> >* contextValues = new std::unordered_map<Pattern, std::tuple<int, int, int> >();

	double n1, n2, n3, n4;
        double Y, D1, D2, D3plus;
	double tokens;

        const int MAXLEVEL=4;


};




/**
 * Files look like this:
 *  -   ORDER 0
 *      ALGORITHM algorithm
 *      COUNTOFCOUNT n1 n2 n3 n4
 *      DISCOUNTS Y D1 D2 D3plus
 *      TOKENS tokens
 *      M m.size()
 *      m0
 *      m1
 *      ...
 *      mlast
 *
 *      ...
 *
 *  -   ORDER n
 *      ALGORITHM algorithm
 *      COUNTOFCOUNT n1 n2 n3 n4
 *      DISCOUNTS Y D1 D2 D3plus
 *      TOKENS tokens
 *      M m.size()
 *      m0
 *      m1
 *      ...
 *      mlast
 */
struct KneserNeyFactory
{
    static KneserNey* readFromFile(const std::string& fileName, IndexedPatternModel<>* patternModel, ClassDecoder* classDecoder = nullptr)
    {
        std::ifstream is;
        is.open(fileName);

        return recursiveReadFromFile(is, patternModel, classDecoder);

        is.close();

    }

    static KneserNey* recursiveReadFromFile(std::istream& is, IndexedPatternModel<>* patternModel, ClassDecoder* classDecoder = nullptr)
    {

        std::string line;
        std::string holder;

        int order;{
        std::getline(is, line); std::istringstream iss(line);
        iss >> holder >> order;
        }

        int algorithm;{
        std::getline(is, line); std::istringstream iss(line);
        iss >> holder >> algorithm;}

        int n1, n2, n3, n4;{
        std::getline(is, line); std::istringstream iss(line);
        iss >> holder >> n1 >> n2 >> n3 >> n4;}

        double Y, D1, D2, D3plus;{
        std::getline(is, line); std::istringstream iss(line);
        iss >> holder >> Y >> D1 >> D2 >> D3plus;}

        int tokens;{
        std::getline(is, line); std::istringstream iss(line);
        iss >> holder >> tokens;}

        int oxyosize;{
        std::getline(is, line); std::istringstream iss(line);
        iss >> holder >> oxyosize;}

        std::unordered_map<Pattern, int>* contextValues_n1p_oXYo = new std::unordered_map<Pattern, int>();
        for(int i = 0; i < oxyosize; ++i)
        {
            Pattern p(&is, false, false);
            std::getline(is, line); std::istringstream iss(line);
            int i1;
            iss >> i1;
            (*contextValues_n1p_oXYo)[p] = i1;
        }

        int oxyzsize;
        {std::getline(is, line); std::istringstream iss(line);
        iss >> holder >> oxyzsize;}

        std::unordered_map<Pattern, int>* contextValues_n1p_oXYZ = new std::unordered_map<Pattern, int>();
        for(int i = 0; i < oxyzsize; ++i)
        {
            Pattern p(&is, false, false);
            std::getline(is, line); std::istringstream iss(line);
            int i1;
            iss >> i1;
            (*contextValues_n1p_oXYZ)[p] = i1;
        }

        int contextvaluessize;
//        {std::getline(is, line); std::istringstream iss(line);
//        iss >> holder >> contextvaluessize;}

        std::unordered_map<Pattern, std::tuple<int, int, int> >* contextValues = new std::unordered_map<Pattern, std::tuple<int, int, int> >();;
//        for(int i = 0; i < contextvaluessize; ++i)
//        {
//            
//            Pattern p(&is, false, false);
////            std::cout << p.tostring(*classDecoder) << std::endl;
//            std::getline(is, line); std::istringstream iss(line);
//            int i1, i2, i3;
//            iss >> i1 >> i2 >> i3;
//            (*contextValues)[p] = std::tuple<int, int, int>(i1, i2,i3);
//        }
//
//        LOG(INFO) << "order:" << order << "alg:" << algorithm << "ns:" << n1 << ";" << n2 << ";" << n3 << ";" << n4 << "Ds:" << Y << ";" << D1 << ";" << D2 << ";" << D3plus << "toks:" << tokens << "oxyo:" << oxyosize << " oxyz: " << oxyzsize << " cv: " << contextvaluessize;
        if(order >0) 
        {
            KneserNey* kn = new KneserNey(recursiveReadFromFile(is, patternModel, classDecoder), order, patternModel, classDecoder, static_cast<KneserNey::Modification>(algorithm));
            kn->n1 = n1;
            kn->n2 = n2;
            kn->n3 = n3;
            kn->n4 = n4;
            kn->Y = Y;
            kn->D1 = D1;
            kn->D2 = D2;
            kn->D3plus = D3plus;
            kn->tokens = tokens;
            kn->contextValues_n1p_oXYo = contextValues_n1p_oXYo; //pointer maken?
            kn->contextValues_n1p_oXYZ = contextValues_n1p_oXYZ; //pointer maken?
            kn->contextValues = contextValues; //pointer maken?
            return kn;
        } else
        {
            KneserNey* kn = new KneserNey(nullptr, order, patternModel, classDecoder, static_cast<KneserNey::Modification>(algorithm));
            kn->n1 = n1;
            kn->n2 = n2;
            kn->n3 = n3;
            kn->n4 = n4;
            kn->Y = Y;
            kn->D1 = D1;
            kn->D2 = D2;
            kn->D3plus = D3plus;
            kn->tokens = tokens;
            kn->contextValues_n1p_oXYo = contextValues_n1p_oXYo; //pointer maken?
            kn->contextValues_n1p_oXYZ = contextValues_n1p_oXYZ; //pointer maken?
            kn->contextValues = contextValues; //pointer maken?
            return kn;
        }
    }

    static void writeToFile(KneserNey& kneserNey, const std::string& fileName, ClassDecoder* classDecoder = nullptr)
    {
        std::ofstream os;
        os.open(fileName);
        recursiveWriteToFile(kneserNey, os, classDecoder);
        os.close();
    }

    static void recursiveWriteToFile(KneserNey& kneserNey, std::ostream& os, ClassDecoder* classDecoder = nullptr)
    {
        os << "ORDER " << kneserNey.order << "\n";
        os << "ALGORITHM " << kneserNey.algorithm << "\n";
        os << "COUNTOFCOUNT " << kneserNey.n1 << " " << kneserNey.n2 << " " << kneserNey.n3 << " " << kneserNey.n4 << "\n";
        os << "DISCOUNTS " << kneserNey.Y << " " << kneserNey.D1 << " " << kneserNey.D2 << " " << kneserNey.D3plus << "\n";
        os << "TOKENS " << kneserNey.tokens << "\n";
        os << "CONTEXToXYo " << (kneserNey.contextValues_n1p_oXYo)->size() << "\n";
        for(const auto& iter: *(kneserNey.contextValues_n1p_oXYo))
        {
            std::cout << "(" << kneserNey.order << ") " << iter.first.tostring(*classDecoder) << std::endl;
            iter.first.write(&os);
            os << iter.second << "\n";
        }
        os << "CONTEXToXYZ " << (kneserNey.contextValues_n1p_oXYZ)->size() << "\n";
        for(const auto& iter: *(kneserNey.contextValues_n1p_oXYZ))
        {
            std::cout << "[" << kneserNey.order << "] " << iter.first.tostring(*classDecoder) << std::endl;
            iter.first.write(&os);
            os << iter.second << "\n";
        }
/*        os << "CONTEXTVALUES " << kneserNey.contextValues->size() << "\n";
        for(const auto& iter: *(kneserNey.contextValues))
        {
            iter.first.write(&os);
            std::tuple<int, int, int> iterVals = iter.second;
            os << std::get<0>(iterVals) << " " << std::get<1>(iterVals) << " " << std::get<2>(iterVals) << "\n";
        }
*/
        if(kneserNey.bra)
        {
            recursiveWriteToFile(*(kneserNey.bra), os, classDecoder);
        }
    }
};



#endif /* KNESERNEY_H_ */
