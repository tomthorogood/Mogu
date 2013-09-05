import syntax
import SymbolRegistry
import SharedData
import ReferenceFinder
from lex_base import regexlib
import re

def directive_start(token):
    d =  r"\s*(%s)\s*:\s*" % token
    return d

def add_references(string):
    f = ReferenceFinder.ReferenceFinder(string)
    for obj in f.refs:
        if obj not in SharedData.symbols:
            SharedData.symbols[obj] = SymbolRegistry.SymbolRegistry(
                label=syntax.as_string(obj))
        for symbol in f.refs[obj]:
            SharedData.symbols[obj].reference(symbol, SharedData.ActiveFile)
    return string

def add_definition(obj, identifier):
    if obj not in SharedData.symbols:
        SharedData.symbols[obj] = SymbolRegistry.SymbolRegistry(
            label = syntax.as_string(obj))
    SharedData.symbols[obj][identifier] = SharedData.ActiveFile
    return identifier

def remove_string_literals(string):
    found = re.findall(regexlib["string_literal"])
    for literal in found:
        string = string.replace(literal,"")
    return string

def reference_widget_list(string):
    w_list = newline_list(string)
    for index,entry in enumerate(w_list):
        if entry.startswith(":"):
            w_list[index] = SharedData.ActiveIdentifier+entry
            if SharedData.VERBAL: print("Expanded %s to %s" % (entry, w_list[index]))
    for w in w_list:
        SharedData.symbols[syntax.as_integer("widget")].reference(w, SharedData.ActiveFile)
    return w_list

def valid_options (string):
    """
        Returns valid integral representations of keywords.
    """
    return [str(syntax.MoguSyntax[key][0]) \
            for key in syntax.MoguSyntax if string in syntax.MoguSyntax[key][1]]

def trim(string):
    return string.strip()

def everything_until(string):
    return r"(.|\n)+?(?=%s)" % string

def newline_list(string):
    nl_list = string.split("\n")
    nl_list = [e for e in [entry.strip() for entry in nl_list if entry] if e]
    nl_list = [i.strip() for i in nl_list]
    return nl_list
