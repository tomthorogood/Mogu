import syntax

reserved_words = syntax.MoguSyntax.keys()

IDENTIFIER = "[:a-zA-Z_][:a-zA-Z_0-9]+"

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
        "math_end"      :   math_en
        }

DIRECTIVE_END = "\s*"

# All mathematical operators
