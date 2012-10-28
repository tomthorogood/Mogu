/*
 * AccessLayer.h
 *
 *  Created on: Oct 27, 2012
 *      Author: tom
 */

#ifndef ACCESSLAYER_H_
#define ACCESSLAYER_H_

#include <declarations.h>
#include <Redis/ConnectionManager.h>

namespace Redis{

class AccessLayer
{

public:
	enum Keyspaces {
		none
		,widgets
		,policies
		,sessions
		,data
		,global_events
		,templates
		,meta
	};
	std::map <Keyspaces, std::string> __prefixes;

	AccessLayer(TemporaryConnection& conn, Keyspaces keyspace = none)
	: connection(conn)
	{
		__keyspace = keyspace;
		__readPattern = __constructReadPattern();
		__writePattern = __constructWritePattern();
	}

	inline void readRequest(uint8_t pattern, ...)
	{
		const char* format = __readPattern[pattern].c_str();
		va_list ap;
		va_start(ap,pattern);
		__execute__(format, ap);
		va_end(ap);
	}

	inline void writeRequest(uint8_t pattern, ...)
	{
		const char* format = __writePattern[pattern].c_str();
		va_list ap;
		va_start(ap,pattern);
		__execute__(format,ap);
		va_end(ap);
	}

private:
	TemporaryConnection& connection;
	std::vector <std::string> __readPattern;
	std::vector <std::string> __writePattern;
	Keyspaces __keyspace;
	std::string __constructReadPattern() =0;
	std::string __constructWritePattern() =0;

	inline void __execute__(const char* format, va_list ap)
	{
		connection.reply = redisvCommand(connection.context, format, ap);
	}
};

static std::map <AccessLayer::Keyspaces, std::string> keyspacePrefixes =
		create_map <AccessLayer::Keyspaces, std::string>
		(AccessLayer::widgets,"widgets")
		(AccessLayer::policies,"policies")
		(AccessLayer::sessions,"s")
		(AccessLayer::data,"data")
		(AccessLayer::global_events,"events")
		(AccessLayer::templates,"templates")
		(AccessLayer::meta,"meta")
;

}//namespace Redis


#endif /* ACCESSLAYER_H_ */
