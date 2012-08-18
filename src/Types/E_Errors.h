/*
 * E_Errors.h
 *
 *  Created on: Aug 18, 2012
 *      Author: tom
 */

#ifndef E_ERRORS_H_
#define E_ERRORS_H_

/*! \brief Possible states of the hashable answer properties */
namespace HashableAnswer_ErrorFlags{
enum HashableAnswer_ErrorFlags {
    is_hashable,                    // A value of 0 means no error flags!
    err_no_hashable_data,           // Only the database was given
    err_no_hashable_question,       // Not enough question data given
    err_no_hashable_form,           // Not enough form data given
    err_no_hashable_answer,         // Not enough answer data given
    err_no_database_selected,       // No database data given
    err_hash_already_set,           // Don't hash a pre-hashed hash!
    err_unknown                     // What did you do?!
};
} //namespace HashableAnswer_ErrorFlags



#endif /* E_ERRORS_H_ */
