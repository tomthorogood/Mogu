/*
 * Submission.h
 *
 *  Created on: Aug 1, 2012
 *      Author: tom
 */

#ifndef SUBMISSION_H_
#define SUBMISSION_H_

#include <declarations.h>
#include <crypt/PacketCenter.h>

#include <hash.h>
namespace Sessions{
namespace SubmissionHandler{

/*!\brief Formats a node key for lookup by hashing the 'storage' attribute
 *
 * @param session The session ID ("global" or numeric ID)
 * @param p_storage The plaintext storage node ("number_of_cats")
 * @return The node lookup string ("s.global.1245915847985")
 */

/* Uses the storage policy of the node in question to make an informed decision
 * about what to do with the data.
 */
void absorb(std::string value, std::string node);

/* Returns whether or not this widget needs to be encrypted before storage */
bool requiresEncryption(const std::string& snode);

/* Returns the storage mode of the widget from its policy entry */
Enums::SubmissionPolicies::StorageMode getStorageMode(
		const std::string& snode);

/* Returns the storage type of the widget data from its policy entry */
Enums::SubmissionPolicies::StorageType getStorageType(
		const std::string& snode);

/* Returns the data type of the widget data from its policy entry. */
Enums::SubmissionPolicies::DataWrapping getDataWrapping(
		const std::string& snode);

/* Retrieves the name of the hash field for input storage, if the storage
 * is to take place in another node.
 */
std::string getHashField(const std::string& snode);

/* When storing submission data in a static slot, as opposed to in the database,
 * it is necessary to get the slot name.
 */
std::string getSlotName(const std::string& snode);

std::string userNodeLookup(std::string sessionid, std::string storage, std::string arg=EMPTY, PacketType translation=DO_TRANSLATION);
std::string dynamicLookup(std::string storage, std::string arg=EMPTY,  PacketType translation=DO_TRANSLATION);


}//namespace Submission
}//namespace Sessions



#endif /* SUBMISSION_H_ */
