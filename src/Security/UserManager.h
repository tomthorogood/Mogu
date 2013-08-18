#ifndef USERMANAGER_H_
#define USERMANAGER_H_

#include <TurnLeftLib/Utils/randomcharset.h>
#include <string>
#include "../Redis/MoguQueryHandler.h"
enum class Security_Status
{
    err_unknown
    , err_user_exists
    , err_user_not_found
    , err_bad_auth
    , ok_register
    , ok_login
    , ok_password_reset
};

class User_Manager {
public:

    User_Manager() {}
    Security_Status register_user(
            std::string& username, const std::string& password);

    Security_Status login_user(
            std::string& username, const std::string& password);

    Security_Status delete_user(int userid);

    /* For a user that's already logged in, allows them to change
     * their password.
     */
    void change_password(const std::string& new_password); 

    /* For a user that cannot login, resets their password to
     * something else, and emails it to them.
     */
    Security_Status reset_password(int userid, std::string email,
            const std::string& appname);
    Security_Status reset_password(std::string& userid,
            const std::string& email, const std::string& appname);

    inline const int& get_user() const { return userid;}

private:
    int userid {-1};
    
    std::string sanitize_password(
            const std::string& password, const std::string& salt);

    inline std::string generate_salt()
    {
        TurnLeft::Utils::RandomCharSet rchar;
        return rchar.generate(1);
    }

    std::string get_user_salt(std::string userid, Redis::Mogu_Query_Handler* db =nullptr);
    std::string get_user_salt(const int& userid, Redis::Mogu_Query_Handler* db=nullptr);
    std::pair <std::string,std::string>
        get_sanitized_info(std::string& uname, const std::string& password);
    std::string get_salt(const std::string& username);
    int get_user_id(const std::string& username);

    /* Either returns an abandoned userid, or creates a new one. */
    int consume_user_id(Redis::Mogu_Query_Handler&);
};

#endif
