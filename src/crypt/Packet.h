#ifndef PACKET_H_
#define PACKET_H_

#include <TurnLeftLib/TurnLeft.h>
#include <string>

typedef unsigned char* CharArray;
typedef const char* c_str;

class Packet
{
private:
	CharArray data;
public:
	const static char SIZE = 8;

	Packet(CharArray input);
	Packet(c_str input);
	Packet(std::string input);

	inline CharArray getData() const
	{
		return data;
	}
	std::string str() const;
	std::string int_str() const;
};

#endif
