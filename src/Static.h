/*
 * Static.h
 *
 *  Created on: Jul 29, 2012
 *      Author: tom
 */

#ifndef STATIC_H_
#define STATIC_H_

#include <declarations.h>

/*!\brief The Application namespace houses a library of functions that are
 * persistent within the user session. The data held here is unique
 * to each user. This can make data available globally to the application
 * instance without exposing it anywhere else.
 */
namespace Application
{
	bool metaKeyConfigured(std::string key);
	std::string getMetaValue(std::string key);
	int getUserLevel(const std::string&);
	std::string getUserLevel(int&);
	void configure();

inline int getRandomSeed()
{
	static int seed_salt = 1;
	srand( time(NULL) + ++seed_salt);
	return rand();
}

const size_t& getNumLevels();

}//namespace Application


#endif /* STATIC_H_ */
