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

void absorb(Goo::Dynamic* widget);
bool requiresEncryption(Goo::Dynamic* inputWidget);
Enums::SubmissionPolicies::StorageMode getStorageMode(
		Goo::Dynamic* inputWidget);

Enums::SubmissionPolicies::StorageType getStorageType(
		Goo::Dynamic* inputWidget);

Enums::SubmissionPolicies::DataWrapping getDataWrapping(
		Goo::Dynamic* inputWidget);

std::string getHashField(Goo::Dynamic* inputWidget);

}//namespace Submission
}//namespace Sessions



#endif /* SUBMISSION_H_ */
