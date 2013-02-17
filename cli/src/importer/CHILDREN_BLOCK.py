import pyRex
from lex_base import IGNORE
from lex_functions import everything_until
from lex_functions import newline_list

CHILDREN_BLOCK = pyRex.Lexer.ParseMap((
    ("begin",           r"\s*children\s*"                           , IGNORE),
    ("block",           everything_until(r"end children")           , newline_list),
    ("end",             r"end children"                             , IGNORE)
))
