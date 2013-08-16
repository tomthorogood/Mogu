#include "UserManager.h"
#include "Encryption.h"
#include "../Types/MoguLogger.h"
#include "../Types/EmailManager.h"
#include "../Redis/MoguQueryHandler.h"
#include "../Redis/NodeEditor.h"
#include "../hash.h"

namespace Application {
    extern Mogu_Logger log;
}

namespace {
    void strtolower(std::string& str)
    {
        size_t sz {str.size()};
        for (size_t i = 0; i < sz; ++i)
        {
            if (isalpha(str[i]) && isupper(str[i]))
                str[i] = tolower(str[i]);
        }
    }
}

std::string User_Manager::get_salt(const std::string& uname)
{
    Redis::Mogu_Query_Handler db(Prefix::user);
    db.execute_query("hget user.meta.salt %s", uname.c_str());
    std::string salt = db.yield_response<std::string>();
    if (salt.empty())
    {
        salt = generate_salt();
        db.execute_query("hset user.meta.salt %s %s",
                uname.c_str(), salt.c_str());
    }
    return salt;
}

std::pair <std::string,std::string> User_Manager::get_sanitized_info(
    std::string& username, const std::string& userauth)
{
    strtolower(username);
    std::string e_username {Security::encrypt(username)};
    std::string e_password {Security::encrypt(userauth)};
    std::string h_username {Hash::to_hash(e_username)};
    std::string h_password {Hash::to_hash(e_password)};

    std::string salt {get_salt(h_username)};
    std::string s_password {sanitize_password(h_password,salt)};
    auto pr = std::make_pair(h_username,s_password);
    return pr;
}

int User_Manager::get_user_id(const std::string& username)
{
    Redis::Mogu_Query_Handler db(Prefix::user);
    db.execute_query("hget user.meta.id %s", username.c_str());
    std::string sid = db.yield_response<std::string>();
    if (sid.empty()) return -1;
    return atoi(sid.c_str());
}


Security_Status User_Manager::register_user(
        std::string& username, const std::string& password)
{
    std::pair <std::string,std::string> info = 
        get_sanitized_info(username,password);

    Redis::Mogu_Query_Handler udb(Prefix::user);

    Application::log.log(Log_Level::notice, "Registering user ", username);

    if (username.empty())
    {
        Application::log.log(Log_Level::critical, "Registering empty string "
            , "as user. Something is wrong.");
    }

    /* First, check to see whether the user already exists */
    if (get_user_id(info.first) != -1)
    {

        return Security_Status::err_user_exists;
    }

    userid = consume_user_id(udb);
    
    udb.append_query("hset user.%d.__meta__ u %s", userid, info.first.c_str());
    udb.append_query("hset user.%d.__meta__ p %s", userid, info.second.c_str());
    udb.append_query("hset user.meta.id %s %d", info.first.c_str(), userid);
    udb.flush();
    return Security_Status::ok_register;
}

Security_Status User_Manager::login_user(
        std::string& username, const std::string& password)
{
    Redis::Mogu_Query_Handler udb(Prefix::user);
    std::pair <std::string,std::string> info = 
        get_sanitized_info(username,password);

    userid = get_user_id(info.first);
    if (userid==-1) return Security_Status::err_user_not_found;
   
    udb.append_query("hget user.%d.__meta__ u", userid);
    udb.append_query("hget user.%d.__meta__ p", userid);

    std::string retr_u = udb.yield_response<std::string>();
    std::string retr_p = udb.yield_response<std::string>();

    bool success = (info.first == retr_u) && (info.second==retr_p);
    if (! success) return Security_Status::err_bad_auth;

    return Security_Status::ok_login;
}

Security_Status User_Manager::reset_password(int username, std::string email_address,
        const std::string& appname)
{
    email_address = Security::decrypt(email_address);

    Redis::Mogu_Query_Handler db(Prefix::user);
    TurnLeft::Utils::RandomCharSet rchar;
    std::string newpass = rchar.generate(4);
    std::string e_newpass = 
        sanitize_password(Security::encrypt(newpass), get_user_salt(username, &db));
    db.append_query("exists user.%d.__meta__", username);
    db.append_query("hset user.%d.__meta__ p %s", username, e_newpass.c_str());
    if (!db.yield_response <bool>()) return Security_Status::err_user_not_found;
    db.flush();

    Email_Manager email;
    std::string message = 
        "We have received your request to reset your password for the " +
        appname + " application. Your new password is " + newpass + 
        "\n\nYou may change this password after logging into your account.";
    email.set_message(message);
    email.set_subject(appname + " Password Reset Notification");
    email.set_recipient(email_address);
    email.send();

    return Security_Status::ok_register;
}

