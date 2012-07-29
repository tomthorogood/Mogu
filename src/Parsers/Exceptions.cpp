#include <Parsers/Exceptions.h>

namespace Parsers{
using std::string;

ParseException::ParseException (std::string message)
{
    string tmp = "Parse exception near: ";
    tmp.append(message);
    errMsg = tmp.c_str();
}

inline const char* ParseException::what() const throw()
{
    return errMsg;
}

TemplateMismatchException::TemplateMismatchException (string msg)
{
    string tmp = "Input does not match template format: ";
    tmp.append(msg);
    errMsg = tmp.c_str();
}

inline const char* TemplateMismatchException::what () const throw()
{
    return errMsg;
}

} //namespace Parsers


