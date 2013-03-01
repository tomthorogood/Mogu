#!/usr/bin/env python

import RootConsumer
import SharedData
import sys

def parse_root(filename):
    with open(filename,"r") as f:
        text = f.read()
        try:
            return RootConsumer.RootConsumer.parse(text)
        except Exception as e:
            sys.stderr.write(
                "\nCannot parse file: %s\n\nImport halted. No changes made.\n\n" % filename
            )
            raise e


def import_file(filename, verbal=False):
    SharedData.ActiveFile = filename
    if verbal:
        sys.stderr.write("Importing %s ... " % filename)
    root_results = parse_root(filename)
    if verbal:
        sys.stderr.write(" DONE!\n")
    return root_results
