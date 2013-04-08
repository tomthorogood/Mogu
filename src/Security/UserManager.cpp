/*
 * UserManager.cpp
 *
 *  Created on: Apr 8, 2013
 *      Author: tom
 */

#include "UserManager.h"
#include "Encryption.h"

#include <TurnLeftLib/Utils/randomcharset.h>
#include <hash.h>

UserManager::UserManager()
:   db(Prefix::user)
{
}

int8_t UserManager::loginUser(
    const std::string& username, const std::string& password)
{
    using Redis::Query;
    using Redis::ContextQuery::REQUIRE_INT;
    using Redis::ContextQuery::REQUIRE_STRING;
    std::string enc_username = Security::encrypt(username);
    std::string enc_password = Security::encrypt(password);

    const char* c_enc_username = enc_username.c_str();

    /* First, we have to see whether the encrypted username exists at all:*/
    std::shared_ptr <Query> userexists = std::make_shared <Query>(
        new Query("hexists user.iterations %s", c_enc_username));

    /* After that, we'll see how many times we have to hash their information
     * to prevent a collision with another user's keyspace.
     */
    std::shared_ptr <Query> userhashiter = std::make_shared <Query>(
        new Query("hget user.iterations %s", c_enc_username));

    /* Then we'll retrieve their salt. */
    std::shared_ptr <Query> usersalt = std::make_shared <Query>(
        new Query("hget user.salt %s", c_enc_username));

    db.appendQuery(userexists, REQUIRE_INT);
    db.appendQuery(userhashiter, REQUIRE_INT);
    db.appendQuery(usersalt, REQUIRE_STRING);

    if (! db.yieldResponse <bool>()) return 0;
    int hash_iters = db.yieldResponse <int>();
    std::string salt = db.yieldResponse <std::string>();
    salt = Security::encrypt(salt);

    userKeyspace = hashTogether(
        enc_username, salt, enc_password, hash_iters);
    std::shared_ptr <Query> keyspace_exists = std::make_shared <Query>(
        new Query("hexists user.%s", userKeyspace));
    db.appendQuery(keyspace_exists, REQUIRE_INT);
    if (!db.yieldResponse <bool> ())
    {
        userKeyspace = EMPTY;
        return 0;
    }
    else
    {
        std::shared_ptr <Query> salt_matches = std::make_shared <Query>(
            new Query("get user.%s", enc_username));
        db.appendQuery(salt_matches, REQUIRE_STRING);
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
    using Redis::Query;
    using Redis::QuerySet::REQUIRE_INT;
    using Redis::QuerySet::IGNORE_RESPONSE;

    std::string enc_username = Security::encrypt(username);
    std::string enc_password = Security::encrypt(password);

    RandomCharSet rchar;
    std::string salt = rchar.generate(1); // three characters long
    std::string enc_salt = Security::encrypt(salt);

    std::shared_ptr <Query> user_already_exists = std::make_shared <Query>(
        new Query("hexists user.salt %s", enc_username.c_str()));

    std::shared_ptr <Query> store_salt = std::make_shared <Query>(new Query
        ("hset user.salt %s %s", enc_username.c_str(), enc_salt.c_str()));

    db.appendQuery(user_already_exists, REQUIRE_INT);
    db.appendQuery(store_salt, IGNORE_RESPONSE);
    if (db.yieldResponse <bool>()) return 0;
    db.execute(); //store the salt

    userKeyspace = createHash(enc_username, enc_password, enc_salt);
    if (userKeyspace != EMPTY)
    {
        // We'll store the encrypted user salt at that location in
        // the database to let future calls know that the user was created,
        // and also allow us to verify that there hasn't been a collision error
        std::shared_ptr <Query> create_keyspace =
            std::make_shared <Query> (new Query("set user.%s %s",
                userKeyspace, enc_salt));
        db.appendQuery(create_keyspace, IGNORE_RESPONSE);
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
    using Redis::Query;
    using Redis::QuerySet::IGNORE_RESPONSE;
    std::string long_source = username + salt + password;
    std::string hashed = Hash::toHash(long_source);

    int num_iters = 0;
    while (keyspaceExists(hashed))
    {
        hashed = Security::encrypt(hashed);
        hashed = Hash::toHash(hashed);
        ++num_iters;
    }

    std::shared_ptr <Query> set_num_iters = std::make_shared <Query>(
        new Query("hset user.iterations %s %d", username, num_iters));
    db.appendQuery(set_num_iters, IGNORE_RESPONSE);
    db.execute();

    return hashed;
}

bool UserManager::keyspaceExists(const std::string& userhash)
{
    using Redis::Query;
    std::shared_ptr <Query> hash_exists = std::make_shared <Query> (new Query(
        "hexists user.%s", userhash.c_str()));
    db.appendQuery(hash_exists);
    return db.yieldResponse<bool>();
}
