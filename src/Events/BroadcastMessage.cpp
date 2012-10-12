/*SN_2_0_FF_PC
 * BroadcastMessage.cpp
 *
 *  Created on: Jul 12, 2012
 *      Author: tom
 */

#include <Events/BroadcastMessage.h>
#include <Core/Moldable.h>
#include <Events/EventPreprocessor.h>
#include <Parsers/NodeValueParser.h>
#include <stdio.h>

namespace Events{
using std::string;
using Goo::Moldable;

namespace Listeners = Enums::Family;
namespace Action = Enums::SignalActions;
namespace Labels = Enums::Labels;
namespace SignalType = Enums::SignalTypes;
namespace TypeBit = Enums::SignalTypes;

BroadcastMessage::BroadcastMessage (
		Goo::Moldable* _broadcaster,
		EventPreprocessor* preproc)
{
	properties = preproc;
	broadcaster = _broadcaster;

	/* If the message type was anything other than a standard string,
	 * we will now process that message.
	 */
	if (preproc->message.status == preproc->message.delayed)
	{
#ifdef DEBUG
		std::cout << "Parsing delayed message: " << preproc->message.original << std::endl;
#endif
		Parsers::NodeValueParser p(
				preproc->message.original
				,&preproc->message.value
				,broadcaster);
	}
}

void BroadcastMessage::updateAction()
{
	if (properties->next_action == 0) return;

	Enums::SignalActions::SignalAction current = properties->action;
	properties->action = (properties->next_action != 0) ?
			properties->next_action
			:properties->action;
	properties->next_action = current;
}
}//namespace Events

