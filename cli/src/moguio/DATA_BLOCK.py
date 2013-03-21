import pyboro
from lex_base import IGNORE
from lex_base import regexlib
from lex_functions import register_data
from lex_functions import everything_until
from DataConsumer import DataConsumer

DATA_BLOCK = pyboro.Lexer.ParseMap((
        ("begin",           "\s*data ",                                 IGNORE),
        ("identifier",      regexlib["identifier"],                     register_data),
        ("data_def",        everything_until(r"end data"),              DataConsumer.parse),
        ("end",             r"end data",                                IGNORE)
))
