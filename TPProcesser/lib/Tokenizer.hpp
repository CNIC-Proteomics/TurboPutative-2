#ifndef TOKENIZER
#define TOKENIZER

#include <string>
#include <vector>

class Tokenizer
{
    protected:

        std::string delim;

    public:

        Tokenizer(std::string delimiter);
        std::vector<std::string> tokenize(std::string line);
};

#endif