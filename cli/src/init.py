#!/usr/bin/env python

def init(args,db):
    # First, check to make sure there isn't a Mogu application
    # already configured in the database
    if len(db.keys("*")) is not 0:
        if not confirm(
                warn("It appears that there is already information in the currently selected database. Initializing a project here may overwrite data already present. It is recommended that you select a different database (with --redis-select) instead."),
                    args.yes):
            print(ok("No changes were made."))
            sys.exit()
    db.hset("widgets.wrapper","type","{container}")
    print(ok("Mogu project has been initialized! Go Mogu!"))
