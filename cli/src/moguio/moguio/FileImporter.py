#!/usr/bin/env python
from moguio.MoguString import MoguString
from Lex import RootConsumer
import SharedData
import sys
import re
current_file_input = None

def filter_comment(string):
    stripped = string.strip()
    if len(stripped)>0:
        return stripped[0] != "#"
    return True

def nonempty(string):
    return len(string.strip()) > 0 

def stage(filename, verbal=False):
    global current_file_input
    with open(filename,"r") as f:
        text = f.readlines()
        text = [line.strip() for line in text]  # Remove all leading/trailing whitespace from each line
        text = filter(nonempty,text)            # Only preserve nonempty lines
        text = filter(filter_comment,text)      # Only preserve non-comments
        text = [MoguString("script",line) for line in text] # Convert all lines to MoguStrings
        text = [line.translate("integral") for line in text]# Convert all text to integral representation

        if verbal:
            for index,line in enumerate(text): print("%d:\t%s"%(index,line))

        text = "\n".join(text)  # Create full text 
        current_file_input = text # Preserver in global namespace for debugging purposes
    return text

def parse_root(filename):
        text = stage(filename)
        try:
            return RootConsumer.parse(text)
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
