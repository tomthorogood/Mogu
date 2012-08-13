/*
 * Submission.h
 *
 *  Created on: Aug 1, 2012
 *      Author: tom
 */

#ifndef SUBMISSION_H_
#define SUBMISSION_H_

#include <declarations.h>

namespace Sessions{
namespace SubmissionHandler{

/* Uses the storage policy of the node in question to make an informed decision
 * about what to do with the data.
 */
void absorb(Goo::Dynamic* widget);

/* Returns whether or not this widget needs to be encrypted before storage */
bool requiresEncryption(Goo::Dynamic* inputWidget);

/* Returns the storage mode of the widget from its policy entry */
Enums::SubmissionPolicies::StorageMode getStorageMode(
		Goo::Dynamic* inputWidget);

/* Returns the storage type of the widget data from its policy entry */
Enums::SubmissionPolicies::StorageType getStorageType(
		Goo::Dynamic* inputWidget);

/* Returns the data type of the widget data from its policy entry. */
Enums::SubmissionPolicies::DataWrapping getDataWrapping(
		Goo::Dynamic* inputWidget);

/* Retrieves the name of the hash field for input storage, if the storage
 * is to take place in another node.
 */
std::string getHashField(Goo::Dynamic* inputWidget);

/* When storing submission data in a static slot, as opposed to in the database,
 * it is necessary to get the slot name.
 */
std::string getSlotName(Goo::Dynamic* inputWidget);



}//namespace Submission
}//namespace Sessions



#endif /* SUBMISSION_H_ */
