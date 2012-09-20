/*
 * MoldableActionCenter.h
 *
 *  Created on: Jul 12, 2012
 *      Author: tom
 */

#ifndef MOLDABLEACTIONCENTER_H_
#define MOLDABLEACTIONCENTER_H_

#include <declarations.h>
#include <Core/Moldable.h>
#include <Events/EventNodeExtractor.h>
#include <Events/EventNodeProcessor.h>

namespace Events
{

/*!\brief The ActionCenter namespace encompasses a static set of functions
 * that handle the transmission and handling of BroadcastMessages.
 */
namespace ActionCenter
{

typedef TurnLeft::Utils::HungryVector <Goo::Moldable*> Listeners;
typedef std::map <BroadcastMessage*, Listeners*> ListenerMap;

/*!\brief Degrades the message signal, determining whether it needs to
 * be repeated to another set of listeners, or if it's reached its final
 * destination.
 * @param broadcast The message to be broadcast.
 */
void submitBroadcast (BroadcastMessage* broadcast);

/*!\brief Creates and returns a new Broadcast Message object, which will
 * not be deleted until it is successfully processed.
 * @param broadcaster The Moldable widget that originally sent the message.
 * @param extractor The already created extraction unit that is holding the
 * string variables extracted from the database.
 * @param processor The EventNodeProcessor that may already have translated
 * some of the widget's properties -- this will avoid reprocessing.
 * @return A new Broadcast Message package.
 */
BroadcastMessage* generateNewBroadcast(
        Goo::Moldable* broadcaster,
        EventNodeExtractor& extractor,
        EventNodeProcessor*);

/*!\brief An overloaded method which does not require a processing unit, but
 * creates one instead. This is will create the processing unit and call the
 * complete submitBroadcast method.
 * @param broadcaster The Moldable widget that originally sent the message.
 * @param extractor The already created extraction unit that is holding
 * the string variables extracted from the database.
 * @return The new broadcast message (sent from the completed method above).
 */
BroadcastMessage* generateNewBroadcast(
        Goo::Moldable* broadcaster,
        EventNodeExtractor& extractor);

/*!\brief If the broadcast is a propagates or catalyzed relay, and the
 * original signal has degraded fully, a relayBroadcast will be generated.
 * @param broadcast The message currently being broadcast.
 * @param nodeName the name of the node containing the widget's properties.
 */
BroadcastMessage* generateNewBroadcast(
        Goo::Moldable* broadcaster,
        std::string& nodeName);

/*!\brief Refers to the current listenermap for the broadcast message and
 * updates it if necessary to refer to the next level of widgets.
 * @param broadcast The message currently being broadcast.
 */
void updateListeners(BroadcastMessage* broadcast);

/*!\brief Instructs listeners to take action based on the message content.
 * @param broadcast The message whose broadcast has reached its terminus.
 */
void directListeners(BroadcastMessage* broadcast);


/*!\brief Determines the nuclear family members of the current listeners
 * receiving the message;.
 * @param broadcast The Broadcast Message.
 */
void getNuclearFamily(BroadcastMessage* broadcast);

/*!\brief Deletes the broadcast signal and removes its entry fro the
 * ListenerMap.
 * @param broadcast The dying broadcast message.
 */
void cleanupBroadcast(BroadcastMessage* broadcast);

//_______ACTION RESOLUTIONS ________//
namespace Actions{

struct EmailPacket
{
	std::string to_address;
	std::string subject;
	std::string message;

	EmailPacket()
	{
		to_address = EMPTY;
		subject = EMPTY;
		message = EMPTY;
	}
};


bool change_session ();
bool register_user ();

void set_index(Listeners* listeners, BroadcastMessage* broadcast);
void increment_index(Listeners* listeners);
void decrement_index(Listeners* listeners);
std::string get_user_email(std::string username);
int send_system_email(EmailPacket*);
int email_current_user(EmailPacket*);
bool reset_password(std::string username);
bool change_password(std::string username, std::string new_pass);
} //namespace Actions

} //Namspace ActionCenter
} //Namespace Events

#endif /* MOLDABLEACTIONCENTER_H_ */
