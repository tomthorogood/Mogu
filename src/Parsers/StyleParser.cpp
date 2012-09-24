/*
 * _StyleParser.cpp
 *
 *  Created on: Jul 15, 2012
 *      Author: tom
 */

#include <Parsers/StyleParser.h>
#include <Parsers/Parsers.h>
#include <Parsers/NodeValueParser.h>
#include <Types/NodeValue.h>
#include <sstream>
#include <Wt/WAnimation>
#include <TurnLeftLib/Utils/explosion.h>

namespace Parsers
{
namespace StyleParser
{
using std::string;
using Goo::Moldable;

mapped getActionBlock(std::string nodeName)
{
    uint8_t block   =0;
    string block_str = getWidgetProperty(nodeName, "block");
    string block_arr[Enums::SignalActions::NUM_ACTIONS];
    TurnLeft::Utils::Explosion explosion(block_str);
    explosion.explode(' ',block_arr);
    int num_blocks = explosion.getNumWords();

    for (int b = 0; b < num_blocks; b++)
    {
        uint8_t action =0;
        Nodes::NodeValue val;
        Parsers::NodeValueParser parser(block_arr[b], &val, NONE,
                &Parsers::enum_callback <Parsers::SignalActionParser>);
        action = (uint8_t) val.getInt();
        block |= action;
    }
    return block;
}

} //namespace StyleParser
} //namespace Parsers
