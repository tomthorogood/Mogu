#!/usr/bin/env python
from coloring import * 
from os import system
import sys
import moguio.RedisWriter

def start(args):
    sys.stdout.write("Setting root widget to %s\n" % args.dbroot)
    w = moguio.RedisWriter.RedisWriter(args)
    w.setRoot(args.dbroot)
    cmd = 'mogu-server --docroot "%s;%s" --http-address %s --http-port %s &' %(
            args.docroot, args.docsubs, args.address, args.port)
    system(cmd);

