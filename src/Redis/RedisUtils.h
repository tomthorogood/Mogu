/*
 * RedisUtils.h
 *
 *  Created on: Oct 22, 2012
 *      Author: tom
 */

#ifndef REDISUTILS_H_
#define REDISUTILS_H_

#include <declarations.h>
#include <Mogu.h>
#include <Redis/RedisCore.h>
#include <Security/Security.h>

struct UniqueHashPackage
{
	std::string original_hash;
	std::string proofed_hash;
	std::string set_node;
	size_t num_cycles;

	UniqueHashPackage(std::string original=EMPTY, std::string node=EMPTY)
	{
		original_hash = original;
		proofed_hash = original;
		set_node = node;
		num_cycles =0;
	}
};

namespace Redis{
inline bool isMemberOfSet(std::string& value, std::string& set_node)
{
	mApp;
	const char* cval = value.c_str();
	const char* cnode = set_node.c_str();
	app->redisCommand("sismember %s %s", cval, cnode);
	return (bool) Redis::getInt(app->reply());
}

inline void makeUniqueHash(UniqueHashPackage& pkg)
{
	while (isMemberOfSet(pkg.proofed_hash, pkg.set_node))
	{
		pkg.proofed_hash = Security::collision_proof(pkg.proofed_hash);
		++ pkg.num_cycles;
	}
}

}//namespace Redis

#endif /* REDISUTILS_H_ */
