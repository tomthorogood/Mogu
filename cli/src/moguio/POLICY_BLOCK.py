import pyRex
from base_consumers import PolicyConsumer
from lex_functions import register_policy
from lex_functions import everything_until
from lex_base import regexlib
from lex_base import IGNORE

POLICY_BLOCK = pyRex.Lexer.ParseMap((
        ("begin",           "\s*policy ",                               IGNORE),
        ("identifier",      regexlib["identifier"],                     register_policy),
        ("policy_def",      everything_until("end policy"),             PolicyConsumer.parse),
        ("end",             "end policy",                               IGNORE)
))
