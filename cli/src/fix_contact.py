from cityhash import Hash
from redis import Redis
db = Redis()

contact = "10325487529627100715"
contact_email = "2457360013983556327"

usernames = db.hkeys("s.global.%s" % Hash.session_lookup)

for user in usernames:
    session = db.hget("s.global.%s" % Hash.session_lookup, user)
    db.hset("s.%s.%s" % (session, contact), contact_email, user)
