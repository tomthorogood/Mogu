import pyRex

from HashConsumer import HashConsumer
from ListConsumer import ListConsumer
from lex_base import regexlib
from lex_base import DIRECTIVE_END
from lex_base import IGNORE
from lex_base import LITERAL
from lex_functions import everything_until

HASH_BLOCK = pyRex.Lexer.ParseMap ((
        ("begin",           "\s*hash",                             IGNORE),
        ("hash_def",        everything_until("end hash"),       HashConsumer.parse),
        ("end",             "end hash",                         IGNORE)
))

LIST_BLOCK = pyRex.Lexer.ParseMap ((
        ("begin",           r"\s*list",                             IGNORE),
        ("list_def",        everything_until(r"end list"),       ListConsumer.parse),
        ("end",             r"end list",                         IGNORE)
))

VALUE_DEFINITION = pyRex.Lexer.ParseMap((
    ("begin",       "\s*"               , IGNORE),
    ("value_def",   regexlib["value"]   , LITERAL),
    ("end",         DIRECTIVE_END       , IGNORE)
))
