/*
 * macros.h
 *
 *  Created on: Sep 19, 2012
 *      Author: tom
 */

#ifndef MACROS_H_
#define MACROS_H_

#define DEFAULT_GROUP "mogu_users"

#define EMPTY ""
#define NONE 0

#define MoguApp static_cast<Mogu*>(wApp)

//Provides access to the user-specific instance of Mogu
#define mApp Mogu* app = MoguApp

#define CreateQuery(db,...) db.appendQuery(\
        std::make_shared <Redis::Query>( Redis::Query(__VA_ARGS__)))

#endif /* MACROS_H_ */
