#!/usr/bin/env python
from os import path

from snippets import confirm_overwrite
from snippets import assertending
def newfile(args, db):
    filenames = args.command[1:]
    for filename in filenames:
        if not confirm_overwrite(filename,args.yes):
            continue

        filename = assertending(filename, ".mogu")
        f = open(filename,"w")
        f.write("# Mogu Import File %s" % filename)
        f.write("\n\n")
        f.close()
    
