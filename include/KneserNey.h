/*
 * KneserNey.h
 *
 *  Created on: Oct 28, 2013
 *      Author: lonrust
 */

#ifndef KNESERNEY_H_
#define KNESERNEY_H_

#include "VectorSpaceModel.h"
#include <pattern.h>

#include <fstream>


/**
 * EVERYTHING IN LOG SPACE
 */
class KneserNey: public VectorSpaceModel
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

        void doSomething(int indentation = 0);

	Modification algorithm;

	KneserNey(IndexedPatternModel<>* patternModel, ClassDecoder* classDecoder, Modification algorithm = Modification::MKN);

	void computeFrequencyStats(int indentation = 0);
        void recursiveComputeFrequencyStats(int indentation = 0);
        void computeAllN(int indentation = 0);
        void recursiveComputeAllN(int indentation = 0);
        void iterativeComputeAllN(int indentation = 0);

        double gamma(const Pattern& pattern);
       
        bool isOOV(const Pattern& pattern, int indentation = 0);

        double D(int c);


        double pkn(const Pattern& word, const Pattern& history, int indentation = 0);
        double pkn(const Pattern& pattern, int indentation = 0);
protected:
	KneserNey(int order, IndexedPatternModel<>* patternModel, ClassDecoder* classDecoder, Modification algorithm = Modification::MKN);
        
        KneserNey* bra;
private:
	double n1, n2, n3, n4;
        double Y, D1, D2, D3plus;
	double tokens;

        std::unordered_map<Pattern, std::tuple<int, int, int, int> > m;
        std::unordered_map<Pattern, double> pkn_cache;


	/**
	 * This is a congregated function that determines N_1, N_2, and N_3+ for
	 * the pattern at one go. N_x is the number of n-grams of which the last
	 * word is a wildcard, and occur exactly x times.
	 */
	double N(const Pattern& pattern, int& N1, int& N2, int& N3, int& marginalCount);

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
    static void readFromFile(const std::string& fileName, ClassDecoder* classDecoder = nullptr)
    {
        std::ifstream is;
        is.open(fileName);

        recursiveReadFromFile(is, classDecoder);

        is.close();

    }

    static void recursiveReadFromFile(std::istream& is, ClassDecoder* classDecoder = nullptr)
    {

        std::cout << "CLASSDECODER SIZE: " << classDecoder->size() << std::endl;
        
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

        int msize;{
        std::getline(is, line); std::istringstream iss(line);
        iss >> holder >> msize;}

        std::cout << "order:" << order << "alg:" << algorithm << "ns:" << n1 << ";" << n2 << ";" << n3 << ";" << n4 << "Ds:" << Y << ";" << D1 << ";" << D2 << ";" << D3plus << "toks:" << tokens << "m:" << msize << std::endl;

        std::unordered_map<Pattern, std::tuple<int, int, int, int> > m;
        for(int i = 0; i < msize; ++i)
        {
            
            Pattern p(&is, false, false);
            std::cout << p.tostring(*classDecoder) << std::endl;
            std::getline(is, line); std::istringstream iss(line);
            int i1, i2, i3, i4;
            iss >> i1 >> i2 >> i3 >> i4;
            //m[p] = std::tuple<int, int, int, int>(i1, i2,i3, i4);
        }

        KneserNey// maak hier de recursieve kneserneychain aan(

        if(order >0) recursiveReadFromFile(is, classDecoder);
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
        os << "M " << kneserNey.m.size() << "\n";
        for(const auto& iter: kneserNey.m)
        {
            std::cout << iter.first.tostring(*classDecoder) << std::endl;
            iter.first.write(&os);
            std::tuple<int, int, int, int> iterVals = iter.second;
            os << std::get<0>(iterVals) << " " << std::get<1>(iterVals) << " " << std::get<2>(iterVals) << " " << std::get<3>(iterVals) << "\n";
        }

        if(kneserNey.bra)
        {
            recursiveWriteToFile(*(kneserNey.bra), os, classDecoder);
        }
    }
};



#endif /* KNESERNEY_H_ */
