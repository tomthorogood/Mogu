#include <Parsers/NodeValueParser.h>
#include <Types/NodeValue.h>
#include <string>

using namespace Parsers;
int main()
{

	NodeValueParser nvp;
	NodeValue v;

	nvp.giveInput(std::string("1818 2819 192389 490 haha here is \"the string!\""),v);
	return 0;
}
