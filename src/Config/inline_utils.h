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

inline std::string remove_widget_prefix (const std::string& s)
{
	const std::string r = "widgets.";
	const size_t len = r.length();
	if (s.find(r) == std::string::npos) return s;
	return s.substr(len,s.length()-len);
}


#endif /* INLINE_UTILS_H_ */
