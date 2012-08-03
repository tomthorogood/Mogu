def m_import (args, db):
    if not args.filename:
        message = "You have told me that you want to import a file, but have not given me\
                a file to import."
        raise MissingArgumentError("--filename", message)

    if args.flushdb:
        message = warn(
                "WARNING: This will overwite ALL data in database %d \
                        of the Redis instance at %s:%d" % (
                    args.redis_db, args.redis_host, args.redis_port))
        if not confirm(message, args.yes):
            print( ok("No changes were made."))
            sys.exit()
    
    package = importdb.evaluate_file(args.filename)
    
    add_widget_set (db, package, args.testing, args.flushdb, args.merge)
    add_global_events (db, package.global_events, arge.testing, args.merge)
    add_meta_values (db, package.meta, args.testing, args.merge)
    add_perspectives(db, package.perspectives, args.testing, args.merge)
    add_policies(db, package.policies, args.testing)
