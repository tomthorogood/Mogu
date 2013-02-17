import pyRex
from lex_base import regexlib
from lex_functions import everything_until
from lex_functions import register_validator
from base_consumers import ValidatorConsumer
from lex_base import IGNORE

VALIDATOR_BLOCK = pyRex.Lexer.ParseMap((
        ("begin",           "\s*validator ",                            IGNORE),
        ("identifier",      regexlib["identifier"],                     register_validator),
        ("validator_def",   everything_until("end validator"),          ValidatorConsumer.parse),
        ("end",             "end validator",                            IGNORE)
))
