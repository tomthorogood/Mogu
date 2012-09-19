/*
 * SessionIDGenerator.cpp
 *
 *  Created on: Aug 1, 2012
 *      Author: tom
 */

#include <time.h>
#include <Sessions/SessionIDGenerator.h>
#include <Sessions/Lookups.h>
#include <hash.h>
#include <sstream>
#include <stdlib.h>

namespace Sessions{

namespace Generator{

using std::stringstream;

void generate_id (TokenCycles* packet,std::string salt)
{
	stringstream stream;
	if (salt == "")
	{
		srand(time(NULL));
		int rand_ = rand();
		stream << rand_;
	}
	else
	{
		stream << salt;
	}
	stream << time(NULL);
	std::string to_hash(stream.str());
	packet->first = Hash::toHash(to_hash);
	//TODO email sysadmin and prompt a merge if hashing more than X times
	while (Lookups::session_exists(packet->first))
	{
		packet->first = Hash::toHash(packet->first);
	}
}

}//namespace Generator
}//namespace Sessions
