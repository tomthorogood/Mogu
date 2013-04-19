/*
 * Static.cpp
 *
 *  The contents of this file detail settings and methods that are global
 *  to all instances of the Mogu app deployed at any given time (this is
 *  in contrast to the 'Mogu' class, which has a distinct instnce for every
 *  connected user).
 *
 *  Created on: Jul 29, 2012
 *      Author: tom
 */

#include <Static.h>
#include <crypt/BlowfishKey.h>
#include <crypt/PacketCenter.h>
#include <Exceptions/Exceptions.h>
#include <Redis/RedisCore.h>
#include <Parsers/TokenGenerator.h>
#include <TurnLeftLib/Utils/inlines.h>
#include <iostream>

#include <Mogu.h>

namespace Application {

namespace {
static bool configured = false;
}

void setUserLevels()
{

}

std::string getUserLevel(
    const int& index)
{
    return "";
}

int getUserLevel(
    const std::string& name)
{
    return 0;
}

void configure()
{
    if (configured) return;
    setUserLevels();
    configured = true;
}

}    //namespace Application