Security_Status User_Manager::reset_password(std::string& username,
        const std::string& email, const std::string& appname)
{
    Redis::Mogu_Query_Handler db(Prefix::user);
    strtolower(username);
    username = Security::encrypt(username);
    
    db.append_query("hget user.meta.id %s", username.c_str());
    return reset_password(db.yield_response <int>(), email, appname);
}

std::string User_Manager::get_user_salt(std::string username, Redis::Mogu_Query_Handler* db)
{
    username = Security::encrypt(username);

    bool del_db_context = false;
    if (db == nullptr) 
    {
        db = new Redis::Mogu_Query_Handler(Prefix::user);
        del_db_context = true;
    }

    db->append_query("hget user.meta.salt %s", username.c_str()); 
    std::string salt = db->yield_response <std::string>();
    if (del_db_context) delete db;
    return salt;
}

std::string User_Manager::get_user_salt(const int& userid, Redis::Mogu_Query_Handler* db)
{
    Redis::Node_Editor node(Prefix::user, "meta.id");
    std::map <std::string,std::string> rlookup;
    node.read(rlookup);

    auto iter = rlookup.begin();
    while (iter != rlookup.end())
    {
        if (userid == atoi(iter->second.c_str()))
            return get_user_salt(iter->first,db);
        ++iter;
    }
    return "";
}

int User_Manager::consume_user_id(Redis::Mogu_Query_Handler& db)
{
    // See if we have any abandoned ids to consume.
    db.append_query("scard user.meta.abandoned");
    if (db.yield_response <int>() > 0)
    {
        db.append_query("spop user.meta.abandoned");
        Application::log.log(Log_Level::notice, "Using abandond userid.");
        return db.yield_response <int>();
    }
    else
    {
        // Otherwise, increment and return the next user count.
        db.append_query("exists user.meta.count");
        if (!db.yield_response<bool>())
        {
            db.execute_query("set user.meta.count 0");
            Application::log.log(Log_Level::notice, "Not users found. Creating ",
                "new user count.");
        }
        db.append_query("incr user.meta.count");
        return db.yield_response <int>();
    }
}

Security_Status User_Manager::delete_user(int userid) 
{
    Redis::Mogu_Query_Handler db(Prefix::user);
    Redis::Mogu_Query_Handler grpdb(Prefix::group);
    db.append_query("exists user.%d", userid);
    db.append_query("llen user.%d.__meta__.g", userid);
    std::vector <std::string> groups;
    if (!db.yield_response <bool>()) return Security_Status::err_user_not_found;
    int num_groups = db.yield_response <int>();
    if (num_groups > 0)
    {
        db.append_query("lrange user.%d.__meta__.g 0 %d", userid, num_groups);
        groups = db.yield_response <std::vector<std::string>>();
    }
    
    db.append_query("keys user.%d.*", userid);
    db.append_query("sadd user.meta.abandoned %d", userid);

    std::vector <std::string> userkeys = db.yield_response <std::vector <std::string>>();
    
    for (auto key : userkeys)
    {
        const char* c_key = key.c_str();
        db.append_query("del user.%d.%s", userid, c_key);
    }
   
    for (auto group : groups)
    {
        const char* c_grp = group.c_str();
        grpdb.append_query("srem groups.%s.__meta__.m %d", c_grp, userid);

    }
    db.flush();
    grpdb.flush();
    return Security_Status::ok_register;
}

std::string User_Manager::sanitize_password(const std::string& password, const std::string& salt)
{
    std::string spass = password + salt;
    char c[1] = {salt[1]};
    int iters = atoi(c) * 1000;
    for (int i=0; i < iters; ++i)
    {
        spass = Hash::to_hash(spass);
    }
    return spass;
}
