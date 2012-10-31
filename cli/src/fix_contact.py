from cityhash import *
from redis import Redis
db = Redis()

CONTACT_HASH = to_city_hash("contact")
NAME_HASH = to_city_hash("name")
EMAIL_HASH = to_city_hash("email")
C_EMAIL_HASH = to_city_hash("contact_email")

def get_sessions(endpoint):
    ret = []
    current = endpoint
    i = 0
    while current != "global" and i < db.dbsize():
        ret.append(current)
        current = db.hget("s.%s.%s" 
                % (current, Hash.meta), Hash.prev)
        i+=1
    return ret

usernames = db.hkeys("s.global.%s" % Hash.session_lookup)

def fix_contact_fields(session,recent):
    NAMEFIELD = to_city_hash("profileedit:name:input")
    
    if db.hexists("s.%s.%s" % (session, CONTACT_HASH), C_EMAIL_HASH):
        email = db.hget("s.%s.%s" % (session, CONTACT_HASH), C_EMAIL_HASH)
        db.hset("s.%s.%s" % (recent, CONTACT_HASH), EMAIL_HASH, email)
        if session != recent:
            db.hdel("s.%s.%s" % (session, CONTACT_HASH), C_EMAIL_HASH)
    
    if db.exists("s.%s.%s" % (session, NAMEFIELD)):
        print ("s.%s.%s" % (session, NAMEFIELD))
        name = db.get("s.%s.%s" % (session, NAMEFIELD))
        print(name)
        if (name.endswith("0_")):
            db.hset("s.%s.%s" % (recent, CONTACT_HASH), NAME_HASH, name)
            if session != recent:
                db.delete("s.%s.%s" % (session, NAMEFIELD))

for user in usernames:
    email = user
    recent = db.hget("s.global.%s" % Hash.session_lookup, user)
    sessions = get_sessions(recent)
    for session in sessions:
        fix_contact_fields(session, recent)
    

