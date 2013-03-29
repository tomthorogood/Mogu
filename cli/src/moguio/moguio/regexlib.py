import syntax
import re

reserved_words = "(%s)" % "|".join(syntax.MoguSyntax.keys())
IDENTIFIER = "(?!(?:%s))[:a-zA-Z_][:a-zA-Z_0-9]+" % reserved_words[1:-1]
#IDENTIFIER = "[:a-zA-Z_][:a-zA-Z_0-9]+" 

#STRLIT = r'''(?x)   # verbose mode
#    (?<!\\)    # not preceded by a backslash
#    "          # a literal double-quote
#    .*?        # non-greedy 1-or-more characters
#    (?<!\\)    # not preceded by a backslash
#    "          # a literal double-quote
#    ''' 
STRLIT = r'''(?<!\\)".*?(?<!\\)"'''

math_op = r'''(\(|\)|\+|\-|\*|\/)'''

# Operators that can begin a math set
# (17 + 4) 
# -17 + 4
math_bg = r'''(\(|\-)'''

# Operators that can end a math set
# (17+4)
math_en = r'''(\))'''

regexlib = {
        "identifier"    :   IDENTIFIER,
        "string_literal":   STRLIT,
        "math_operator" :   math_op,
        "math_begin"    :   math_bg,
        "math_end"      :   math_en,
        "reserved"      :   reserved_words
        }

DIRECTIVE_END = "\s*"

# All mathematical operators
