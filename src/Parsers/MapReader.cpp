/*
 * MapReader.cpp
 *
 *  Created on: Dec 2, 2012
 *      Author: tom
 */

#include "MapParser.h"

MapReader::MapReader(
    const char keyDelimiter
    , const char valDelimiter)
{
    key_delimiter = keyDelimiter;
    val_delimiter = valDelimiter;

    __cpos__ = 0;
    __vsiz__ = 0;
}

void MapReader::parseKey()
{
    __cpos__ = line.find_first_of(key_delimiter);

    if (__cpos__ == std::string::npos) {
        __cpos__ = 0;
        return;     //TODO throw error
    }

    key = line.substr(0, __cpos__);
    ++__cpos__;     //consume the delimiter
}

void MapReader::parseNextValue()
{
    std::string value;

    if (!positionIsValid()) return;

    // Create the next 'slice'
    size_t start = line.find_first_of(val_delimiter, __cpos__);
    __cpos__ = line.find_first_of(val_delimiter, start);

    ++start;        //consume the delimiter

    // If we've reached the last value in the list, return the rest of
    // the token as one string
    if (__cpos__ == std::string::npos) {
        value = line.substr(start, line.length()-start);
    }
    else {
        size_t length = __cpos__-start+1;
        value = line.substr(start, length);
        ++__cpos__; //consume the delimiter
    }

    values.push_back(value);

}

void MapReader::readLine (const std::string& line)
{
    reset();
    this->line = line;
    parseKey();
    do {
        parseNextValue();
    } while (valuesIncreased());
}



