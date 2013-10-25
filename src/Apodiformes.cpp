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

#include <classencoder.h>
#include <classdecoder.h>
#include <patternmodel.h>

int main(int argc, char** argv)
{
	std::cout << "STRAK" << std::endl;

	std::string rawcorpusfile = "/tmp/hamlet.txt";
	std::ofstream * out = new std::ofstream(rawcorpusfile);
	const char * poem =
	"To be or not to be , that is the question ;\n"
	"Whether 'tis nobler in the mind to suffer\n"
	"The slings and arrows of outrageous fortune ,\n"
	"Or to take arms against a sea of troubles ,\n"
	"And by opposing , end them . To die , to sleep ;\n"
	"No more ; and by a sleep to say we end\n"
	"The heart-ache and the thousand natural shocks\n"
	"That flesh is heir to — 'tis a consummation\n"
	"Devoutly to be wish'd . To die , to sleep ;\n"
	"To sleep , perchance to dream . Ay , there's the rub ,\n"
	"For in that sleep of death what dreams may come ,\n"
	"When we have shuffled off this mortal coil ,\n"
	"Must give us pause . There's the respect\n"
	"That makes calamity of so long life ,\n"
	"For who would bear the whips and scorns of time,\n"
	"Th'oppressor's wrong , the proud man 's contumely ,\n"
	"The pangs of despised love , the law 's delay ,\n"
	"The insolence of office , and the spurns\n"
	"That patient merit of th' unworthy takes ,\n"
	"When he himself might his quietus make\n"
	"With a bare bodkin ? who would fardels bear ,\n"
	"To grunt and sweat under a weary life ,\n"
	"But that the dread of something after death ,\n"
	"The undiscovered country from whose bourn\n"
	"No traveller returns , puzzles the will ,\n"
	"And makes us rather bear those ills we have\n"
	"Than fly to others that we know not of ?\n"
	"Thus conscience does make cowards of us all ,\n"
	"And thus the native hue of resolution\n"
	"Is sicklied o'er with the pale cast of thought ,\n"
	"And enterprises of great pitch and moment\n"
	"With this regard their currents turn awry ,\n"
	"And lose the name of action .\n"
	"Soft you now ! The fair Ophelia ! Nymph ,\n"
	"in thy orisons be all my sins remember'd .\n"
	"To flee or not to flee .\n" //additions to test skipgrams
	"To see or not to see .\n"
	"To pee or not to pee .\n"; //See that Shakespeare? I could be a poet too!
	*out << std::string(poem);
	out->close();

	std::cerr << "Class encoding corpus..." << std::endl;
	system("colibri-classencode /tmp/hamlet.txt");



	std::cerr << "Class decoding corpus..." << std::endl;
	system("colibri-classdecode -c /tmp/hamlet.colibri.cls -f /tmp/hamlet.colibri.dat");


	PatternModelOptions options;
	options.DOREVERSEINDEX = true;
	options.DOFIXEDSKIPGRAMS = true;

	const std::string classfile = "/tmp/hamlet.colibri.cls";
	ClassDecoder classdecoder = ClassDecoder(classfile);
	ClassEncoder classencoder = ClassEncoder(classfile);

	std::string infilename = "/tmp/hamlet.colibri.dat";
	std::string outputfilename = "/tmp/data.colibri.patternmodel";


	std::cerr << std::endl;
	std::cerr << "Building indexed model" << std::endl;
	IndexedPatternModel<> indexedmodel;
	indexedmodel.train(infilename, options);

	std::cerr << "Iterating over all patterns" << std::endl;
	for (IndexedPatternModel<>::iterator iter = indexedmodel.begin(); iter != indexedmodel.end(); iter++) {
		const Pattern pattern = iter->first;
		const IndexedData data = iter->second;
		std::cout << pattern.tostring(classdecoder) << std::endl;
	}



	std::cout << "ALS EEN REIGER" << std::endl;
}
