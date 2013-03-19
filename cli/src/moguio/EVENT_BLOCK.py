import pyRex
from lex_base import IGNORE
from lex_functions import everything_until
from EventConsumer import EventConsumer

EVENT_BLOCK = pyRex.Lexer.ParseMap ((
    ("begin",           r"\s*events\s*"                      , IGNORE),
    ("block",           everything_until(r"end events")      , EventConsumer.parse),
    ("end",             r"end events"                        , IGNORE)
))
