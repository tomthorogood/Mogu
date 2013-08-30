import redis
udb = redis.Redis(db=1)
gdb = redis.Redis(db=2)

udb.flushdb()
gdb.flushdb()
