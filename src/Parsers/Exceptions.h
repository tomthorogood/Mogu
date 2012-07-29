/*
 * Exceptions.h
 *
 *  Created on: Jul 15, 2012
 *      Author: tom
 */

#ifndef EXCEPTIONS_H_
#define EXCEPTIONS_H_

#include <declarations.h>

namespace Parsers
{


/*!\brief The ParseException is thrown if there is a mismatch in either the
 * template or the string corresponding to the template, wherein any set of
 * enclosures is off balance (an opening bracket without a closing bracket,
 * for instance.
 */
class ParseException : public std::exception
{
private:
    const char* errMsg;
public:
    ParseException (std::string message);
    const char* what () const throw();
};


/*!\brief the TemplateMismatchException is thrown if the string corresponding
 * to a template directive does not match the template's expectations.
 * If the template is: {%VAR%}, and the corresponding string is
 * Hello{GREETING}, this would be a mismatch.
 */
class TemplateMismatchException : public std::exception
{
private:
    const char* errMsg;
public:
    TemplateMismatchException(std::string msg);
    const char* what() const throw();
};


}//namespace Parsers
#endif /* EXCEPTIONS_H_ */
