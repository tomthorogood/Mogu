# A set of submodules to replace human readable syntax
# with integral representations

import syntax # Syntax/Integer Map
from pyRex import Consumer
from pyRex import Lexer
from regexlib import regexlib # Basic regular expressions
from lex import everything_until
import re

def find_syntax_tokens(string):
    """
    Creates a list of valid syntax tokens from 'syntax.py', and 
    searches the input string for each of the tokens, returning
    the tokens found as a list. Sorts the list of tokens by
    length first to be sure that the largest inputs are found
    first.
    """
    ltokens = [key for key in syntax.MoguSyntax]
    ltokens = sorted(ltokens,key=len)
    ltokens.reverse()
    tokens = "(%s)" % ("|".join(ltokens))

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
        working = working.replace(token,str(syntax.as_integer(token)))
    working = working % tuple(literals)
    return working

if __name__ == "__main__":
    import random
    literals = ['"hello"','"goodbye"','"doctordonna"','"tardis"','"bad wolf"']
    def create_string(num=4):
        string = ""
        for i in range(num):
            string += "%s %s " % (random.choice(literals),random.choice([key for key in syntax.MoguSyntax]))
        return string.strip()

    rstring = create_string()
    print(rstring)
    print(replace_syntax(rstring))
    print(replace_syntax("set own css to \"new css\""))
    print(replace_syntax('"this is a test string" image link'))
