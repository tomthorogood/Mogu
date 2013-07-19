#include <Mogu.h>
#include "UserManager.h"
#include "Encryption.h"
#include <Types/EmailManager.h>
#include <Redis/ContextQuery.h>
#include <Redis/NodeEditor.h>
#include <hash.h>
SecurityStatus UserManager::registerUser(
        const std::string& username, const std::string& password)
{
    Redis::ContextQuery udb(Prefix::user);

    std::string e_username = Security::encrypt(username);
    std::string e_password = Security::encrypt(password);
    std::string salt = generateSalt();

    /* First, check to see whether the user already exists */
    udb.appendQuery("hexists user.meta.id %s", e_username.c_str());
    if (udb.yieldResponse <bool>()) return SecurityStatus::ERR_USER_EXISTS;

    /* Next, sanitize the user's password */
    e_password = sanitizePassword(e_password, salt);

    int userid = consumeUserId(udb);

    udb.appendQuery("hset user.meta.salt %s %s", 
            e_username.c_str(), salt.c_str()); 
    udb.appendQuery("hset user.%d.__meta__ u %s", userid, e_username.c_str());
    udb.appendQuery("hset user.%d.__meta__ p %s", userid, e_password.c_str());
    udb.execute();

    return SecurityStatus::OK_REGISTER;
}

SecurityStatus UserManager::loginUser(
        const std::string& username, const std::string& password)
{
    Redis::ContextQuery udb(Prefix::user);
    std::string e_username = Security::encrypt(username);
    std::string e_password = Security::encrypt(password);
    
   /* Ensure that the user actually exists. */
   udb.appendQuery("hexists user.meta.id %s", e_username.c_str());
   udb.appendQuery("hget user.meta.id %s", e_username.c_str());
   if (!udb.yieldResponse <bool>())
   {
       udb.execute();
       return SecurityStatus::ERR_USER_NOT_FOUND;
   }
   int userid = udb.yieldResponse <int>();
   std::string salt = getUserSalt(e_username, &udb);
   e_password = sanitizePassword(e_password, salt);

   udb.appendQuery("hget user.%d.__meta__ u", userid);
   udb.appendQuery("hget user.%d.__meta__ p", userid);

   bool success = 
       (e_username == udb.yieldResponse <std::string>()) 
       && (e_password == udb.yieldResponse <std::string>());

   if (! success) return SecurityStatus::ERR_BAD_AUTH;

   this->userid = userid;
   return SecurityStatus::OK_LOGIN;

}

SecurityStatus UserManager::resetPassword(int userid, std::string email_address)
{
    if (Security::isEncrypted(email_address))
        email_address = Security::decrypt(email_address);

    Redis::ContextQuery db(Prefix::user);
    TurnLeft::Utils::RandomCharSet rchar;
    std::string newpass = rchar.generate(4);
    std::string e_newpass = 
        sanitizePassword(Security::encrypt(newpass), getUserSalt(userid, &db));
    db.appendQuery("exists user.%d.__meta__", userid);
    db.appendQuery("hset user.%d.__meta__ p %s", userid, e_newpass.c_str());
    if (!db.yieldResponse <bool>()) return SecurityStatus::ERR_USER_NOT_FOUND;
    db.execute();

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

SecurityStatus UserManager::resetPassword(std::string userid, const std::string& email)
{
    Redis::ContextQuery db(Prefix::user);
    if (! Security::isEncrypted(userid))
        userid = Security::encrypt(userid);
    
    db.appendQuery("hget user.meta.id %s", userid.c_str());
    return resetPassword(db.yieldResponse <int>(), email);
}

std::string UserManager::getUserSalt(std::string userid, Redis::ContextQuery* db)
{
    if (!Security::isEncrypted(userid))
        userid = Security::encrypt(userid);

    bool del_db_context = false;
    if (db == nullptr) 
    {
        db = new Redis::ContextQuery(Prefix::user);
        del_db_context = true;
    }

    db->appendQuery("hget user.meta.salt %s", userid.c_str()); 
    std::string salt = db->yieldResponse <std::string>();
    if (del_db_context) delete db;
    return salt;
}

std::string UserManager::getUserSalt(const int& userid, Redis::ContextQuery* db)
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

int UserManager::consumeUserId(Redis::ContextQuery& db)
{
    // See if we have any abandoned ids to consume.
    db.appendQuery("scard user.meta.abandoned");
    if (db.yieldResponse <int>() > 0)
    {
        db.appendQuery("spop user.meta.abandoned");
        return db.yieldResponse <int>();
    }
    else
    {
        // Otherwise, increment and return the next user count.
        db.appendQuery("incr user.meta.count");
        return db.yieldResponse <int>();
    }
}

SecurityStatus UserManager::deleteUser(int userid) 
{
    Redis::ContextQuery db(Prefix::user);
    Redis::ContextQuery grpdb(Prefix::group);
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
    db.execute();
    grpdb.execute();
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
    return Security::encrypt(spass);
}
