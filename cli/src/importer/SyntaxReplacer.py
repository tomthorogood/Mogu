# A set of submodules to replace human readable syntax
# with integral representations

import syntax # Syntax/Integer Map
from sets import Set
from pyRex import Consumer
from pyRex import Lexer
from regexlib import regexlib # Basic regular expressions
from lex_functions import reference_widget
from lex_functions import reference_data
from lex_functions import reference_template
from lex_functions import reference_validator
from lex_functions import reference_policy
import SharedData
import SymbolRegistry
import re

def is_integer(string):
    try:
        int(string)
        return True
    except ValueError:
        return False

def referenced_types():
    referenced_types = [
            syntax.as_integer("widget"),
            syntax.as_integer("data"),
            syntax.as_integer("user"),
            syntax.as_integer("group")
            ]
    return referenced_types

def reference(integral_repr, symbol):
    integral_repr = int(integral_repr)
    referenceTables = (
            SymbolRegistry.widgetRegistry,
            SymbolRegistry.dataRegistry,
            SymbolRegistry.policyRegistry,
            SymbolRegistry.policyRegistry
            )
    types = referenced_types()
    for index,repr_type in enumerate(types):
        if integral_repr == repr_type:
            referenceTables[index][symbol].append(SharedData.ActiveFile)

def create_references(string):
    stringlits = re.findall(regexlib["string_literal"], string)
    
    # We don't need to test string literals. Therefore, strip them out. 
    for lit in stringlits:
        while lit in string:
            string = string.replace(lit," ")
    # get only valid tokens
    tokens = [entry for entry in string.split(" ") if entry]
    for index,token in enumerate(tokens):
        if not is_integer(token) and index != 0:
            if (int(tokens[index-1]) in referenced_types()):
                reference(tokens[index-1], token)

def find_syntax_tokens(string):
    """
    Creates a list of valid syntax tokens from 'syntax.py', and 
    searches the input string for each of the tokens, returning
    the tokens found as a list. Sorts the list of tokens by
    length first to be sure that the largest inputs are found
    first.
    """
    ltokens = [key+"\s+" for key in syntax.MoguSyntax]
    ltokens = sorted(ltokens,key=len)
    ltokens.reverse()
    tokens = "(%s)" % ("|".join(ltokens))
    found = re.findall(tokens,string)
    return re.findall(tokens,string)

def filter_string_literals (string):
    """
    Given an input string, determines which portions of the string
    are string literals, and which are not. Returns a list of 
    the strings that do not contain string literals.
    """

    literals = re.findall(regexlib["string_literal"],string)
    for lit in literals:
        string = string.replace(lit,"%s")
    return string,literals

def replace_syntax(string):
    working, literals = filter_string_literals(string)
    tokens = find_syntax_tokens(working)
    for token in tokens:
        working = working.replace(token,str(syntax.as_integer(token))+" ")
    working = working % tuple(literals)
    working = working.strip()
    create_references(working)
    return working

if __name__ == "__main__":
    strings = (
            'set own css to "new css"',
            'append widget "widget name" to self',
            'append retrieve widget "entry:name" to self',
            'reset widget "foo"',
            'set own content to widget "other widget" content',
            'store own contents at user "first_name"'
            )

