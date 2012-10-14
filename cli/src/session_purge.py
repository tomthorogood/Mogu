#!/usr/bin/env python
from snippets import *
from cityhash import * 
def purge_sessions(args,db):
    gnamespace = "s.global.%s"

    if not confirm( 
            fail(
                "You are about to permanently remove ALL non-global sessions from your database.\
This cannot be undone! Be very, very careful here!"), args.yes):
        sys.exit()
    all_sessions = db.keys("s.*")
    to_be_deleted = []
    for session in all_sessions:
        if "global" not in session:
            to_be_deleted.append(session)
            continue
    to_be_deleted.extend([
        gnamespace % Hash.session_lookup,
        gnamespace % Hash.auth_lookup,
        gnamespace % Hash.salt_lookup])
    for key in to_be_deleted:
        db.delete(key)

