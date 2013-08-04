#ifndef USERMANAGER_H_
#define USERMANAGER_H_

#include <TurnLeftLib/Utils/randomcharset.h>
#include <string>
#include "../Redis/MoguQueryHandler.h"
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
            std::string& username, const std::string& password);

    SecurityStatus loginUser(
            std::string& username, const std::string& password);

    SecurityStatus deleteUser(int userid);

    /* For a user that's already logged in, allows them to change
     * their password.
     */
    void changePassword(const std::string& new_password); 

    /* For a user that cannot login, resets their password to
     * something else, and emails it to them.
     */
    SecurityStatus resetPassword(int userid, std::string email,
            const std::string& appname);
    SecurityStatus resetPassword(std::string& userid,
            const std::string& email, const std::string& appname);

    inline const int& getUser() { return userid;}

private:
    std::string sanitizePassword(
            const std::string& password, const std::string& salt);

    inline std::string generateSalt()
    {
        TurnLeft::Utils::RandomCharSet rchar;
        return rchar.generate(1);
    }

    std::string getUserSalt(std::string userid, Redis::MoguQueryHandler* db =nullptr);
    std::string getUserSalt(const int& userid, Redis::MoguQueryHandler* db=nullptr);
    std::pair <std::string,std::string>
        getSanitizedInfo(std::string& uname, const std::string& password);
    std::string getSalt(const std::string& username);
    int getUserId(const std::string& username);

    /* Either returns an abandoned userid, or creates a new one. */
    int consumeUserId(Redis::MoguQueryHandler&);
    int userid = -1;
};

#endif
