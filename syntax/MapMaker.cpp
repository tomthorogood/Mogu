#include <fstream>
#include <vector>
#include <string>
#include <TurnLeftLib/Utils/inlines.h>
#include <exception>
#include <iostream>

using std::ifstream;
using std::ofstream;
using std::string;
using std::endl;
using std::cout;
using TurnLeft::Utils::trimchar;

#define FILENAME "syntax.pre"

typedef std::vector <string> strvector;

inline void write_enum_line(ofstream& out, std::string& token, bool first=false)
{
    std::string ln = "\t";
    if (!first) ln += ',';
    ln += token;
    out << ln << endl;
}

inline void write_map_line(ofstream& out, std::string& token)
{
    std::string ln = "\t(\"";
    ln += token;
    ln += "\")";
    ln += "("+token+")";
    out << ln << endl;
}

inline void write_reverse_map_line(ofstream& out, std::string& token)
{
	std::string ln = "\t(" + token + ")";
	ln += "(\"" + token + "\")";
	out << ln << endl;
}

inline bool is_valid(string& line)
{
    try 
    {
        char c = line.at(0);
        if (!isalpha(c)) return false;
    }
    catch (const std::exception& e)
    {
    	return false;
    }
    return true;
}

void read_lines(strvector& vec)
{
    ifstream f;
    f.open(FILENAME);
    string line;

    if (!f.is_open()) return;
    while (!f.eof())
    {
        if (f.eof()) break;
        getline(f, line);
        try
        {
        	trimchar(line);
        	if (!is_valid(line)) continue;
        }
        catch (const std::exception& e)
        {
        	continue;
        }
        vec.push_back(line);
    }
}

void make_enum(ofstream& out, strvector& vec)
{
    std::string enum_start = "enum MoguToken {";
    out << enum_start << endl;
    size_t vecsz = vec.size();
    for (size_t i = 0; i < vecsz; ++i)
    {
        bool first = i==0;
        write_enum_line(out, vec.at(i), first); 
    }
    out << "};" << endl;
}

void make_str_map(ofstream& out, strvector& vec)
{
    std::string map_start = "static std::map<std::string,MoguToken> tokenParser = create_map <std::string, MoguToken> ()";
    out << map_start << endl;
    size_t vecsz = vec.size();
    for (size_t i = 0; i < vecsz; ++i)
    {
        write_map_line(out, vec.at(i));
    }
    out << ";" << endl;
}

void make_enum_map(ofstream& out, strvector& vec)
{
	std::string map_start = "static std::map<MoguToken, std::string> reverseTokenParser = create_map <MoguToken,std::string> ()";
	out << map_start << endl;
	size_t vecsz = vec.size();
	for (size_t i = 0; i < vecsz; ++i)
	{
		write_reverse_map_line(out, vec.at(i));
	}
	out << ";" << endl;
}

int main()
{
    strvector tokens;
    read_lines(tokens);
    ofstream out;
    out.open("Tokens.h");
    out << "#include <declarations.h>" << endl << endl;
    make_enum(out,tokens);
    out << endl << endl;
    make_str_map(out,tokens);
    make_enum_map(out,tokens);
    out.close();

    return 0;
}
