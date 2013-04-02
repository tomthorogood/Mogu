import pyboro

from HashConsumer import HashConsumer
from ListConsumer import ListConsumer
from lex_base import regexlib
from lex_base import IGNORE
from lex_base import LITERAL
from lex_functions import everything_until

HASH_BLOCK = pyboro.Lexer.ParseMap ((
        ("begin",           "\s*hash",                     IGNORE),
        ("hash_def",        everything_until("end hash"),  HashConsumer.parse),
        ("end",             "end hash",                    IGNORE)
))

LIST_BLOCK = pyboro.Lexer.ParseMap ((
        ("begin",           r"\s*list",                     IGNORE),
        ("list_def",        everything_until(r"end list"),  ListConsumer.parse),
        ("end",             r"end list",                    IGNORE)
))

VALUE_DEFINITION = pyboro.Lexer.ParseMap((
    ("begin",       "\s*"               , IGNORE),
    ("value_def",   regexlib["value"]   , LITERAL),
    ("end",         r"\S*"              , IGNORE)
))
