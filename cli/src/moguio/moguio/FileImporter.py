#!/usr/bin/env python

from Lex import RootConsumer
import SharedData
import sys

def filter_comment(string):
    return string.strip()[0] != "#"

def parse_root(filename):
    with open(filename,"r") as f:
        text = f.readlines()
        # Strip comments out of the file.
        text = filter(filter_comment,text)
        text = "\n".join(text)
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
