#!/usr/bin/env python
from mogupy_conf import META_KEYS
from coloring import * 
from os import system
def start(args,db):
    global META_KEYS
    config = args.conf
    for key in META_KEYS:
        if config.has_option("meta_keys", key):
            val = config.get("meta_keys", key)
            print(info("meta.%s -> %s" % (key,val)))
            db.set("meta.%s" % key, val)

    cli_command = "mogu-server --docroot \"%s;/resources\" --http-address %s --http-port %s" % (
            config.get("local_server","root"), config.get("local_server", "address"), config.get("local_server", "port"))
    if config.get("local_server","debug") == "on":
        cli_command += " --gdb"
    print(cli_command)
    system(cli_command)
