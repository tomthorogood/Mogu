/*
 * Bindery.h
 *
 *  Created on: Sep 28, 2012
 *      Author: tom
 */

#ifndef BINDERY_H_
#define BINDERY_H_

#include <declarations.h>
#include <Parsers/NodeValueParser.h>


namespace Goo
{
namespace MoldableFactory
{

using namespace Enums::SignalTriggers;
using Parsers::NodeValueParser;

void bind_all_events(Moldable* widget);
void bind_event (const std::string&,Moldable*);

}
}


#endif /* BINDERY_H_ */
