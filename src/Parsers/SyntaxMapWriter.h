/*
 * SyntaxMapWriter.h
 *
 *  Created on: Dec 18, 2012
 *      Author: tom
 */

#ifndef SYNTAXMAPWRITER_H_
#define SYNTAXMAPWRITER_H_

#include <declarations.h>
#include <fstream>
#include <sstream>

class SyntaxMapWriter {
    std::stringstream __mbuf__;
    std::stringstream __ebuf__;
public:
    SyntaxMapWriter(
        const std::string& e_type
        , const std::string& map_name
        , std::string preface =EMPTY);
    void add_entry(std::pair <std::string, std::vector <std::string> >);


    inline void add_entries(
    std::vector <std::pair <std::string, std::vector <std::string> > > v) {
        size_t vsz = v.size();
        for (size_t i = 0; i < vsz; ++i)
        {
            std::pair <std::string, std::vector <std::string> > pr = v.at(i);
            add_entry(pr);
        }
    }
    void write(const char* outfile);

};


#endif /* SYNTAXMAPWRITER_H_ */
