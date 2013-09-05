from lex_functions import everything_until
from lex_functions import newline_list
from lex_functions import add_definition
from lex_functions import add_references

from RegexLib import regexlib
from lex_base import NEWLINES
from lex_base import POLICY_DATA
from lex_base import POLICY_DEFAULT
from lex_base import POLICY_ENCRYPTION
from lex_base import VALIDATOR_TYPE
from lex_base import VALIDATOR_TEST
from lex_base import HASH_DEFINITION
from lex_base import LIST_DEFINITION
from lex_base import WIDGET_CONTENT
from lex_base import WIDGET_TYPE
from lex_base import WIDGET_SOURCE
from lex_base import WIDGET_LOCATION
from lex_base import WIDGET_TEMPLATE
from lex_base import WIDGET_SORT
from lex_base import WIDGET_STYLE
from lex_base import WIDGET_VALIDATOR
from lex_base import CHILDREN_BLOCK

from lex_base import LITERAL
from lex_base import IGNORE

import syntax
import MoguString

from pyboro import Consumer
from pyboro import Lexer

import re

def convert_to_script(s):
    s = MoguString("integral", s)
    s = s.translate("script")
    maxlen = 80 if len(s) > 80 else len(s)
    return s[:maxlen]

################################
# BEGIN SECOND TIER PARSE MAPS #
################################

MOGU_CMD = Lexer.ParseMap((
    ("begin"    ,   "^"                                            , IGNORE),
    ("action"   ,   "%(action)s" %regexlib                         , LITERAL),
    ("ws"       ,   "\s+"                                          , IGNORE),
    ("dir_obj"  ,   "%(object_set)s" %regexlib                     , add_references),
    ("opt_end"  ,   "(\s*%(preposition)s(\s+%(location)s)?\s+%(value)s)?"%regexlib   , add_references),
    ("end"      ,   "$"                                            , IGNORE)
), strip=True, flags=re.M)


#######################################################
#               BEGIN BASE CONSUMERS                  #
#                                                     #
# Base consumers are made up of ParseMaps that do not #
# refer to other consumers, but are depended upon.    #
#######################################################


ValidatorConsumer = Consumer.Consumer([VALIDATOR_TYPE, VALIDATOR_TEST, NEWLINES],
    "A list of key : value attributes defining a particular validator's use",
    convert_to_script)

HashConsumer = Consumer.Consumer([
    HASH_DEFINITION,
    NEWLINES
], help="""
    hash
        key : value
        ...
    end hash
""")

ListConsumer = Consumer.Consumer([
    LIST_DEFINITION,
    NEWLINES
], """
    list
        item 1
        item 2
        ...
    end list
""", convert_to_script)

WhenConsumer = Consumer.Consumer([MOGU_CMD, NEWLINES],
    "Malformed event command; did you forget \"widget\"?", convert_to_script)

#################################
# BEGIN NEXT TIER OF PARSE MAPS #
#################################

t = "perspective"
PERSPECTIVE_BLOCK = Lexer.ParseMap ((
        ("begin",          "\s*%s\s+"%t,                IGNORE),
        ("identifier",     regexlib["identifier"],      LITERAL),
        ("block",      everything_until(r"end\s+%s"%t), WhenConsumer.parse),
        ("end",            r"end\s+%s"%t,               IGNORE)
))

t = syntax.as_integer("hash")
HASH_BLOCK = Lexer.ParseMap ((
        ("begin",           "\s*%s"%t,                      IGNORE),
        ("hash_def",        everything_until("end %s"%t),   HashConsumer.parse),
        ("end",             "end %s" %t,                    IGNORE)
))

t = syntax.as_integer("list")
LIST_BLOCK = Lexer.ParseMap ((
        ("begin",           r"\s*%s"%t,                     IGNORE),
        ("list_def",        everything_until(r"end %s"%t),  ListConsumer.parse),
        ("end",             r"end %s"%t,                    IGNORE)
))

VALUE_DEFINITION = Lexer.ParseMap((
    ("begin",       "\s*"               , IGNORE),
    ("value_def",   regexlib["value"]   , LITERAL),
    ("end",         r"\S*"              , IGNORE)
))

t = syntax.as_integer("validator") 
VALIDATOR_BLOCK = Lexer.ParseMap((
    ("begin",           "\s*%d\s+"%t,                            IGNORE),
    ("identifier",      regexlib["identifier"],\
            lambda s: add_definition(syntax.as_integer("validator"),s)),
    ("validator_def",   everything_until("end %s"%t),          ValidatorConsumer.parse),
    ("end",             "end %d"%t,                            IGNORE)
))

