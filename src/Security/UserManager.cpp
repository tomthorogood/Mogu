#include "UserManager.h"
#include "Encryption.h"
#include "../Types/MoguLogger.h"
#include "../Types/EmailManager.h"
#include "../Redis/MoguQueryHandler.h"
#include "../Redis/NodeEditor.h"
#include "../hash.h"

namespace {
    void strtolower(std::string& str)
    {
        int sz = str.size();
        for (size_t i = 0; i < sz; ++i)
        {
            if (isalpha(str[i]) && isupper(str[i]))
                str[i] = tolower(str[i]);
        }
    }
}

std::string UserManager::getSalt(const std::string& uname)
{
    Redis::MoguQueryHandler db(Application::contextMap, Prefix::user);
    db.executeQuery("hget user.meta.salt %s", uname.c_str());
    std::string salt = db.yieldResponse<std::string>();
    if (salt.empty())
    {
        salt = generateSalt();
        db.executeQuery("hset user.meta.salt %s %s",
                uname.c_str(), salt.c_str());
    }
    return salt;
}

std::pair <std::string,std::string> UserManager::getSanitizedInfo(
    std::string& username, const std::string& userauth)
{
    strtolower(username);
    std::string e_username = Security::encrypt(username);
    std::string e_password = Security::encrypt(userauth);
    std::string h_username = Hash::toHash(e_username);
    std::string h_password = Hash::toHash(e_password);

    std::string salt = getSalt(h_username);
    std::string s_password = sanitizePassword(h_password,salt);
    auto pr = std::make_pair(h_username,s_password);
    return pr;
}

int UserManager::getUserId(const std::string& username)
{
    Redis::MoguQueryHandler db(Application::contextMap,Prefix::user);
    db.executeQuery("hget user.meta.id %s", username.c_str());
    std::string sid = db.yieldResponse<std::string>();
    if (sid.empty()) return -1;
    return atoi(sid.c_str());
}


SecurityStatus UserManager::registerUser(
        std::string& username, const std::string& password)
{
    std::pair <std::string,std::string> info = 
        getSanitizedInfo(username,password);

    Redis::MoguQueryHandler udb(Application::contextMap, Prefix::user);

    Application::log.log(LogLevel::NOTICE, "Registering user ", username);

    if (username.empty())
    {
        Application::log.log(LogLevel::CRITICAL, "Registering empty string "
            , "as user. Something is wrong.");
    }

    /* First, check to see whether the user already exists */
    if (getUserId(info.first) != -1)
    {

        return SecurityStatus::ERR_USER_EXISTS;
    }

    userid = consumeUserId(udb);
    
    udb.appendQuery("hset user.%d.__meta__ u %s", userid, info.first.c_str());
    udb.appendQuery("hset user.%d.__meta__ p %s", userid, info.second.c_str());
    udb.appendQuery("hset user.meta.id %s %d", info.first.c_str(), userid);
    udb.flush();
    return SecurityStatus::OK_REGISTER;
}

SecurityStatus UserManager::loginUser(
        std::string& username, const std::string& password)
{
    Redis::MoguQueryHandler udb(Application::contextMap, Prefix::user);
    std::pair <std::string,std::string> info = 
        getSanitizedInfo(username,password);

    userid = getUserId(info.first);
    if (userid==-1) return SecurityStatus::ERR_USER_NOT_FOUND;
   
    udb.appendQuery("hget user.%d.__meta__ u", userid);
    udb.appendQuery("hget user.%d.__meta__ p", userid);

    std::string retr_u = udb.yieldResponse<std::string>();
    std::string retr_p = udb.yieldResponse<std::string>();

    bool success = (info.first == retr_u) && (info.second==retr_p);
    if (! success) return SecurityStatus::ERR_BAD_AUTH;

    return SecurityStatus::OK_LOGIN;
}

SecurityStatus UserManager::resetPassword(int username, std::string email_address,
        const std::string& appname)
{
    email_address = Security::decrypt(email_address);

    Redis::MoguQueryHandler db(Application::contextMap, Prefix::user);
    TurnLeft::Utils::RandomCharSet rchar;
    std::string newpass = rchar.generate(4);
    std::string e_newpass = 
        sanitizePassword(Security::encrypt(newpass), getUserSalt(username, &db));
    db.appendQuery("exists user.%d.__meta__", username);
    db.appendQuery("hset user.%d.__meta__ p %s", username, e_newpass.c_str());
    if (!db.yieldResponse <bool>()) return SecurityStatus::ERR_USER_NOT_FOUND;
    db.flush();

    EmailManager email;
    std::string message = 
        "We have received your request to reset your password for the " +
        appname + " application. Your new password is " + newpass + 
        "\n\nYou may change this password after logging into your account.";
    email.setMessage(message);
    email.setSubject(appname + " Password Reset Notification");
    email.setRecipient(email_address);
    email.send();

    return SecurityStatus::OK_REGISTER;
}

