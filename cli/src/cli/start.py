#!/usr/bin/env python
from coloring import * 
from os import system
import sys

def start(args):
    cmd = 'mogu-server --docroot "%s;%s" --http-address %s --http-port %s &' %(
            args.docroot, args.docsubs, args.address, args.port)
    sys.stdout.write("%s\n"%cmd)
    system(cmd);

