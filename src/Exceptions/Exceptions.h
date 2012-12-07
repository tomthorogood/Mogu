/*
 * Exceptions.h
 *
 *  Created on: Aug 22, 2012
 *      Author: tom
 */

#ifndef EXCEPTIONS_H_
#define EXCEPTIONS_H_

#include <exception>

namespace Exceptions {
class MoguException: public std::exception
{
    std::string __node_name;
    std::string __error_msg;
public:
    MoguException(
        const std::string& node_name, const std::string& error_msg)
        : std::exception()
    {
        __node_name = node_name;
        __error_msg = error_msg;
    }
    virtual const char* what() const throw ()
    {
        std::string output(__node_name);
        output.append(": ");
        output.append(__error_msg);
        const char* __out = output.c_str();
        return __out;
    }
    virtual ~MoguException() throw ()
    {
    }
};

class Err_UserNotFound: public MoguException
{
public:
    Err_UserNotFound(
        std::string node_name, std::string error_msg)
        : MoguException(node_name, error_msg)
    {
    }
};

class Err_PasswordNotMatch: public MoguException
{
public:
    Err_PasswordNotMatch(
        std::string a, std::string b)
        : MoguException(a, b)
    {
    }
};

class Err_SlotLookupFailed: public MoguException
{
public:
    Err_SlotLookupFailed(
        std::string a, std::string b)
        : MoguException(a, b)
    {
    }
};

class Err_WtAuthFailure: public MoguException
{
public:
    Err_WtAuthFailure(
        std::string a, std::string b)
        : MoguException(a, b)
    {
    }
};

class Err_EventOrderException: public MoguException
{
public:
    Err_EventOrderException(
        std::string a, std::string b)
        : MoguException(a, b)
    {
    }
};

class Err_CorruptedSessionList: public MoguException
{
    std::string session_id;
public:
    Err_CorruptedSessionList(
        std::string a, std::string b)
        : MoguException(a, b)
    {
        session_id = b;
    }
    inline std::string getSessionID() const
    {
        return session_id;
    }
    virtual ~Err_CorruptedSessionList() throw ()
    {
    }
};

class Err_NoUserSet: public MoguException
{
public:
    Err_NoUserSet(
        std::string b)
        : MoguException("No User Set! ", b)
    {
    }
};

class Err_NoCallbackGiven: public MoguException
{
public:
    Err_NoCallbackGiven(
        std::string b)
        : MoguException("Attempted to use a callback without"
            " telling me what callback you intended to use at: ", b)
    {
    }
};

class Err_EmptySession: public MoguException
{
public:
    Err_EmptySession(
        std::string b)
        : MoguException(
            "Somehow the application has no session id. This is terrible.", b)
    {
    }
};

class Err_MissingProperty: public MoguException
{
public:
    Err_MissingProperty(
        std::string node, std::string property)
        : MoguException("Missing " + property + " at ", node)
    {
    }
};

class Err_RecursionError: public MoguException
{
public:
    Err_RecursionError(
        std::string func)
        : MoguException(func,
            ": Recursion error. Object properties changing during recursive strategies!")
    {
    }
};

class Err_NonExistentField: public MoguException
{
public:
    Err_NonExistentField(
        std::string node, std::string field, std::string func)
        : MoguException(
            "Attempt to create string from non-existent field " + node + "->"
                + field + " at ", func)
    {
    }
};

}    //namespace Exceptions
#endif /* EXCEPTIONS_H_ */
