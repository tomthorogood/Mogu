"""
Deletes a user and all user sessions from the database. 
"""
import subprocess
from cityhash import Hash
from session_list import previous_sessions
def delete_user(db, username):
    #First, we need to encrypt the username
    proc = subprocess.Popen(
            ["/etc/mogu/cli_src/clidec", "encrypt", username],
            shell = False, stdout=subprocess.PIPE)
    euser = proc.communicate()[0].replace("\n","")


    #Then, we need to retrieve the user's last sessionid.
    sessionid = db.hget("s.global.%s" % Hash.session_lookup, euser)

    #Then, we need to get the user's authtoken before it is deleted:
    authtoken = db.hget("s.%s.%s" % (sessionid,Hash.meta),Hash.auth)
    assert(len(authtoken) > 0)

    #Then, we need to populate a list of all the user's sessions
    all_sessions = previous_sessions(db, sessionid)

    #Then, we delete everything pertaining to each of those sessions
    for session in all_sessions:
        keyspace = db.keys("s.%s.*" % session)
        for key in keyspace:
            db.delete(key)
    
    #Delete mention of the user from the session lookup table
    db.hdel("s.global.%s" % Hash.session_lookup, euser)

    #Find the authstring matching the user's auth token.
    auth_table = db.hgetall("s.global.%s" % Hash.auth_lookup)
    for entry in auth_table:
        if auth_table[entry] == authtoken:
            db.hdel("s.global.%s" % Hash.auth_lookup, entry)
            break

    db.hdel("s.global.%s" % Hash.salt_lookup, euser)

if __name__ == "__main__":
    from redis import Redis
    db = Redis()
    testuser = "FOO@BAR.COM"
    delete_user(db,testuser)
    
