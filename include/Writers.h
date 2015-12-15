#ifndef WRITERS_H
#define WRITERS_H

#include <fstream>


class Writer
{
        public:
        virtual void write(const std::string& s, bool newLine = false) = 0;
};

class ScreenWriter : public Writer
{
        public:
        ScreenWriter() {}
        void write(const std::string& s, bool newLine = false) { std::cout << s << (newLine ? "\n" : ""); }
};

class FileWriter : public Writer
{
        std::ofstream os;
        public:
        FileWriter(const std::string& fileName)
        {
                os.open(fileName);
        }
        ~FileWriter()
        {
                os.close();
        }
        void write(const std::string& s, bool newLine = false) { os << s << (newLine ? "\n" : ""); }
};

#endif
