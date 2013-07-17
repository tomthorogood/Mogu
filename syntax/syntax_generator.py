#!/usr/bin/env python

#TODO ADD OPERATORS TO OUTPUT! @272 imap/smap

# This file parses the syntax.pre file, and is a replacement
# for "syntax.cpp", since the language is mostly irrelevant 
# for this piece of building Mogu. 
# 
# This reads 'syntax.pre', and produces 'syntax.h' and 'syntax.py', 
# which are used by Mogu's runtime and static interpreters (respectively).

###############
# BASIC SETUP #
###############

from pyboro import Lexer
from argparse import ArgumentParser
from collections import OrderedDict

import os
import sys

# CONSTANTS #

INPUT_FILE  = "syntax.pre"
OUTPUT_PY   = "syntax.py"
OUTPUT_H    = "syntax.h"

# Start with double digits to avoid clashes.
global_aggregator = 10

# Command Line Interface #
parser = ArgumentParser()
parser.add_argument("-m", type=int, action="store", dest="m")
parser.add_argument("-v", action="store_true", default=False, dest="v")
args = parser.parse_args()

# Turn on Verbose output for the lexer if requested
Lexer.VERBAL = args.v

with open("cpp_template.cpp","r") as f:
    CPP_HEADER = f.read()

PY_ADDITIONS = """
def as_integer(string):
    global MoguSyntax
    return MoguSyntax[string.strip()][0]

def as_string(integer):
    global MoguSyntax
    integer = integer.strip()
    if int(integer) == 0:
        return "0"
    reverseDict = dict.fromkeys(MoguSyntax,None)
    for key in MoguSyntax:
        ival = MoguSyntax[key][0]
        reverseDict[ival] = key
    return reverseDict[int(integer)]
"""

OPERATORS=OrderedDict([
        # Will be in the c++ form: SyntaxDef KEY (VAL[0],"KEY");
        # Syntactical operators (as opposed to placeholders)
        # will be represented in Python with:
        #   MoguOperators[KEY] = (val[0],[val1])
        ("OUT_OF_RANGE_BEGIN"    , (args.m-8,"")),
        ("OUT_OF_RANGE_END"      , (args.m-7,"")),
        ("TOKEN_DELIM"           , (args.m-6,"")),
        ("OPER_OPPAREN"          , (args.m-5,"(")),
        ("OPER_CLPARN"           , (args.m-4,")")),
        ("OPER_PLUS"             , (args.m-3,"+")),
        ("OPER_MINUS"            , (args.m-2,"-")),
        ("OPER_MULT"             , (args.m-1,"*")),
        ("OPER_DIV"              , (args.m,"/")),
])

OPER_DICT_OUT = """
MoguOperators = {} 
"""

for operator in OPERATORS:
    op_int = OPERATORS[operator][0]
    op_str = OPERATORS[operator][1]
    if op_str:
        OPER_DICT_OUT += \
            "\nMoguOperators['%s'] = %d" % (op_str,op_int)

class FormatError(Exception):
    def __init__(self, fstring):
        self.value = "FString error at %s." % (fstring)
        self.value += "FormatString must be in dict-parseable format. '%(foo)s' not '%s'."

    def __str__(self):
        return self.value



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
        ("begin",   "\(\s*",    Lexer.ParseMap.IGNORE),
        ("human",   "[^,]+",    Lexer.ParseMap.LITERAL),
        ("delim",   "\s*,\s*",  Lexer.ParseMap.IGNORE),
        ("enum",    "[^,]+",    Lexer.ParseMap.LITERAL),
        ("delim",   "\s*,\s*",  Lexer.ParseMap.IGNORE),
        ("notes",   "[^\)]+",   Lexer.ParseMap.LITERAL),
        ("end",     "\)\s*",    Lexer.ParseMap.IGNORE)
    ))
    def __init__(self, row_text):
        parsed_input = SyntaxTableRow.parser.parse(row_text)
        self.human_readable = HumanReadable(parsed_input["human"])
        self.enumerated = EnumeratedValue(parsed_input["enum"])
        self.notes = TokenNotes(parsed_input["notes"])

