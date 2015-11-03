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

        double gamma(const Pattern& pattern, bool lowerOrder, bool debug = false);
       
        bool isOOV(const Pattern& pattern);
        bool isOOVWord(const Pattern& word);

        double D(int c);

        double pkn(const Pattern& word, const Pattern& history, bool debug = false);
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

        std::unordered_map<Pattern, double> pkn_cache;

        const int MAXLEVEL=4;

	/**
	 * This is a congregated function that determines N_1, N_2, and N_3+ for
	 * the pattern at one go. N_x is the number of n-grams of which the last
	 * word is a wildcard, and occur exactly x times.
	 */
	double N(const Pattern& pattern, int& N1, int& N2, int& N3, int& marginalCount, const Pattern& p);

};




///**
// * Files look like this:
// *  -   ORDER 0
// *      ALGORITHM algorithm
// *      COUNTOFCOUNT n1 n2 n3 n4
// *      DISCOUNTS Y D1 D2 D3plus
// *      TOKENS tokens
// *      M m.size()
// *      m0
// *      m1
// *      ...
// *      mlast
// *
// *      ...
// *
// *  -   ORDER n
// *      ALGORITHM algorithm
// *      COUNTOFCOUNT n1 n2 n3 n4
// *      DISCOUNTS Y D1 D2 D3plus
// *      TOKENS tokens
// *      M m.size()
// *      m0
// *      m1
// *      ...
// *      mlast
// */
//struct KneserNeyFactory
//{
//    static KneserNey* readFromFile(const std::string& fileName, IndexedPatternModel<>* patternModel, ClassDecoder* classDecoder = nullptr)
//    {
//        std::ifstream is;
//        is.open(fileName);
//
//        return recursiveReadFromFile(is, patternModel, classDecoder);
//
//        is.close();
//
//    }
//
//    static KneserNey* recursiveReadFromFile(std::istream& is, IndexedPatternModel<>* patternModel, ClassDecoder* classDecoder = nullptr)
//    {
//
//        std::string line;
//        std::string holder;
//
//        int order;{
//        std::getline(is, line); std::istringstream iss(line);
//        iss >> holder >> order;
//        }
//
//        int algorithm;{
//        std::getline(is, line); std::istringstream iss(line);
//        iss >> holder >> algorithm;}
//
//        int n1, n2, n3, n4;{
//        std::getline(is, line); std::istringstream iss(line);
//        iss >> holder >> n1 >> n2 >> n3 >> n4;}
//
//        double Y, D1, D2, D3plus;{
//        std::getline(is, line); std::istringstream iss(line);
//        iss >> holder >> Y >> D1 >> D2 >> D3plus;}
//
//        int tokens;{
//        std::getline(is, line); std::istringstream iss(line);
//        iss >> holder >> tokens;}
//
//        int msize;{
//        std::getline(is, line); std::istringstream iss(line);
//        iss >> holder >> msize;}
//
//        LOG(INFO) << "order:" << order << "alg:" << algorithm << "ns:" << n1 << ";" << n2 << ";" << n3 << ";" << n4 << "Ds:" << Y << ";" << D1 << ";" << D2 << ";" << D3plus << "toks:" << tokens << "m:" << msize;
//
//        std::unordered_map<Pattern, std::tuple<int, int, int, int> >* m = new std::unordered_map<Pattern, std::tuple<int, int, int, int> >();;
//        for(int i = 0; i < msize; ++i)
//        {
//            
//            Pattern p(&is, false, false);
////            std::cout << p.tostring(*classDecoder) << std::endl;
//            std::getline(is, line); std::istringstream iss(line);
//            int i1, i2, i3, i4;
//            iss >> i1 >> i2 >> i3 >> i4;
//            (*m)[p] = std::tuple<int, int, int, int>(i1, i2,i3, i4);
//        }
//        if(order >0) 
//        {
//            KneserNey* kn = new KneserNey(recursiveReadFromFile(is, patternModel, classDecoder), order, patternModel, classDecoder, static_cast<KneserNey::Modification>(algorithm));
//            kn->n1 = n1;
//            kn->n2 = n2;
//            kn->n3 = n3;
//            kn->n4 = n4;
//            kn->Y = Y;
//            kn->D1 = D1;
//            kn->D2 = D2;
//            kn->D3plus = D3plus;
//            kn->tokens = tokens;
//            kn->m = m; //pointer maken?
//            return kn;
//        } else
//        {
//            KneserNey* kn = new KneserNey(nullptr, order, patternModel, classDecoder, static_cast<KneserNey::Modification>(algorithm));
//            kn->n1 = n1;
//            kn->n2 = n2;
//            kn->n3 = n3;
//            kn->n4 = n4;
//            kn->Y = Y;
//            kn->D1 = D1;
//            kn->D2 = D2;
//            kn->D3plus = D3plus;
//            kn->tokens = tokens;
//            kn->m = m; //pointer maken?
//            return kn;
//        }
//    }
//
//    static void writeToFile(KneserNey& kneserNey, const std::string& fileName, ClassDecoder* classDecoder = nullptr)
//    {
//        std::ofstream os;
//        os.open(fileName);
//        recursiveWriteToFile(kneserNey, os, classDecoder);
//        os.close();
//    }
//
//    static void recursiveWriteToFile(KneserNey& kneserNey, std::ostream& os, ClassDecoder* classDecoder = nullptr)
//    {
//        os << "ORDER " << kneserNey.order << "\n";
//        os << "ALGORITHM " << kneserNey.algorithm << "\n";
//        os << "COUNTOFCOUNT " << kneserNey.n1 << " " << kneserNey.n2 << " " << kneserNey.n3 << " " << kneserNey.n4 << "\n";
//        os << "DISCOUNTS " << kneserNey.Y << " " << kneserNey.D1 << " " << kneserNey.D2 << " " << kneserNey.D3plus << "\n";
//        os << "TOKENS " << kneserNey.tokens << "\n";
//        os << "M " << kneserNey.m->size() << "\n";
//        for(const auto& iter: *(kneserNey.m))
//        {
////            std::cout << iter.first.tostring(*classDecoder) << std::endl;
//            iter.first.write(&os);
//            std::tuple<int, int, int, int> iterVals = iter.second;
//            os << std::get<0>(iterVals) << " " << std::get<1>(iterVals) << " " << std::get<2>(iterVals) << " " << std::get<3>(iterVals) << "\n";
//        }
//
//        if(kneserNey.bra)
//        {
//            recursiveWriteToFile(*(kneserNey.bra), os, classDecoder);
//        }
//    }
//};
//
//
//
#endif /* KNESERNEY_H_ */
