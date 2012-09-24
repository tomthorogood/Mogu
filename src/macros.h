/*
 * macros.h
 *
 *  Created on: Sep 19, 2012
 *      Author: tom
 */

#ifndef MACROS_H_
#define MACROS_H_

#define WIDGET_HO_BITS 0x3f
#define MAX_TOKENS 3
#define EMPTY ""
#define NONE 0

//Pre-hashed values to speed up common queries:
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

//Pre-glued node values to speed up common queries:
#define __NODE_GLOBAL_ "s.global."

// encrypted_username -> last_session
#define __NODE_SESSION_LOOKUP __NODE_GLOBAL_ __SESSION_LOOKUP_HASH
#define __NODE_COLLISION_SES_LOOKUP __NODE_SESSION_LOOKUP"."__COLLISION_HASH

// collisionproof(encrypted_authstring) -> collisionproof(last_authtoken)
#define __NODE_AUTH_LOOKUP __NODE_GLOBAL_ __AUTH_LOOKUP_HASH

// encrypted_username -> user_salt
#define __NODE_SALT_LOOKUP __NODE_GLOBAL_ __SALT_HASH

// encrypted_username -> #collisions(encrypted_authstring)
#define __NODE_COLLISION_STR_LOOKUP __NODE_GLOBAL_ __COLLISION_HASH"."__AUTHSTR_HASH

// collisionproof(encrypted_authstring) -> #collisions(last_authtoken)
#define __NODE_COLLISION_TOK_LOOKUP __NODE_GLOBAL_ __COLLISION_HASH"."__AUTHTOK_HASH

#define __NODE_ALL_AUTHS __NODE_GLOBAL_ __ALL_AUTHS_HASH

#endif /* MACROS_H_ */
