#include <Mogu.h>
#include "UserManager.h"
#include "Encryption.h"
#include <Types/EmailManager.h>
#include <Redis/MoguQueryHandler.h>
#include <Redis/NodeEditor.h>
#include <hash.h>
SecurityStatus UserManager::registerUser(
        const std::string& username, const std::string& password)
{

    Redis::MoguQueryHandler udb(Application::contextMap, Prefix::user);

    Application::log.log(LogLevel::NOTICE, "Registering user ", username);

    if (username.empty())
    {
        Application::log.log(LogLevel::CRITICAL, "Registering empty string "
            , "as user. Something is wrong.");
    }


    std::string e_username = Security::encrypt(username);
    std::string e_password = Security::encrypt(password);
    std::string salt = generateSalt();

    /* First, check to see whether the user already exists */
    udb.appendQuery("hexists user.meta.id %s", e_username.c_str());
    if (udb.yieldResponse <bool>()) return SecurityStatus::ERR_USER_EXISTS;

    /* Next, sanitize the user's password */
    e_password = sanitizePassword(e_password, salt);

    userid = consumeUserId(udb);

    udb.appendQuery("hset user.meta.salt %s %s", 
            e_username.c_str(), salt.c_str());
    udb.appendQuery("hset user.%d.__meta__ u %s", userid, e_username.c_str());
    udb.appendQuery("hset user.%d.__meta__ p %s", userid, e_password.c_str());
    udb.appendQuery("hset user.meta.id %s %d", e_username.c_str(), userid);
    udb.flush();
    return SecurityStatus::OK_REGISTER;
}

SecurityStatus UserManager::loginUser(
        const std::string& username, const std::string& password)
{
    Redis::MoguQueryHandler udb(Application::contextMap, Prefix::user);
    std::string e_username = Security::encrypt(username);
    std::string e_password = Security::encrypt(password);
    
   /* Ensure that the user actually exists. */
   udb.appendQuery("hexists user.meta.id %s", e_username.c_str());
   udb.appendQuery("hget user.meta.id %s", e_username.c_str());
   if (!udb.yieldResponse <bool>())
   {
       udb.flush();
       return SecurityStatus::ERR_USER_NOT_FOUND;
   }
   userid = udb.yieldResponse <int>();
   std::string salt = getUserSalt(e_username, &udb);
   e_password = sanitizePassword(e_password, salt);

   udb.appendQuery("hget user.%d.__meta__ u", userid);
   udb.appendQuery("hget user.%d.__meta__ p", userid);

   bool success = 
       (e_username == udb.yieldResponse <std::string>()) 
       && (e_password == udb.yieldResponse <std::string>());

   if (! success) return SecurityStatus::ERR_BAD_AUTH;

   return SecurityStatus::OK_LOGIN;
}

SecurityStatus UserManager::resetPassword(int username, std::string email_address)
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

    mApp;
    EmailManager email;
    std::string message = 
        "We have received your request to reset your password for the " +
        app->getName() + " application. Your new password is " + newpass + 
        "\n\nYou may change this password after logging into your account.";
    email.setMessage(message);
    email.setSubject(app->getName() + " Password Reset Notification");
    email.setRecipient(email_address);
    email.send();

    return SecurityStatus::OK_REGISTER;
}

SecurityStatus UserManager::resetPassword(std::string username, const std::string& email)
{
    Redis::MoguQueryHandler db(Application::contextMap, Prefix::user);
    username = Security::encrypt(username);
    
    db.appendQuery("hget user.meta.id %s", username.c_str());
    return resetPassword(db.yieldResponse <int>(), email);
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
    return EMPTY;
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
        spass = Hash::toHash(Security::encrypt(spass));
    }
    return spass;
}
