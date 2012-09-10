/*
 * inline_utils.h
 *
 *  Created on: Sep 10, 2012
 *      Author: tom
 */

#ifndef INLINE_UTILS_H_
#define INLINE_UTILS_H_

#include <sstream>

inline std::string itoa (int i)
{
	std::stringstream s;
	s << i;
	return s.str();
}


#endif /* INLINE_UTILS_H_ */
