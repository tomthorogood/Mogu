/*
 * ApplicationManager.cpp
 *
 *  Created on: Oct 21, 2012
 *      Author: tom
 */

#include <Types/ApplicationManager.h>
#include <TurnLeftLib/Utils/randomcharset.h>
#include <Security/LoginAuthenticator.h>
#include <Security/Security.h>
#include <Redis/RedisUtils.h>
#include <Mogu.h>
#include <hash.h>

using namespace TurnLeft::Utils;

static const std::string INIT_SESSION = "global";

ApplicationManager::ApplicationManager(
    Mogu& app)
    : application(app)
{
    application.setSessionID(INIT_SESSION);
}

void ApplicationManager::createNewSession(
    Security::AuthPackage& pkg)
{
    mApp;
    TokenCycles authtoken_info;
    std::string new_session_id = Redis::makeUniqueRChar(__NODE_SESSION_SET);
    std::string node = "s." + new_session_id + "." + __META_HASH;

    const char* cauth_str = pkg.proofed_auth_string.c_str();
    const char* cnewsession = new_session_id.c_str();
    const char* cnode = node.c_str();
    const char* clast_session = pkg.last_session.c_str();
    const char* cuserid = pkg.mogu_userid.c_str();

    /* Link the new session to the previous session */
    app->redisExec(Mogu::Discard, "hset %s %s %s", cnode, __PREV_HASH,
        clast_session);

    /* Link the user id with the new session */
    app->redisExec(Mogu::Discard, "hset %s %s %s", __NODE_SESSION_LOOKUP,
        cuserid, cnewsession);

    /* Add the new session to the session set for easy access */
    app->redisExec(Mogu::Discard, "sadd %s %s", __NODE_SESSION_SET,
        cnewsession);

    /* Create anew temporary auth token */
    std::string new_auth_tok = Redis::makeUniqueRChar(__NODE_AUTH_TOK_SET);
    const char* cnew_auth_tok = new_auth_tok.c_str();

    app->redisExec(Mogu::Discard, "hset %s %s %s", cnode, __AUTH_HASH,
        cnew_auth_tok);

    //Add the new auth token to the auth token set, and remove the previous one. */
    app->redisExec(Mogu::Discard, "sadd %s %s", __NODE_AUTH_TOK_SET,
        cnew_auth_tok);

    // Do not attempt to remove the previous auth token if the previous session was "global"
    if (pkg.last_session != "global") {
        std::string query = "s." + pkg.last_session + "." + __META_HASH;
        const char* cquery = query.c_str();
        app->redisExec(Mogu::Keep, "hget %s %s", cquery, __AUTH_HASH);
        std::string prev_auth = redisReply_STRING;
        const char* cauth = prev_auth.c_str();
        app->redisExec(Mogu::Discard, "srem %s %s", __NODE_AUTH_TOK_SET, cauth);
    }

    //Link the new auth token to the user's auth string
    app->redisExec(Mogu::Discard, "hset %s %s %s", __NODE_AUTH_LOOKUP,
        cauth_str, cnew_auth_tok);
    app->setSessionID(new_session_id);
}
