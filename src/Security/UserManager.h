/*
 * UserManager.h
 *
 *  Created on: Apr 8, 2013
 *      Author: tom
 */

#ifndef USERMANAGER_H_
#define USERMANAGER_H_

#include <declarations.h>
#include <Redis/ContextQuery.h>

/*!\brief Allows an application instance to log a user into the system.
 *
 */
class UserManager {
public:
    UserManager();
    /*!\brief Attempts to log in a user given the username and password,
     * returning a status code which can be handled by the calling class.
     * 1    - The login was successful
     * 0    - The user's credentials were incorrect
     * -1   - There was an error while attempting to authenticate the user
     *
     * If successful, the userKeyspace variable is set.
     * \sa userKeyspace
     *
     */
    int8_t loginUser(const std::string& username, const std::string& password);

    /*!\brief Attempts to register a user with the given name and password.
     * The function returns a status code which can be handled by the calling
     * class.
     *
     * 1    - Registration was successful
     * 0    - The user already exists within the system
     * -1   - There was an error while attempting to register the user
     */
    int8_t registerUser(const std::string& username, const std::string& password);

    inline const int& getUser() const {return userKeyspace;}
    
    bool setActiveGroup(const std::string& groupid){return true;} //TODO

private:
    Redis::ContextQuery db;
    /*!\brief The address within the user database where this user's
     * information lives.
     */
    int userKeyspace    =0;
    /*!\brief Given a username, password, and salt, creates a
     * collision proofed hash of user data.
     *
     * encrypted(username) + encrypted(salt) + encrypted(password)
     *      => hashed together (x hash_iters)
     */
    std::string hashTogether(
        const std::string& username     //encrypted
        , const std::string& password   //encrypted
        , const std::string& salt       //encrypted
        , const int& hash_iters);

    std::string createHash(
        const std::string& username     //encrypted
        , const std::string& password   //encrypted
        , const std::string& salt       //encrypted
    );


    bool keyspaceExists(const std::string& userhash);
};


#endif /* USERMANAGER_H_ */
