#include <hash.h>
#include <iostream>
#include <string>
#include <Security/Encryption.h>
#include <sstream>
#include <TurnLeftLib/Utils/inlines.h>

using namespace std;

int main(int nargs, char** args)
{
	stringstream rpl;
	string val = args[2];
	for (uint8_t ch =0; ch < val.size(); ch++)
	{
		if (val.at(ch) == '_') rpl << ' ';
		else rpl << val.at(ch);
	}
	val = rpl.str();
    TurnLeft::Utils::trimchar(val);
	if (string(args[1]) == "encrypt")
	{
		cout << Security::encrypt(string(args[2])) << endl;
	}
	else
	{
		val = Security::decrypt(val);
		TurnLeft::Utils::trimchar(val, '_');
		cout << val << endl;
	}

    return 0;
}

