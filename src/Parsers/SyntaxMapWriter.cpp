/*
 * SyntaxMapWriter.cpp
 *
 *  Created on: Dec 18, 2012
 *      Author: tom
 */

#include "SyntaxMapWriter.h"

SyntaxMapWriter::SyntaxMapWriter(
   const std::string& e_type
   , const std::string& map_name
   , std::string preface) {
    __ebuf__ << "#ifndef __SYNTAX_H_\n#define __SYNTAX_H_\n";
    __ebuf__ << preface << "\nnamespace Tokens {\n"
            << "\n\nenum " << e_type << "{\n";
    __mbuf__ <<
        "\n\nextern SyntaxMap <" << e_type << "> " << map_name << ";";
    __mbuf__ <<
        "\n" << map_name << " = " << "SyntaxMap <" << e_type << ">\n";
}

void SyntaxMapWriter::add_entry (
    std::pair <std::string, std::vector <std::string> > pr){
    size_t words = pr.second.size();
    __ebuf__ << "\t" << pr.first << ",\n";
    for (size_t i = 0; i < words; ++i) {
        __mbuf__ << "\t(" << pr.first << ",\"" << pr.second[i] << "\")\n";
    }
}

void SyntaxMapWriter::write(const char* outfile)
{
    std::ofstream out;
    __mbuf__ << ";" << std::endl;
    __ebuf__ << "};" << std::endl;
    out.open(outfile);
    out << __ebuf__.str();
    out << __mbuf__.str();
    out << "\n}//namespace Tokens\n#endif //SYNTAX_H_";
    out.close();
}
