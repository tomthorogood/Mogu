/*
 * typedefs.h
 *
 *  Created on: Sep 19, 2012
 *      Author: tom
 */

#ifndef TYPEDEFS_H_
#define TYPEDEFS_H_
#include <unordered_map>


namespace Redis {
typedef std::vector<std::string> strvector;
}

typedef TurnLeft::Utils::HungryVector<char> CharVector;

typedef std::vector<std::pair<char, char> > CharCouplets;

typedef std::pair<std::string, int> TokenCycles;

enum class Prefix {
    __NONE__    =0,
    widgets     =1,
    data        =2,
    user        =4,
    group       =8,
    templates   =16,
    validators  =32,
    policies    =64,
    temp        =128,
    perspectives=256,
    /* If adding prefixes, add them BEFORE META, and UPDATE META to reflect
     * a new bit-aligned number.
     */
    meta        =512
};

#endif /* TYPEDEFS_H_ */