SecurityStatus UserManager::resetPassword(std::string& username,
        const std::string& email, const std::string& appname)
{
    Redis::MoguQueryHandler db(Application::contextMap, Prefix::user);
    strtolower(username);
    username = Security::encrypt(username);
    
    db.appendQuery("hget user.meta.id %s", username.c_str());
    return resetPassword(db.yieldResponse <int>(), email, appname);
}

std::string UserManager::getUserSalt(std::string username, Redis::MoguQueryHandler* db)
{
    username = Security::encrypt(username);

    bool del_db_context = false;
    if (db == nullptr) 
    {
        db = new Redis::MoguQueryHandler(Application::contextMap, Prefix::user);
        del_db_context = true;
    }

    db->appendQuery("hget user.meta.salt %s", username.c_str()); 
    std::string salt = db->yieldResponse <std::string>();
    if (del_db_context) delete db;
    return salt;
}

std::string UserManager::getUserSalt(const int& userid, Redis::MoguQueryHandler* db)
{
    Redis::NodeEditor node(Prefix::user, "meta.id");
    std::map <std::string,std::string> rlookup;
    node.read(rlookup);

    auto iter = rlookup.begin();
    while (iter != rlookup.end())
    {
        if (userid == atoi(iter->second.c_str()))
            return getUserSalt(iter->first,db);
        ++iter;
    }
    return "";
}

int UserManager::consumeUserId(Redis::MoguQueryHandler& db)
{
    // See if we have any abandoned ids to consume.
    db.appendQuery("scard user.meta.abandoned");
    if (db.yieldResponse <int>() > 0)
    {
        db.appendQuery("spop user.meta.abandoned");
        Application::log.log(LogLevel::NOTICE, "Using abandond userid.");
        return db.yieldResponse <int>();
    }
    else
    {
        // Otherwise, increment and return the next user count.
        db.appendQuery("exists user.meta.count");
        if (!db.yieldResponse<bool>())
        {
            db.executeQuery("set user.meta.count 0");
            Application::log.log(LogLevel::NOTICE, "Not users found. Creating ",
                "new user count.");
        }
        db.appendQuery("incr user.meta.count");
        return db.yieldResponse <int>();
    }
}

SecurityStatus UserManager::deleteUser(int userid) 
{
    Redis::MoguQueryHandler db(Application::contextMap, Prefix::user);
    Redis::MoguQueryHandler grpdb(Application::contextMap, Prefix::group);
    db.appendQuery("exists user.%d", userid);
    db.appendQuery("llen user.%d.__meta__.g", userid);
    std::vector <std::string> groups;
    if (!db.yieldResponse <bool>()) return SecurityStatus::ERR_USER_NOT_FOUND;
    int num_groups = db.yieldResponse <int>();
    if (num_groups > 0)
    {
        db.appendQuery("lrange user.%d.__meta__.g 0 %d", userid, num_groups);
        groups = db.yieldResponse <std::vector<std::string>>();
    }
    
    db.appendQuery("keys user.%d.*", userid);
    db.appendQuery("sadd user.meta.abandoned %d", userid);

    std::vector <std::string> userkeys = db.yieldResponse <std::vector <std::string>>();
    
    for (auto key : userkeys)
    {
        const char* c_key = key.c_str();
        db.appendQuery("del user.%d.%s", userid, c_key);
    }
   
    for (auto group : groups)
    {
        const char* c_grp = group.c_str();
        grpdb.appendQuery("srem groups.%s.__meta__.m %d", c_grp, userid);

    }
    db.flush();
    grpdb.flush();
    return SecurityStatus::OK_REGISTER;
}

std::string UserManager::sanitizePassword(const std::string& password, const std::string& salt)
{
    std::string spass = password + salt;
    char c[1] = {salt[1]};
    int iters = atoi(c) * 1000;
    for (int i=0; i < iters; ++i)
    {
        spass = Hash::toHash(spass);
    }
    return spass;
}
