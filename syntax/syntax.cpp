#include <Parsers/SyntaxReader.h>
#include <Parsers/SyntaxMapWriter.h>

#define __INFILE__ "syntax.pre"
#define __OUTFILE__ "syntax"

int main()
{
    std::string prefix =  "#include <Parsers/SyntaxMap.h>";
    SyntaxReader reader(__INFILE__);
    auto syntax_pairs = reader.getPairs();
    SyntaxMapWriter writer("MoguTokens", "syntaxMap", prefix);
    writer.add_entries(syntax_pairs);
    writer.write(__OUTFILE__);
    return 0;
}
