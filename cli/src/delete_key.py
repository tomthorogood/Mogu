#!/usr/bin/env python
from snippets import *
from coloring import *

def delete_key(args,db):
    keys = args.command[1:]

    if args.filename:
        f = open(args.filename,"r")
        keys = [key.replace("\n","") for key in f.readlines()]
        print(keys)
    for key in keys:
        ex = db.exists(key)
        print(ex)
        if (ex):
            warning = warn("%s will be permanently deleted. THERE IS NO UNDO!" % key)
            if confirm(warning, args.yes):
                db.delete(key)
            else:
                print(ok("No changes were made."))
        else:
            print(fail("%s does not exist in this database." % key))
            print("For your safety, we have disabled auto key-complete when deleting. You must use the full key name.")
