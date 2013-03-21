import pyboro
from WidgetConsumer import WidgetConsumer
from lex_base import IGNORE
from lex_base import regexlib
from lex_functions import register_template
from lex_functions import register_widget
from lex_functions import everything_until

TEMPLATE_BLOCK = pyboro.Lexer.ParseMap((
        ("begin",           "\s*template ",      IGNORE),
        ("identifier",      regexlib["identifier"],                     register_template),
        ("template_def",    everything_until("end template"),         WidgetConsumer.parse),
        ("end",             "end template",                             IGNORE)
))

WIDGET_BLOCK = pyboro.Lexer.ParseMap ((
        ( "begin",          "widget ",                          IGNORE),
        ( "identifier",     regexlib["identifier"],             register_widget),
        ("widget_def",      everything_until("end widget"),     WidgetConsumer.parse),
        ( "end",            r"end widget",                      IGNORE)
))