WHEN_BLOCK = Lexer.ParseMap ((
    ("begin",       r"^when\s+",                IGNORE),
    ("trigger",     regexlib["event_triggers"], LITERAL),
    ("block_begin", r"\s*{",                    IGNORE),
    ("block",       everything_until("\}"),     WhenConsumer.parse),
    ("block_end",   r"\}$",                     IGNORE),
),strip=True,flags=re.M)

################################
# BEGIN NEXT TIER OF CONSUMERS #
################################

PolicyConsumer = Consumer.Consumer([
    POLICY_DATA,
    POLICY_ENCRYPTION,
    POLICY_DEFAULT,
    HASH_BLOCK,
    LIST_BLOCK,
    NEWLINES
], "A list of key : value attributes defining a particular storage policy",
convert_to_script)

DataConsumer = Consumer.Consumer([
    HASH_BLOCK,      
    LIST_BLOCK,
    VALUE_DEFINITION,
    NEWLINES
    ],"A data block consisiting of a hash dictionary (key:value pairs), a list (many items each on its own line), or a single value",
    convert_to_script)

EventConsumer = Consumer.Consumer([
    WHEN_BLOCK,
    NEWLINES
], "A set of 'when [trigger] { [command list] }' blocks", convert_to_script)

#################################
# BEGIN NEXT TIER OF PARSE MAPS #
#################################
t = "policy"
POLICY_BLOCK = Lexer.ParseMap((
    ("begin",           "\s*%s\s+"%t,                    IGNORE),
    ("identifier",      regexlib["identifier"],\
            lambda s: add_definition("policy",s)),
    ("policy_def",      everything_until(r"end\s+%s"%t), PolicyConsumer.parse),
    ("end",             r"end\s+%s"%t,                   IGNORE)
))

EVENT_BLOCK = Lexer.ParseMap ((
    ("begin",  r"\s*events\s*"                 , IGNORE),
    ("block",  everything_until(r"end events") , EventConsumer.parse),
    ("end",    r"end events"                   , IGNORE)
))

t = syntax.as_integer("data")
DATA_BLOCK = Lexer.ParseMap((
    ("begin",      "\s*%s "%t,                     IGNORE),
    ("identifier", regexlib["identifier"],
        lambda s: add_definition(syntax.as_integer("data"),s)),
    ("data_def",   everything_until(r"end %s"%t),  DataConsumer.parse),
    ("end",        r"end %s"%t,                    IGNORE),
))

################################
# BEGIN NEXT TIER OF CONSUMERS #
################################

WidgetConsumer = Consumer.Consumer([
    WIDGET_TYPE,
    WIDGET_CONTENT,
    WIDGET_SOURCE,
    WIDGET_LOCATION,
    WIDGET_TEMPLATE,
    WIDGET_STYLE,
    WIDGET_VALIDATOR,
    WIDGET_SORT,
    EVENT_BLOCK,
    CHILDREN_BLOCK,
    NEWLINES
], "A complete widget definition (did you forget 'end widget'?)",
convert_to_script)

#################################
# BEGIN NEXT TIER OF PARSE MAPS #
#################################

t = syntax.as_integer("template")
TEMPLATE_BLOCK = Lexer.ParseMap((
    ("begin",           "\s*%s\s+"%t,                   IGNORE),
    ("identifier",      regexlib["identifier"],\
        lambda s: add_definition(syntax.as_integer("template"),s)),
    ("template_def",    everything_until("end %s"%t),   WidgetConsumer.parse),
    ("end",             "end\s+%s"%t,                   IGNORE)
))

t = syntax.as_integer("widget")
WIDGET_BLOCK = Lexer.ParseMap ((
    ( "begin",      "\s*%s\s+"%t,                    IGNORE),
    ( "identifier", regexlib["identifier"],
        lambda s: add_definition(syntax.as_integer("widget"),s)),
    ("widget_def",  everything_until(r"end\s+%s"%t), WidgetConsumer.parse),
    ( "end",        r"end\s+%s"%t,                   IGNORE)
))


################################
# BEGIN NEXT TIER OF CONSUMERS #
################################

RootConsumer = Consumer.Consumer([
    WIDGET_BLOCK,
    TEMPLATE_BLOCK,
    POLICY_BLOCK,
    DATA_BLOCK,
    VALIDATOR_BLOCK,
    PERSPECTIVE_BLOCK,
    NEWLINES,
], "blocks of valid MoguScript.", convert_to_script)
