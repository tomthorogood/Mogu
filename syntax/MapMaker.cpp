#include <declarations.h>
#include <Parsers/TokenGenerator.h>
#include <fstream>

#define INFILE "syntax.pre"
#define OUTFILE "Tokens.h"

#define STATIC_START "namespace StaticTokens{\nenum Token{\n"
#define STATIC_END "};//enum Token\n}//namespace StaticTokens"
#define STATIC_LINE(TOK) "\t\t," TOK "\n"

#define MAP_START \
	"std::map <std::string,StaticTokens::Token> = " \
	"create_map <std::string, StaticTokens::Token>\n"

#define R_MAP_START \
	"std::map <StaticTokens::Token, std::string> = " \
	"create_map <StaticTokens::Token, std::string>\n"

/* Returns a reference to a static vector whose first entry will either be
 * the empty string, meaning the line is not parseable, or the enumerated token.
 * Subsequent strings will be string aliases of the token.
 */
const std::vector <std::string>& parse_line(std::string& line)
{
	static std::vector <std::string> vec;
	vec.clear();

	if (line.at(0) == '#') return vec;

	Parsers::TokenGenerator tgen(line);
	std::string token = tgen.next(':');

	if (token == EMPTY) return vec;

	vec.push_back(token);

	do {
		token = tgen.next(',');
		if (token != EMPTY) vec.push_back(token);
	} while (token != EMPTY);

	return vec;
}

bool line_is_parseable (const std::vector <std::string>& vec)
{
	return vec[0] != EMPTY && vec.size() > 1;
}

inline std::string map_line(
		const std::string& alias, const std::string& token)
{
	return ("\t(\""+alias) + ("\")("+token) + ")\n";
}

inline std::string r_map_line(
		const std::string& token, const std::string& alias)
{
	return ("\t("+token) + (")(\""+alias) + "\")\n";
}

inline void write_map(std::ofstream& out)
{
	out << MAP_START;
	std::ifstream in;
	in.open(INFILE);

	while (!in.eof())
	{
		if (in.eof()) break;
		std::string line;
		getline(in,line);
		const std::vector <std::string>& tokens
			= parse_line(line);
		if (!line_is_parseable(tokens)) break;
		size_t len = tokens.size();
		for (size_t t = 1; t < len; ++t)
		{
			std::string out_line = map_line(tokens[t], tokens[0]);
			out << out_line << std::endl;
		}
	}
	out << ";" << std::endl << std::endl;
}

using std::ifstream;
using std::ofstream;

