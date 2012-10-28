/*
 * ConnectionManager.h
 *
 *  Created on: Oct 27, 2012
 *      Author: tom
 */

#ifndef CONNECTIONMANAGER_H_
#define CONNECTIONMANAGER_H_

#include <Redis/RedisCore.h>

namespace Redis{

struct TemporaryConnection
{
	size_t connection_number;
	redisContext* context;
	redisReply* reply;

	TemporaryConnection(size_t conn, redisContext* c, redisReply* r)
	:	connection_number(conn)
	{
		context = c;
		reply = r;
	}
	~TemporaryConnection()
	{
		freeReplyObject(reply);
		redisFree(context);
	}
};

struct ConnectionConfiguration
{
	std::string host;
	std::string auth;
	size_t database_number;
	uint16_t port;

	ConnectionConfiguration(
			std::string d_host = "localhost"
			,std::string d_auth = EMPTY
			,size_t d_database_number = 0
			,uint16_t d_port = 6379
			)
	{
		host = d_host;
		auth = d_auth;
		database_number = d_database_number;
		port = d_port;
	}

	ConnectionConfiguration(const ConnectionConfiguration& copy)
	{
		host = copy.host;
		auth = copy.auth;
		database_number = copy.database_number;
		port = copy.port;
	}
};

class ConnectionManager
{
	std::map <size_t, TemporaryConnection* > __connections;
	size_t num_requests;
	ConnectionConfiguration& default_connection;

	size_t new_request()
	{
		std::map <size_t, TemporaryConnection*>::iterator iter
			= __connections.begin();
		for (
				size_t n = 0;
				n < num_requests, iter != __connections.end();
				++n, ++iter)
		{
			size_t iterpos = iter->first;
			if (iterpos > n) return n;
		}
		return ++num_requests;
	}

public:
	ConnectionManager(const ConnectionConfiguration& defaultConnection)
	: default_connection(defaultConnection), __connections()
	{
		num_requests = 0;
	}


	TemporaryConnection& requestConnection(
			std::string host
			,int port
			,std::string auth = EMPTY
			,size_t dbnum = 0
		)
	{
		redisContext* context = redisContext(host.c_str(),port);
		redisReply* reply = NULL;
		if (auth != EMPTY)
		{
			reply = (redisReply*) redisCommand(context, "auth %s", auth);
			freeReplyObject(reply);
		}
		reply = (redisReply*) redisCommand(context, "select %d", dbnum);
		size_t request = new_request();
		TemporaryConnection* conn = new TemporaryConnection(
				request,
				context,
				reply);
		__connections[request] = conn;
		return *conn;
	}

	TemporaryConnection& requestConnection (
			const ConnectionConfiguration& config = default_connection)
	{
		return requestConnection(
				config.host, config.port, config.auth, config.database_number);
	}

	inline void releaseConnection(size_t connection_number)
	{
		TemporaryConnection* c = __connections[connection_number];
		if (c != NULL)
		{
			delete c;
		}
		__connections.erase(connection_number);
	}

	~ConnectionManager()
	{
		std::map <size_t,TemporaryConnection*>::iterator iter
			= __connections.begin();
		while (iter != __connections.end())
		{
			if (iter->second != NULL) delete iter->second;
			++iter;
		}
	}
};

}


#endif /* CONNECTIONMANAGER_H_ */
