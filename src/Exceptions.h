/*
 * Exceptions.h
 *
 *  Created on: Aug 20, 2012
 *      Author: tom
 */

#ifndef EXCEPTIONS_H_
#define EXCEPTIONS_H_

#include <declarations.h>
#include <exception>
class MoguException : public std::exception
{
	const char* __message;
public:
	MoguException (std::string message)
	{
		__message = message.c_str();
	}
	virtual const char* what() const throw()
	{
		return __message;
	}
};

class MoldableGooLoadException : public MoguException
{
	const char* __node_name;
public:
	MoldableGooLoadException(std::string message, Goo::Moldable* widget)
	:	MoguException(message)
	{
		std::string node_name = widget->getNodeList()->at(0);
		__node_name = node_name.c_str();
	}

	virtual const char* what() const throw()
	{
		const char* msg = MoguException::what();
		char* ret = new char[strlen(msg)+32];
		strcpy(ret,__node_name);
		strcat(ret, ": ");
		strcat(ret, msg);
		const char* _ret = ret;
		return _ret;
	}
};


#endif /* EXCEPTIONS_H_ */
