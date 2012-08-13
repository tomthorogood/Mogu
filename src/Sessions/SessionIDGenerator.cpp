/*
 * SessionIDGenerator.cpp
 *
 *  Created on: Aug 1, 2012
 *      Author: tom
 */

#include <time.h>
#include <Sessions/SessionIDGenerator.h>
#include <hash.h>
#include <sstream>
#include <stdlib.h>

namespace Sessions{

namespace Generator{

using std::stringstream;

std::string generate_id (std::string salt)
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
	std::string hashed_string = Hash::toHash(to_hash);
	return hashed_string;
}

}//namespace Generator
}//namespace Sessions
