#ifndef USERMANAGER_H_
#define USERMANAGER_H_

#include <declarations.h>
#include <TurnLeftLib/Utils/randomcharset.h>

enum class SecurityStatus
{
    ERR_UNKNOWN
    , ERR_USER_EXISTS
    , ERR_USER_NOT_FOUND
    , ERR_BAD_AUTH
    , OK_REGISTER
    , OK_LOGIN
    , OK_PASSWORD_RESET
};

class UserManager {
public:

    UserManager() {}
    SecurityStatus registerUser(
            const std::string& username, const std::string& password);

    SecurityStatus loginUser(
            const std::string& username, const std::string& password);

    SecurityStatus deleteUser(int userid);

    /* For a user that's already logged in, allows them to change
     * their password.
     */
    void changePassword(const std::string& new_password); 

    /* For a user that cannot login, resets their password to
     * something else, and emails it to them.
     */
    SecurityStatus resetPassword(int userid, std::string email);
    SecurityStatus resetPassword(std::string userid, const std::string& email);

    inline const int& getUser() { return userid;}

private:
    std::string sanitizePassword(
            const std::string& password, const std::string& salt);

    inline std::string generateSalt()
    {
        TurnLeft::Utils::RandomCharSet rchar;
        return rchar.generate(1);
    }

    std::string getUserSalt(std::string userid, Redis::ContextQuery* db =nullptr);
    std::string getUserSalt(const int& userid, Redis::ContextQuery* db=nullptr);

    /* Either returns an abandoned userid, or creates a new one. */
    int consumeUserId(Redis::ContextQuery&);
    int userid = -1;
};

#endif
