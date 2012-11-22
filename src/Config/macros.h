/*
 * macros.h
 *
 *  Created on: Sep 19, 2012
 *      Author: tom
 */

#ifndef MACROS_H_
#define MACROS_H_

#define DEFAULT_GROUP "mogu_users"

#define WIDGET_HO_BITS 0x3f
#define MAX_TOKENS 3
#define EMPTY ""
#define NONE 0

#define __PREFIX_GROUP "g."
#define __PREFIX_SESSION "s."
#define __PREFIX_WIDGET "widgets."


#define MoguApp (Mogu*) wApp

//Provides access to the user-specific instance of Mogu
#define mApp Mogu* app = MoguApp

#define redisReply_TRUE  (bool)Redis::getInt(app->reply())
#define redisReply_STRING Redis::toString(app->reply())
#define redisReply_INT Redis::getInt(app->reply())

//Pre-hashed values to speed up common queries -- DO NOT CHANGE:
#define __DEFAULT_GROUP_HASH "1448036143964893002"
#define __AUTH_HASH "8163637335359820509"
#define __META_HASH "9532958398116805642"
#define __PREV_HASH "653035263252793737"
#define __SALT_HASH "432302536154660963"
#define __SESSION_LOOKUP_HASH "6250022396821446324"
#define __AUTH_LOOKUP_HASH "4889342676755297139"
#define __COLLISION_HASH "1116545542298320121"
#define __AUTHSTR_HASH "12729235922454487927"
#define __AUTHTOK_HASH "1031436179838087849"
#define __ALL_AUTHS_HASH "4801973836335212534"
#define __SET_HASH "10275542420063459543"
#define __USERID_LOOKUP_HASH "4651336642905166632"
#define __GROUPS_HASH "6785144517405584032"
#define __MEMBERSHIP_HASH "630696924263564959"

//Pre-glued node values to speed up common queries:
#define __NODE_GLOBAL_ "s.global."

// encrypted_username -> last_session
#define __NODE_SESSION_LOOKUP __NODE_GLOBAL_ __SESSION_LOOKUP_HASH

#define __NODE_USERID_LOOKUP __NODE_GLOBAL_ __USERID_LOOKUP_HASH

// { Session^1 ... Session^n }
#define __NODE_SESSION_SET __NODE_GLOBAL_ __SESSION_LOOKUP_HASH "." __SET_HASH

#define __NODE_AUTH_STR_SET __NODE_GLOBAL_ __AUTHSTR_HASH "." __SET_HASH

// { Token^1 ... Token^n }
#define __NODE_AUTH_TOK_SET __NODE_GLOBAL_ __AUTHTOK_HASH "." __SET_HASH

#define __NODE_USERID_SET __NODE_GLOBAL_ __USERID_LOOKUP_HASH "." __SET_HASH

// collisionproof(encrypted_authstring) -> collisionproof(last_authtoken)
#define __NODE_AUTH_LOOKUP __NODE_GLOBAL_ __AUTH_LOOKUP_HASH

// encrypted_username -> user_salt
#define __NODE_SALT_LOOKUP __NODE_GLOBAL_ __SALT_HASH

// encrypted_username -> #collisions(encrypted_authstring)
#define __NODE_COLLISION_STR_LOOKUP __NODE_GLOBAL_ __COLLISION_HASH "." __AUTHSTR_HASH

#define __NODE_DEFAULT_MEMBERSHIP "g." __DEFAULT_GROUP_HASH "." __MEMBERSHIP_HASH

#endif /* MACROS_H_ */
