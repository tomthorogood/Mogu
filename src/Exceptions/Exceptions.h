/*
 * Exceptions.h
 *
 *  Created on: Aug 22, 2012
 *      Author: tom
 */

#ifndef EXCEPTIONS_H_
#define EXCEPTIONS_H_

#include <exception>

namespace Exceptions
{
class MoguException : public std::exception
{
	std::string __node_name;
	std::string __error_msg;
public:
	MoguException (
			const std::string& node_name,
			const std::string& error_msg)
	:	std::exception()
	{
		__node_name = node_name;
		__error_msg = error_msg;
	}
	virtual const char* what() const throw()
	{
		std::string output(__node_name);
		output.append(": ");
		output.append(__error_msg);
		const char* __out = output.c_str();
		return __out;
	}
	virtual ~MoguException() throw() {}
};

class Err_UserNotFound : public MoguException
{
public:
	Err_UserNotFound(std::string node_name, std::string error_msg)
	:	MoguException(node_name, error_msg){}
};

class Err_PasswordNotMatch : public MoguException
{
public:
	Err_PasswordNotMatch(std::string a, std::string b)
	: MoguException (a,b){}
};

class Err_SlotLookupFailed : public MoguException
{
public:
	Err_SlotLookupFailed(std::string a, std::string b)
	: MoguException(a,b){}
};

class Err_WtAuthFailure : public MoguException
{
public:
	Err_WtAuthFailure(std::string a, std::string b)
	: MoguException(a,b){}
};

class Err_EventOrderException : public MoguException
{
public:
	Err_EventOrderException(std::string a, std::string b)
	:	MoguException(a,b){}
};

}//namespace Exceptions
#endif /* EXCEPTIONS_H_ */