class SyntaxDefDecl(object):
    def __init__(self, identifier, integer):
        self.identifier = identifier
        self.integer = str(integer)

    def __str__(self):
        return '\tconstexpr const static SyntaxDef %(identifier)s (%(integer)s, "%(integer)s","%(identifier)s");' % self.__dict__

class InitializerLine(object):
    def __init__(self, key, value):
        self.key = key
        self.val = value

    def __str__(self):
        return "\t\t{%(key)s, %(val)s}" % self.__dict__

class InitializerList(object):
    def __init__(self,dct):
        self.d = dct

    def __str__(self):
        output_list = []
        for entry in self.d:
            output_list.append(InitializerLine(entry, self.d[entry]))
        return "{\n%s\n}\n" % ",\n".join([str(e) for e in output_list])

class MapDef(object):
    def __init__(self, name, t1, t2, initdict):
        self.name = name
        self.t1 = t1
        self.t2 = t2
        self.idict = initdict

    def __str__(self):
        initlist = InitializerList(self.idict)
        mDecl = "const static LockedMap <%(t1)s,%(t2)s> %(name)s = " \
                % self.__dict__
        body = str(initlist)

        return "%s%s;\n" % (mDecl,body)



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

def generate_py(dict_name, entries):
    """
    Given a list of 'SyntaxDictEntry' objects,
    returns a complete string of Python syntax.
    """
    global OPER_DICT_OUT

    start = PythonDictOpen(dict_name)
    end = PythonDictClose(dict_name)
    output = str(start)
    for entry in entries:
        output += str(entry)
    
    output += str(end)
    output += "\n\n%s" % OPER_DICT_OUT
    return output

def generate_cpp(enum_name, entries):
    """
    Given a list of 
    (identifier,integer) tuples, 
    generates appropriate C++.
    """
    global CPP_HEADER
    global OPERATORS
    output = {
            "syntax_definitions"    : None,
            "syntax_imap"           : None,
            "syntax_hmap"           : None,
            "syntax_smap"           : None
            }
    syntax_definition_list = []
    for entry in entries:
        definition = SyntaxDefDecl(entry[0],entry[1])
        syntax_definition_list.append(str(definition))
    for entry in OPERATORS:
        definition = SyntaxDefDecl(entry,OPERATORS[entry][0])
        syntax_definition_list.append(str(definition))
    output["syntax_definitions"] = "\n".join(syntax_definition_list)

    imap_tpl = OrderedDict()
    hmap_tpl = OrderedDict()
    smap_tpl = OrderedDict()
    for entry in entries:
        imap_tpl[str(entry[1])] = "MoguSyntax::%s" % entry[0]
        hmap_tpl["\"%s\""% entry[0]] = "MoguSyntax::%s" % entry[0]
        smap_tpl["\"%s\"" %str(entry[1])] = "MoguSyntax::%s" % entry[0]

    for oper in OPERATORS:
        enum = oper
        intg = OPERATORS[oper][0]
        common = OPERATORS[oper][1]

        imap_tpl["%s" % str(intg)] = "MoguSyntax::%s" % enum
        smap_tpl['"%s"' % str(intg)] = "MoguSyntax::%s" % enum
        if common:
            hmap_tpl['"%s"' % common] = "MoguSyntax::%s" % enum 


    imap = MapDef("imap","const int", "const SyntaxDef&",imap_tpl)
    hmap = MapDef("hmap","const std::string", "const SyntaxDef&", hmap_tpl)
    smap = MapDef("smap", "const std::string", "const SyntaxDef&", smap_tpl);

    output["syntax_imap"] = str(imap)
    output["syntax_hmap"] = str(hmap)
    output["syntax_smap"] = str(smap)

    return CPP_HEADER % output

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
        cpp_entries.append((str(parsed.enumerated), global_aggregator))
        global_aggregator += 1
        if global_aggregator >= 100:
            sys.stderr.write(
                "WARNING: Too much syntax! Redefine the basline for 100.")
    except Lexer.InputMatchError as e:
        continue

py_out = generate_py("MoguSyntax",py_entries) 
cpp_out = generate_cpp("MoguSyntax",cpp_entries)



with open(OUTPUT_PY,"w") as f:
    f.write(py_out)

with open(OUTPUT_H,"w") as f:
    f.write(cpp_out)
