
IDENTIFIER = "[:a-zA-Z_][:a-zA-Z_0-9]*"

STRLIT = r'''(?x)   # verbose mode
    (?<!\\)    # not preceded by a backslash
    "          # a literal double-quote
    .*?        # non-greedy 1-or-more characters
    (?<!\\)    # not preceded by a backslash
    "          # a literal double-quote
    ''' 
STRLIT = r'''(?<!\\)".*?(?<!\\)"'''
WHITESPACE = r"\s"
regexlib = {
        "identifier"    :   IDENTIFIER,
        "whitespace"    :   WHITESPACE,
        "string_literal":   STRLIT
        }

