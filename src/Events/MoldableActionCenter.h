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
#include <Events/BroadcastMessage.h>

namespace Events
{

/*!\brief The ActionCenter namespace encompasses a static set of functions
 * that handle the transmission and handling of BroadcastMessages.
 */
namespace ActionCenter
{

typedef std::vector <Goo::Moldable*> Listeners;
typedef std::map <BroadcastMessage*, Listeners*> ListenerMap;

/*!\brief Degrades the message signal, determining whether it needs to
 * be repeated to another set of listeners, or if it's reached its final
 * destination.
 * @param broadcast The message to be broadcast.
 */
void submitBroadcast (BroadcastMessage& broadcast);


/*!\brief Refers to the current listenermap for the broadcast message and
 * updates it if necessary to refer to the next level of widgets.
 * @param broadcast The message currently being broadcast.
 */
void updateListeners(BroadcastMessage& broadcast);

/*!\brief Instructs listeners to take action based on the message content.
 * @param broadcast The message whose broadcast has reached its terminus.
 */
void directListeners(BroadcastMessage& broadcast);


/*!\brief Determines the nuclear family members of the current listeners
 * receiving the message;.
 * @param broadcast The Broadcast Message.
 */
void getNuclearFamily(BroadcastMessage& broadcast);

/*!\brief Deletes the broadcast signal and removes its entry fro the
 * ListenerMap.
 * @param broadcast The dying broadcast message.
 */
void cleanupBroadcast(BroadcastMessage& broadcast);

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

void set_index(Listeners* listeners, int& index);
void increment_index(Listeners* listeners);
void decrement_index(Listeners* listeners);
std::string get_user_email(std::string username);
int send_system_email(EmailPacket*);
int email_current_user(EmailPacket*);
bool reset_password(std::string username);
bool change_password(std::string username, std::string new_pass);
bool test(Goo::Moldable&,Nodes::NodeValue&);
} //namespace Actions

} //Namspace ActionCenter
} //Namespace Events

#endif /* MOLDABLEACTIONCENTER_H_ */
