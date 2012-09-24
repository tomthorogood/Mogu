/*
 * E_Submissions.h
 *
 *  Created on: Aug 18, 2012
 *      Author: tom
 */

#ifndef E_SUBMISSIONS_H_
#define E_SUBMISSIONS_H_

namespace SubmissionPolicies{
enum StorageMode{
	append,
	replace
};

enum StorageType{
	list,
	string,
	hash
};

enum DataWrapping{
	none,
	enumerated_type,
	static_node,
	dynamic_node,
	integral_type,
	floating_type,
	file
};

}



#endif /* E_SUBMISSIONS_H_ */
