import pyRex
import syntax
from lex_base import IGNORE
from lex_base import regexlib
from lex_functions import everything_until
from WhenConsumer import WhenConsumer

# a "when" block looks like this:
# when triggered {
#   do this
#   and this
#   and this
# }
WHEN_BLOCK = pyRex.Lexer.ParseMap ((
        ("begin",       "\s*when\s+",               IGNORE),
        ("trigger",     regexlib["event_triggers"], syntax.as_integer),
        ("block_begin", "\s*{",                     IGNORE),
        ("block",       everything_until("\}"),     WhenConsumer.parse),
        ("block_end",   "\}\s*\n",                  IGNORE)
))
