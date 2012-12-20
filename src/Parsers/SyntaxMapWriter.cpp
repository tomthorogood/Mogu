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
   , std::string prefix
   , std::string suffix) {
    e_type_ = e_type;
    map_name_ = map_name;

    __hbuf__ << "#ifndef SYNTAX_H_ \n#define SYNTAX_H_\n";
    __hbuf__ << "#include <Parsers/SyntaxMap.h>\n";
    __hbuf__ << prefix << "\nnamespace Tokens {\n";
    __hbuf__ << "\n\nenum " << e_type << "{\n";

    __tbuf__ << "namespace Tokens {\n";
    __tbuf__ << "SyntaxMap <" << e_type << "> " << map_name << " = ";
    __tbuf__ << "SyntaxMap <" << e_type << "> \n";

    __pbuf__ << "#!/usr/bin/env python\n\n" << map_name << "Strings = [\n";

}

void SyntaxMapWriter::add_entry (
    std::pair <std::string, std::vector <std::string> > pr){
    size_t words = pr.second.size();
    __hbuf__ << "\t" << pr.first << ",\n";
    for (size_t i = 0; i < words; ++i) {
        __tbuf__ << "\t(" << pr.first << ",\"" << pr.second[i] << "\")\n";
        __pbuf__ << "\t\"{" << pr.second[i] << "}\",\n";
    }

}

void SyntaxMapWriter::write(std::string outfile_name)
{
    std::string header_file = outfile_name + ".h";
    std::string text_file = outfile_name + ".txt";
    std::string python_file = outfile_name + ".py";
    std::ofstream hout;
    std::ofstream tout;
    std::ofstream pout;
    hout.open(header_file.c_str());

    __hbuf__ << "};\n\n";
    __hbuf__ << "extern SyntaxMap <" << e_type_ << "> " << map_name_ << ";\n";
    __hbuf__ << "}//namespace Tokens\n\n#endif //SYNTAX_H_";
    hout << __hbuf__.str();
    hout.close();

    tout.open(text_file.c_str());
    __tbuf__ << ";\n}//namespace Tokens\n";
    tout << __tbuf__.str();
    tout.close();

    pout.open(python_file.c_str());
    __pbuf__ << "]\n";
    pout << __pbuf__.str();
    pout.close();

}
