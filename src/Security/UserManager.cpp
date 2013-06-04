/*
 * UserManager.cpp
 *
 *  Created on: Apr 8, 2013
 *      Author: tom
 */

#include "UserManager.h"
#include "Encryption.h"
#include <Redis/ContextQuery.h>
#include <TurnLeftLib/Utils/randomcharset.h>
#include <hash.h>

UserManager::UserManager()
:   db(Prefix::user)
{
}

int8_t UserManager::loginUser(
    const std::string& username, const std::string& password)
{
    std::string enc_username = Security::encrypt(username);
    std::string enc_password = Security::encrypt(password);

    const char* c_enc_username = enc_username.c_str();

    /* First, we have to see whether the encrypted username exists at all:*/
    db.appendQuery( "hexists user.iterations %s", c_enc_username);
    /* After that, we'll see how many times we have to hash their information
     * to prevent a collision with another user's keyspace.
     */
    db.appendQuery( "hget user.iterations %s", c_enc_username);
    /* Then we'll retrieve their salt. */
    db.appendQuery( "hget user.salt %s", c_enc_username);


    if (! db.yieldResponse <bool>()) return 0;
    int hash_iters = db.yieldResponse <int>();
    std::string salt = db.yieldResponse <std::string>();
    salt = Security::encrypt(salt);

    userKeyspace = hashTogether(
        enc_username, salt, enc_password, hash_iters);
    db.appendQuery(Redis::Query(
        "hexists user.%s", userKeyspace.c_str()), db.REQUIRE_INT);
    if (!db.yieldResponse <bool> ())
    {
        userKeyspace = EMPTY;
        return 0;
    }
    else
    {
        db.appendQuery(
            "get user.%s", enc_username.c_str(), db.REQUIRE_STRING);
        std::string auth_salt = db.yieldResponse<std::string>();
        int retval = (auth_salt == salt) ? 1 : -1;
        return retval;
    }

    return -1;
}

int8_t UserManager::registerUser(
    const std::string& username, const std::string& password)
{
    using TurnLeft::Utils::RandomCharSet;

    std::string enc_username = Security::encrypt(username);
    std::string enc_password = Security::encrypt(password);

    RandomCharSet rchar;
    std::string salt = rchar.generate(1); // three characters long
    std::string enc_salt = Security::encrypt(salt);

    db.appendQuery(
        Redis::Query("hexists user.salt %s", enc_username.c_str()), 
            db.REQUIRE_INT);

    db.appendQuery(
        Redis::Query("hse user.salt %s %s", enc_username.c_str(), enc_salt.c_str()),
        db.IGNORE_RESPONSE);

    if (db.yieldResponse <bool>()) return 0;
    db.execute(); //store the salt

    userKeyspace = createHash(enc_username, enc_password, enc_salt);
    if (userKeyspace != EMPTY)
    {
        // We'll store the encrypted user salt at that location in
        // the database to let future calls know that the user was created,
        // and also allow us to verify that there hasn't been a collision error
        db.appendQuery(Redis::Query(
            "set user.%s %s", userKeyspace.c_str(), enc_salt.c_str()),
            db.IGNORE_RESPONSE);
        db.execute();
        return 1;
    }

    return -1;
}

std::string UserManager::hashTogether(
    const std::string& username
    , const std::string& password
    , const std::string& salt
    , const int& hash_iters)
{
    std::string long_source = username + salt + password;
    std::string hashed = Hash::toHash(long_source);

    for (int iter = 0; iter < hash_iters; ++iter) {
        hashed = Security::encrypt(hashed);
        hashed = Hash::toHash(hashed);
    }

    return hashed;
}

std::string UserManager::createHash(
    const std::string& username
    , const std::string& password
    , const std::string& salt
    )
{
    std::string long_source = username + salt + password;
    std::string hashed = Hash::toHash(long_source);

    int num_iters = 0;
    while (keyspaceExists(hashed))
    {
        hashed = Security::encrypt(hashed);
        hashed = Hash::toHash(hashed);
        ++num_iters;
    }

    db.appendQuery(
        Redis::Query("hset user.iterations %s %d", username.c_str(), num_iters),
        db.IGNORE_RESPONSE);
    db.execute();

    return hashed;
}

bool UserManager::keyspaceExists(const std::string& userhash)
{
    db.appendQuery("hexists user.%s", userhash.c_str());
    return db.yieldResponse<bool>();
}
