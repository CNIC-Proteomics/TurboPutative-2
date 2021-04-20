#include <string>
#include <vector>

#include "Tokenizer.hpp"

Tokenizer::Tokenizer(std::string delimiter)
{
    delim = delimiter;
}

std::vector<std::string> Tokenizer::tokenize(std::string line)
{
    // std::string delim = "\t";
    std::vector<std::string> tokenArr;

    std::string line_tmp = line;
    auto lastPos = line.find(delim);

    while (lastPos != std::string::npos)
    {
        std::string token = line_tmp.substr(0, lastPos);
        tokenArr.push_back(token);

        line_tmp = line_tmp.substr(lastPos+delim.length());
        lastPos = line_tmp.find(delim);
    }

    tokenArr.push_back(line_tmp);

    return tokenArr;
}