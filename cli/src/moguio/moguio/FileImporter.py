#!/usr/bin/env python
from moguio.MoguString import MoguString
from Lex import RootConsumer
import SharedData
import sys
import re
import os
import Preprocessor
current_file_input = None


def nonempty(string):
    return len(string.strip()) > 0 

def preprocess(text,verbal=False):
    global current_file_input
    current_file_input = text
    if not isinstance(text,list):
        text = text.split("\n")

    assert(isinstance(text,list))
    text = [ line.strip() for line in text ]# Remove all leading/trailing whitespace from each line
    text = filter(nonempty,text)            # Only preserve nonempty lines
    iterindex = 0
    step = "Text to MoguString:Script"
    try:
        # Convert all strings to MoguStrings
        for index,line in enumerate(text):
            iterindex = index
            ms = MoguString("script",line)
            text[index] = ms
    
        step = "MoguString:Script to MoguString:Integral"

        # Translate all strings to integral syntax
        for index,line in enumerate(text):
            assert(isinstance(line,MoguString))
            iterindex = index
            line.translate('integral')
            line.active('integral')
    except Exception as e:
        errMsg = """
    %s

Could not parse text: 
    %s

MoguString Attributes:
    String Literals: 
        %s

    Current Temp:
        %s

Step:
    %s
""" %(e, text[iterindex], text[iterindex].string_literals, text[iterindex].temp, step)
        raise ImportError(errMsg)

    assert(isinstance(text,list))
    text = [line.integral for line in text]
    text = "\n".join(text)
    current_file_input = text
    return text

def stage(filename=None, verbal=False):
    global current_file_input
    with open(filename,"r") as f:
        text = f.read()
        preproc = Preprocessor.Preprocessor(verbose=verbal)
        path=os.path.dirname(filename)
        text = preproc.processInput(text,path)
        text = preprocess(text)
        current_file_input = text # Preserver in global namespace for debugging purposes
    return text

def parse_root(filename,verbal=False):
        text = stage(filename,verbal)
        return RootConsumer.parse(text)

def import_raw(text, verbal=False):
    SharedData.ActiveFile = "Raw Input"
    text = preprocess(text,verbal)
    if verbal:
        sys.stderr.write("Importing from raw input")
    try:
        return RootConsumer.parse(text)
    except Exception as e:
        sys.stderr.write(
                "\nCannot parse text. Import halted. No changes made.\n\n" % filename
        )
        sys.stderr.flush()
        sys.exit(1)

def import_file(filename, verbal=False):
    SharedData.ActiveFile = filename
    if verbal:
        sys.stdout.write("Importing %s ... " % filename)
    root_results = parse_root(filename,verbal)
    if verbal:
        sys.stdout.write(" DONE!\n")
    return root_results
