#!/usr/bin/env python

# This file parses the syntax.pre file, and is a replacement
# for "syntax.cpp", since the language is mostly irrelevant 
# for this piece of building Mogu. 
# 
# This reads 'syntax.pre', and produces 'syntax.h' and 'syntax.py', 
# which are used by Mogu's runtime and static interpreters (respectively).

###############
# BASIC SETUP #
###############

import os

INPUT_FILE  = "syntax.pre"
OUTPUT_PY   = "syntax.py"
OUTPUT_H    = "syntax.h"

global_aggregator = 0

from pyboro import Lexer
import sys

# borrow some basic regular expressions
from regexlib import regexlib

PY_ADDITIONS = """
def as_integer(string):
    global MoguSyntax
    return MoguSyntax[string.strip()][0]
"""

def spaces(s1,s2):
    """
    For pretty formatting of output files. Ensures proper
    alignment of components.
    """
    maxlen = 40
    return " " * (maxlen-len(s1)-len(s2))
    

class ParsedInput(object):
    def __init__(self, input_token):
        self.input_token = input_token.strip()

    def __str__(self):
        return self.input_token

    def __len__(self):
        return len(self.input_token)

class HumanReadable(ParsedInput):
    """
    Describes the human readable portion of Mogu syntax.
    These tokens are what will be written by people and
    converted into enumerated values.
    """
    def __init__(self, input_token):
        """
        In the line:
            (   foo|bar ,   foo_bar ,   a foo that bars )
                ^^^^^^^^
                will be the input token.
        """
        super(HumanReadable,self).__init__(input_token)

        # Human readable tokens may optionally have multiple
        # HR segments. These will be delineated by the pipe symbol.
        self.tokens = self.input_token.split("|")

class EnumeratedValue(ParsedInput):
    def __init__(self, input_token):
        super(EnumeratedValue, self).__init__(input_token)
    def __int__(self):
        return int(self.input_token)

class TokenNotes(ParsedInput):
    def __init__(self, input_token):
        super(TokenNotes, self).__init__(input_token)

class SyntaxTableRow(object):
    parser = Lexer.ParseMap((
        ("begin",   "\(\s*" % regexlib,                 Lexer.ParseMap.IGNORE),
        ("human",   "[^,]+",                                        Lexer.ParseMap.LITERAL),
        ("delim",   "\s*,\s*" % regexlib,   Lexer.ParseMap.IGNORE),
        ("enum",    "[^,]+",                                        Lexer.ParseMap.LITERAL),
        ("delim",   "\s*,\s*" % regexlib,   Lexer.ParseMap.IGNORE),
        ("notes",   "[^\)]+",                                       Lexer.ParseMap.LITERAL),
        ("end",     "\)\s*"  % regexlib,                Lexer.ParseMap.IGNORE)
    ))
    def __init__(self, row_text):
        parsed_input = SyntaxTableRow.parser.parse(row_text)
        self.human_readable = HumanReadable(parsed_input["human"])
        self.enumerated = EnumeratedValue(parsed_input["enum"])
        self.notes = TokenNotes(parsed_input["notes"])

class FormatError(Exception):
    def __init__(self, fstring):
        self.value = "FString error at %s." % (fstring)
        self.value += "FormatString must be in dict-parseable format. '%(foo)s' not '%s'."

    def __str__(self):
        return self.value

class OutputLine(object):
    def __init__(self, fstring, args = {}):
        self.fstring = fstring if fstring.endswith("\n") else fstring + "\n"
        if "%(" not in self.fstring:
            raise FormatError(self.fstring)
        self.args = args

    def __str__(self):
        return self.fstring % self.args

class PythonDictOpen(OutputLine):
    def __init__(self, name):
        super(PythonDictOpen, self).__init__("%(name)s = {",{'name':name})

class PythonDictClose(OutputLine):
    def __init__(self, name):
        super(PythonDictClose,self).__init__("} # END %(name)s", {'name':name})

class SyntaxDictEntry(OutputLine):
    def __init__(self, human, integral, notes):
        fstring = "\t\"%(human)s\"%(align)s: (%(integral)d,\"%(notes)s\"),\n"
        align = spaces(human, (str(integral)+str(notes)))
        args = {
            "human"     :   human,
            "align"     :   align,
            "integral"  :   int(integral),
            "notes"     :   notes
        }
        super(SyntaxDictEntry,self).__init__(fstring,args)

class CppEnumOpen(OutputLine):
    def __init__(self, name):
        fstring = "enum %(name)s {"
        super(CppEnumOpen, self).__init__(fstring,{'name':name})

class CppEnumEntry(OutputLine):
    def __init__(self, enum, integer):
        integer = int(integer)
        align = spaces(enum,str(integer))
        fstring = "%(enum)s%(align)s= %(integer)d"
        if integer != 0:
            # since it's easier to edit cpp entries with a preceeding comma,
            # rather than a suffix comma, we'll do it like this.
            fstring = ","+fstring
        fstring = "\t"+fstring
        args = {
            "enum"      :   enum,
            "align"     :   align,
            "integer"   :   integer
        }
        super(CppEnumEntry, self).__init__(fstring, args)

class CppEnumClose(OutputLine):
    def __init__(self, name):
        fstring = "}; // END %(name)s"
        super(CppEnumClose,self).__init__(fstring,{'name':name})


def generate_py(dict_name, entries):
    """
    Given a list of 'SyntaxDictEntry' objects,
    returns a complete string of Python syntax.
    """
    start = PythonDictOpen(dict_name)
    end = PythonDictClose(dict_name)
    output = str(start)
    for entry in entries:
        output += str(entry)
    output += str(end)
    return output

def generate_cpp(enum_name, entries):
    """
    Given a list of 'CppEnumEntry' objects, returns a complete
    string of C++ syntax.
    """
    start = CppEnumOpen(enum_name)
#    end = CppEnumClose(enum_name)
    output = str(start)
    for entry in entries:
        output += str(entry)
    #output += str(end)
    return output

lines = None
with open(INPUT_FILE,"r") as f:
    lines = f.readlines()

py_entries = []
cpp_entries = []

for line in lines:
    if "#" in line: 
        continue
    try: 
        parsed = SyntaxTableRow(line)
        for human in parsed.human_readable.tokens:
            py_entries.append(
                    SyntaxDictEntry(human, global_aggregator, parsed.notes))
        cpp_entries.append(
                CppEnumEntry(parsed.enumerated,global_aggregator))
        global_aggregator += 1
    except Lexer.InputMatchError as e:
        sys.stderr.write("Could not parse line: %s" % line)
        continue

py_out = generate_py("MoguSyntax",py_entries) + PY_ADDITIONS
cpp_out = generate_cpp("MoguSyntax",cpp_entries)



with open(OUTPUT_PY,"w") as f:
    f.write(py_out)

with open(OUTPUT_H,"w") as f:
    f.write(cpp_out)

os.system("./operators")

with open(OUTPUT_H,"a") as f:
    cl = CppEnumClose("MoguSyntax");
    f.write("\n%s" % str(cl))

os.system('sed -i "s/template/template_/g" syntax.h')
os.system('sed -i "s/remove/remove_/g" syntax.h')
