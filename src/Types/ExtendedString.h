/*
 * PlainTextValue.h
 *
 *  Created on: Nov 8, 2012
 *      Author: tom
 */

#ifndef PLAINTEXTVALUE_H_
#define PLAINTEXTVALUE_H_

class StringFormatException : public std::exception
{
	const char* err;
public:
	StringFormatException (const char* valname, const char* line)
	: std::exception()
	{
		std::string str("Inherited string class of invalid type at ");
		str+= valname + " " + line;
		err = str.c_str();

	}
	const char* what() const throw()
	{
		return err;
	}
};


class ExtendedString : public std::string{
	std::string bad;
protected:
	std::string getBad() =0;
	std::string getName() =0;
	inline void testInput(const std::string& str)
	{
		if (str.find(bad) != std::string::npos)
		{
			std::string name = getName();
			throw StringFormatException(name.c_str(), __LINE__);
		}
	}
public:
	ExtendedString() : std::string() {}

	ExtendedString (const std::string& str) : std::string(str)
	{
		bad = getBad();
		testInput(str);
	}

	ExtendedString (const char* str) : std::string(str)
	{
		bad = getBad();
		std::string s(str);
		testInput(s);
	}

};

class PlainTextValue : public ExtendedString
{
protected:
	inline std::string getBad() { return "_0_";}
	inline std::string getName() { return "PlainTextValue";}
public:
	PlainTextValue (const std::string& str) : ExtendedString(str){}
	PlainTextValue(const char* str) : ExtendedString(str){}
};

class EncryptedValue : public ExtendedString
{
protected:
	inline std::string getBad() { return " ";}
	inline std::string getName() { return "EnecryptedValue";}
public:
	EncryptedValue(const std::string& str) : ExtendedString(str){}
	EncryptedValue(const char* str) : ExtendedString(str){}
};


#endif /* PLAINTEXTVALUE_H_ */
