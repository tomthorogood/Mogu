/*
 * MoguScript_Tokenizer.cpp
 *
 *  Created on: Sep 11, 2012
 *      Author: tom
 */

#include <Parsers/MoguScript_Tokenizer.h>
namespace Parsers {


std::string MoguScript_Tokenizer::next( char delimiter)
{
    if (getCurrentPosition() == 0) {
        CharCouplets* w = getWrappers();
        unsigned int s = w->size();
        std::string orig = getOriginal();
        char char0 = orig.at(0);
        for (unsigned int i = 0; i < s; i++) {
            if (w->at(i).first == char0) {
                return TokenGenerator::next();
            }
        }
        setCurrentPosition(orig.length());
        starts_with_string = true;
        return orig;
    }
    return TokenGenerator::next();
}

}    //namespace Parsers
