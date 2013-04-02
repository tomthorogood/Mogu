from lex_functions import everything_until
from lex_functions import newline_list
from lex_functions import reference_widget_list
from lex_functions import register_widget
from lex_functions import register_template
from lex_functions import register_data
from lex_functions import register_validator
from lex_functions import register_policy

from lex_base import regexlib
from lex_base import NEWLINES
from lex_base import POLICY_MODE
from lex_base import POLICY_DATA
from lex_base import VALIDATOR_TYPE
from lex_base import VALIDATOR_TEST
from lex_base import HASH_DEFINITION
from lex_base import LIST_DEFINITION
from lex_base import WIDGET_CONTENT
from lex_base import WIDGET_TYPE
from lex_base import WIDGET_SOURCE
from lex_base import WIDGET_LOCATION
from lex_base import WIDGET_TEMPLATE
from lex_base import WIDGET_STYLE
from lex_base import CHILDREN_BLOCK

from lex_base import LITERAL
from lex_base import IGNORE

import syntax
import MoguString

from pyboro import Consumer
from pyboro import Lexer

import re

################################
# BEGIN SECOND TIER PARSE MAPS #
################################

MOGU_CMD = Lexer.ParseMap((
    ("begin"    ,   "^"                                                                           , IGNORE),
    ("action"   ,   "%(action)s" %regexlib                                                          , syntax.as_integer),
    ("ws"       ,   "\s+"                                                                           , IGNORE),
    ("dir_obj"  ,   "%(object_set)s" %regexlib                                                      , MoguString.importString),
    ("opt_end"  ,   "(\s*%(preposition)s\s+%(value)s)?"%regexlib                                    , MoguString.importString),
    ("end"      ,   "$"                                                                           , IGNORE)
), strip=True, flags=re.M)


#######################################################
#               BEGIN BASE CONSUMERS                  #
#                                                     #
# Base consumers are made up of ParseMaps that do not #
# refer to other consumers, but are depended upon.    #
#######################################################

PolicyConsumer = Consumer.Consumer([
    POLICY_MODE,
    POLICY_DATA,
    NEWLINES
], help="A list of key : value attributes defining a particular storage policy")

ValidatorConsumer = Consumer.Consumer([
    VALIDATOR_TYPE,
    VALIDATOR_TEST,
    NEWLINES
], help="A list of key : value attributes defining a particular validator's use")

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
], help="""
    list
        item 1
        item 2
        ...
    end list
""")

WhenConsumer = Consumer.Consumer([
    MOGU_CMD,
    NEWLINES
],help="action [object [identifier|attribute]] [preposition value] (on one line)")

#################################
# BEGIN NEXT TIER OF PARSE MAPS #
#################################

HASH_BLOCK = Lexer.ParseMap ((
        ("begin",           "\s*hash",                     IGNORE),
        ("hash_def",        everything_until("end hash"),  HashConsumer.parse),
        ("end",             "end hash",                    IGNORE)
))

LIST_BLOCK = Lexer.ParseMap ((
        ("begin",           r"\s*list",                     IGNORE),
        ("list_def",        everything_until(r"end list"),  ListConsumer.parse),
        ("end",             r"end list",                    IGNORE)
))

VALUE_DEFINITION = Lexer.ParseMap((
    ("begin",       "\s*"               , IGNORE),
    ("value_def",   regexlib["value"]   , LITERAL),
    ("end",         r"\S*"              , IGNORE)
))

VALIDATOR_BLOCK = Lexer.ParseMap((
        ("begin",           "\s*validator ",                            IGNORE),
        ("identifier",      regexlib["identifier"],                     register_validator),
        ("validator_def",   everything_until("end validator"),          ValidatorConsumer.parse),
        ("end",             "end validator",                            IGNORE)
))

WHEN_BLOCK = Lexer.ParseMap ((
        ("begin",       r"^when\s+",                IGNORE),
        ("trigger",     regexlib["event_triggers"], syntax.as_integer),
        ("block_begin", r"\s*{",                    IGNORE),
        ("block",       everything_until("\}"),     WhenConsumer.parse),
        ("block_end",   r"\}$",                     IGNORE)
),strip=True,flags=re.M)

POLICY_BLOCK = Lexer.ParseMap((
        ("begin",           "\s*policy ",                               IGNORE),
        ("identifier",      regexlib["identifier"],                     register_policy),
        ("policy_def",      everything_until("end policy"),             PolicyConsumer.parse),
        ("end",             "end policy",                               IGNORE)
))
################################
# BEGIN NEXT TIER OF CONSUMERS #
################################

DataConsumer = Consumer.Consumer([
    HASH_BLOCK,      
    LIST_BLOCK,
    VALUE_DEFINITION,
    NEWLINES
    ], help="A data block consisiting of a hash dictionary (key:value pairs), a list (many items each on its own line), or a single value")

EventConsumer = Consumer.Consumer([
    WHEN_BLOCK,
    NEWLINES
], help="A set of 'when [trigger] { [command list] }' blocks")

#################################
# BEGIN NEXT TIER OF PARSE MAPS #
#################################

EVENT_BLOCK = Lexer.ParseMap ((
    ("begin",           r"\s*events\s*"                      , IGNORE),
    ("block",           everything_until(r"end events")      , EventConsumer.parse),
    ("end",             r"end events"                        , IGNORE)
))

DATA_BLOCK = Lexer.ParseMap((
        ("begin",           "\s*data ",                                 IGNORE),
        ("identifier",      regexlib["identifier"],                     register_data),
        ("data_def",        everything_until(r"end data"),              DataConsumer.parse),
        ("end",             r"end data",                                IGNORE)
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
    EVENT_BLOCK,
    CHILDREN_BLOCK,
    NEWLINES
], help="""
widget [identifier]
    key : value
    ...

    [children
        child1
        ...
    end children]

    [events
        when [trigger] {
            command1
            command2 
            ...
        }
    end events]
end widget
""")

#################################
# BEGIN NEXT TIER OF PARSE MAPS #
#################################

TEMPLATE_BLOCK = Lexer.ParseMap((
        ("begin",           "\s*template ",                     IGNORE),
        ("identifier",      regexlib["identifier"],             register_template),
        ("template_def",    everything_until("end template"),   WidgetConsumer.parse),
        ("end",             "end template",                     IGNORE)
))

WIDGET_BLOCK = Lexer.ParseMap ((
        ( "begin",          "widget ",                          IGNORE),
        ( "identifier",     regexlib["identifier"],             register_widget),
        ("widget_def",      everything_until("end widget"),     WidgetConsumer.parse),
        ( "end",            r"end widget",                      IGNORE)
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
    NEWLINES,
], help="Basic Mogu syntax in the form of object blocks. See http://www.github.com/tomthorogood/gomogu for more assistance.")
