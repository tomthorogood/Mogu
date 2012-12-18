/*
 * SyntaxReader.h
 *
 *  Created on: Dec 18, 2012
 *      Author: tom
 */

#ifndef SYNTAXREADER_H_
#define SYNTAXREADER_H_

#include <declarations.h>

class SyntaxReader {
    const char* __infile__;
    std::vector <std::pair <std::string, std::vector <std::string> > >__tpairs__;
    std::vector <std::string> word_list(const std::string& input);
public:
    SyntaxReader(const char* infile =EMPTY);
    void read_line(const std::string& line);

    inline const
        std::vector <std::pair <std::string, std::vector <std::string> > >&
        getPairs() { return __tpairs__;}
};


#endif /* SYNTAXREADER_H_ */
