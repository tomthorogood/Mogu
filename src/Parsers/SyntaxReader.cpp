/*
 * SyntaxReader.cpp
 *
 *  Created on: Dec 18, 2012
 *      Author: tom
 */

#include "SyntaxReader.h"
#include <fstream>
#include <TurnLeftLib/Utils/inlines.h>
#include <TurnLeftLib/Utils/explosion.h>
#include <algorithm>
#include <boost/algorithm/string.hpp>

SyntaxReader::SyntaxReader(const char* infile) {
    __infile__ = infile;
    if (__infile__ != EMPTY) {
        std::ifstream in;
        in.open(__infile__);
        while (!in.eof()) {
            if (in.eof()) break;
            std::string line;
            getline(in,line);
            std::cout << line << std::endl;
            read_line(line);
        }
    }
}

void SyntaxReader::read_line(const std::string& line) {
    std::string input = line;
    if (input == EMPTY) return;
    TurnLeft::Utils::trimchar(input);
    if (line.at(0) == '#') return;

    //foo:bar,baz
    size_t delim = line.find_first_of(":");

    //delim = 3
    std::string enumerated = input.substr(0, delim);
    std::string str_eq = input.substr(delim+1);
    for (std::pair <std::string, std::vector<std::string> > pr: __tpairs__) {
        if (pr.first == str_eq)
        {
            return;
        }
    }
    std::vector <std::string> wordlist = word_list(str_eq);
    __tpairs__.push_back(std::make_pair(enumerated, wordlist));
}

std::vector<std::string> SyntaxReader::word_list(const std::string& input)
{
    std::vector <std::string> wordlist;
    if (input.find_first_of(",") != std::string::npos) {
        TurnLeft::Utils::Explosion explosion(input);
        boost::split(wordlist, input, boost::is_any_of(","));
    }
    else wordlist.push_back(input);
    return wordlist;
}
