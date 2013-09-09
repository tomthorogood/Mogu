import syntax
import SymbolRegistry
import SharedData
import ReferenceFinder
import re

def directive_start(token):
    d =  r"\s*(%s)\s*:\s*" % token
    return d

def add_references(string):
    f = ReferenceFinder.ReferenceFinder(string)
    for obj in f.refs:
        if obj not in SharedData.symbols:
            SharedData.symbols[obj] = SymbolRegistry.SymbolRegistry(
                label= syntax.as_string(obj) if obj!="policy" else obj)
        for symbol in f.refs[obj]:
            SharedData.symbols[obj].reference(symbol, SharedData.ActiveFile)
    return string.strip()

def temp_join(func, delim, ret_index, *args):
    string = delim.join(args)
    func(string)
    return args[ret_index].strip()

def add_definition(obj, identifier):
    SharedData.ActiveIdentifier = identifier
    if obj not in SharedData.symbols:
        SharedData.symbols[obj] = SymbolRegistry.SymbolRegistry(
            label = syntax.as_string(obj))
    SharedData.symbols[obj][identifier] = SharedData.ActiveFile
    return identifier.strip()

def reference_widget_list(string):
    w_list = newline_list(string)
    for index,entry in enumerate(w_list):
        if entry.startswith(":"):
            w_list[index] = SharedData.ActiveIdentifier+entry
            if SharedData.VERBAL: print("Expanded %s to %s" % (entry, w_list[index]))
    for w in w_list:
        SharedData.symbols[syntax.as_integer("widget")].reference(w, SharedData.ActiveFile)
    return w_list

def trim(string):
    return string.strip()

def everything_until(string):
    return r"(.|\n)+?(?=%s)" % string

def newline_list(string):
    nl_list = string.split("\n")
    nl_list = [e for e in [entry.strip() for entry in nl_list if entry] if e]
    nl_list = [i.strip() for i in nl_list]
    return nl_list
