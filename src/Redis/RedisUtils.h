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
#include <Static.h>

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

	inline void lock() {
		mApp;
		app->redisExec(Mogu::Discard, "sadd %s %s",
				set_node.c_str(), proofed_hash.c_str());
	}
};

namespace Redis{

inline bool isMemberOfSet(std::string& value, const char* node)
{
	mApp;
	const char* cval = value.c_str();
	app->redisCommand("sismember %s %s", cval, node);
	return (bool) Redis::getInt(app->reply());
}

inline bool isMemberOfSet(std::string& value, std::string& set_node)
{
	const char* cnode = set_node.c_str();
	return isMemberOfSet(value, cnode);
}



inline void makeUniqueHash(UniqueHashPackage& pkg)
{
	while (isMemberOfSet(pkg.proofed_hash, pkg.set_node))
	{
		pkg.proofed_hash = Security::collision_proof(pkg.proofed_hash);
		++ pkg.num_cycles;
	}
}

inline std::string makeUniqueRChar(const char* set_node)
{
	int sz =1;
	TurnLeft::Utils::RandomCharSet rchar(Application::getRandomSeed());
	std::string str = rchar.generate(sz);
	while (Redis::isMemberOfSet(str, set_node))
	{
		++sz;
		str = rchar.generate(sz);
	}
	return str;
}

inline void getKeysAndVals(
		Redis::strvector& keys, Redis::strvector& vals, const char* node)
{
	mApp;
	app->redisExec(Mogu::Keep, "hkeys %s", node);
	Redis::toVector(app->reply(), keys);
	app->redisExec(Mogu::Keep, "hvals %s", node);
	Redis::toVector(app->reply(), vals);
}

inline std::string reverseHashLookup(const std::string& value, const char* node)
{
	Redis::strvector keys;
	Redis::strvector vals;
	getKeysAndVals(keys,vals,node);
	auto iter = std::find(vals.begin(), vals.end(), value);
	int index = iter - vals.begin();
	return keys[index];
}

}//namespace Redis

#endif /* REDISUTILS_H_ */
