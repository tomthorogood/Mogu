#!/usr/bin/env python
from coloring import *

def dbdiff(args,db):
    backupdb = Redis(args.redis_host, args.redis_port, args.command[1], args.redis_auth)
    orig_keys =db.keys("*")
    bak_keys = backupdb.keys("*")
    for key in orig_keys:
        if key not in bak_keys:
            print fail(key) + " was not found in the backup database!"
    message = ("# Orig. Keys: %d | # Bak. Keys: %d") % (len(orig_keys),len(bak_keys))
    if len(orig_keys) is not len(bak_keys):
        message = fail(message)
    else:
        message = ok(message)
    print message
