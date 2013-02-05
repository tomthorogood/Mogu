import pyRex
import syntax
from lex_functions import *
# This file contains information for use in PyRex to lex
# *.mogu files

IGNORE = pyRex.Lexer.ParseMap.IGNORE #alias for ease of reading
LITERAL = pyRex.Lexer.ParseMap.LITERAL #alias for ease of reading

def valid_options (string):
    return [key for key in syntax.syntax if string in syntax.syntax[key][1]]

# a valid identifier for use in mogu will follow
# standard programming identifier conventions:
# it can start with an alpha character or underscore,
# and be followed by any number of alpha characters, underscores,
# and numbers
#
# Mogu widgets may also start with a ':', signifying that 
# their name should be an appendage to their parent's name. For instance:
#
#
# widget parent_widget
#   ...
#   children 
#       :child_widget
#   end children
# end widget
#
#
# widget :child_widget
#
#   ...
# end widget

IDENTIFIER = "[:a-zA-Z_][:a-zA-Z_0-9]*"
STRLIT = r'''(?x)   # verbose mode
    (?<!\\)    # not preceded by a backslash
    "          # a literal double-quote
    .*?        # non-greedy 1-or-more characters
    (?<!\\)    # not preceded by a backslash
    "          # a literal double-quote
    ''' 
WHITESPACE = "[ \t]"

regexlib = {
        "identifier"    :   IDENTIFIER,
        "whitespace"    :   WHITESPACE,
        "string_literal":   STRLIT
        }

# A widget block looks like this:
    # widget widgetname 
    #   ...
    # end widget
# where widgetname is a valid identifier, and
# where '...' represents everything until 'end widget' is reached as the
# only entry on a line.

WIDGET_BLOCK = pyRex.Lexer.ParseMap ((
        ( "begin",          "widget ",              IGNORE),
        ( "identifier",     IDENTIFIER,             LITERAL),
        ( "widget_def",     "[^(\nend widget\n)]",  register_widget),
        ( "end",            "\nend widget\n",       IGNORE)
))

EVENT_BLOCK = pyRex.Lexer.ParseMap ((
    ("begin",           "%(whitespace)s*events[ \t]*\n" % regexlib,     IGNORE),
        ("block",           "[^(end events\n)]",                        LITERAL),
        ("end",             "end events\n",                             IGNORE)
))

TEMPLATE_BLOCK = pyRex.Lexer.ParseMap((
        ("begin",           "%(whitespace)s*template " % regexlib,      IGNORE),
        ("identifier",      IDENTIFIER,                                 register_template),
        ("template_def",    "[^(\nend template\n)]",                    LITERAL),
        ("end",             "\nend template\n",                         IGNORE)
))

POLICY_BLOCK = pyRex.Lexer.ParseMap((
        ("begin",           "%(whitespace)s*policy " % regexlib,        IGNORE),
        ("identifier",      IDENTIFIER,                                 register_policy),
        ("policy_def",      "[^(\nend policy\n)]",                      LITERAL),
        ("end",             "\nend policy\n",                           IGNORE)
))

DATA_BLOCK = pyRex.Lexer.ParseMap((
        ("begin",           "%(whitespace)s*data " % regexlib,          IGNORE),
        ("identifier",      IDENTIFIER,                                 register_data),
        ("data_def",        "[^(\nend data\n)]",                        LITERAL),
        ("end",             "\nend data\n",                             IGNORE)
))

VALIDATOR_BLOCK = pyRex.Lexer.ParseMap((
        ("begin",           "%(whitespace)s*validator " % regexlib,     IGNORE),
        ("identifier",      IDENTIFIER,                                 register_validator),
        ("validator_def",   "[^(\nend validator\n)]",                   LITERAL),
        ("end",             "\nend validator\n",                        IGNORE)
))

CHILDREN_BLOCK = pyRex.Lexer.ParseMap((
        ("begin",           "%(whitespace)s*children$(whitespace)s*\n", IGNORE),
        ("block",           "*([ \t]*%s[ \t]*\n)+" % IDENTIFIER,        LITERAL),
        ("end",             "end children\n",                           IGNORE)
))

VALID_EVENT_TRIGGERS = "(%s)" % "|".join(valid_options("event trigger"))


# a "when" block looks like this:
# when triggered {
#   do this
#   and this
#   and this
# }
WHEN_BLOCK = pyRex.Lexer.ParseMap ((
        ("begin",       "%(whitespace)s*when%(whitespace)s*",           IGNORE),
        ("trigger",     VALID_EVENT_TRIGGERS,                           syntax.as_integer),
        ("block_begin", "$(whitespace)s*\{",                            IGNORE),
        ("block",       "[^(\}\n)]+",                                   LITERAL),
        ("block_end",   "[ \t]*\}[ \t]*\n",                             IGNORE)
))

########################
# Widget block entries #
########################
#
# A widget block will look like this. It will not have all of the entries 
# listed, but will have at least some of them. Different widget types have
# different entries which they support.
#
# widget my_widget 
#   type    :   input
#   css     :   "red_border green_text"
#   source  :   "/resources/foo.png"
#   location:   "http://www.foobar.com"
#   content :   "This is a foo!"
#   template:   template_widget
#   events
#       when clicked {
#           command I
#           command II
#       }
#       when mouseover {
#           command III
#       }
#       when changed {
#           command IV
#       }
#   end events
# end widget


def DIRECTIVE_START(token):
    return r"[\n\t ]*%s[\t ]*:[\t ]*" % token
DIRECTIVE_END = "[\t ]*\n"

# Widget Type #


# This will require the use of Mogu's syntax.py file, which has a dictionary 
# whose keys represent value mogu syntax, and whose values are a tuple of
# (repr,notes)
# where 'repr' is an integral value, and 'notes' contain tags for the keys.

VALID_W_TYPES = "(%s)" % "|".join(valid_options("widget type"))

WIDGET_TYPE = pyRex.Lexer.ParseMap((
        ("begin",   DIRECTIVE_START("type")     , IGNORE),
        ("type",    VALID_W_TYPES               , syntax.as_integer),
        ("end",     DIRECTIVE_END               , IGNORE)
))

# Widget Styling #

# Widget styling is delinated by a 
#   css :   "foo"

# where "foo" represents at least one valid css class
VALID_CSS_CLASSES = "((%s ) +)+" % IDENTIFIER

WIDGET_STYLE = pyRex.Lexer.ParseMap((
        ("begin",       DIRECTIVE_START("css")          , IGNORE),
        ("css_classes", VALID_CSS_CLASSES               , LITERAL),
        ("end",         DIRECTIVE_END                   , IGNORE)
))

# Widget Contents #

# A widget's contents can contain any raw string or Mogu command. 
# Therefore, the first iteration over the contents will simply take
# the contents as a literal. It can later be determined whether the
# contents are a string or Mogu command. 
#
# Other tags which follow a similar pattern are listed here as well. 

WIDGET_CONTENT = pyRex.Lexer.ParseMap((
        ("begin",       DIRECTIVE_START("content")      , IGNORE),
        ("content",     "[^\n]*"                        , LITERAL),
        ("end",         DIRECTIVE_END                   , IGNORE)
))

WIDGET_SOURCE = pyRex.Lexer.ParseMap((
        ("begin",       DIRECTIVE_START("source")       , IGNORE),
        ("source",      "[^\n]*"                        , LITERAL),
        ("end",         DIRECTIVE_END                   , IGNORE)
))

WIDGET_LOCATION = pyRex.Lexer.ParseMap((
        ("begin",       DIRECTIVE_START("location")     , IGNORE),
        ("location",    "[^\n]*"                        , LITERAL),
        ("end",         DIRECTIVE_END                   , IGNORE)
))

# Widget Template #

# A template value must be a value listed in the "templates" keyspace.
# When importing MoguScript, the argumet will be checked to ensure that
# the template itself exists. 

WIDGET_TEMPLATE = pyRex.Lexer.ParseMap((
        ("begin",       DIRECTIVE_START("template")     , IGNORE),
        ("template",    IDENTIFIER                      , check_template_exists),
        ("end",         DIRECTIVE_END                   , IGNORE)
))

POLICY_MODE  = pyRex.Lexer.ParseMap((
        ("begin",       DIRECTIVE_START("mode")         , IGNORE),
        ("mode",        IDENTIFIER                      , check_policy_exists),
        ("end",         DIRECTIVE_END                   , IGNORE)
))

regexlib["datatype"] = "(%s)" % ("|".join(valid_options("datatype")))
POLICY_DATA = pyRex.Lexer.ParseMap((
        ("begin",       DIRECTIVE_START("data")         , IGNORE),
        ("data",        regexlib["datatype"]            , LITERAL),
        ("end",         DIRECTIVE_END                   , IGNORE)
))

#################
# MOGU COMMANDS #
#################

# Fundamentally, commands do one thing: They perform a 
# single action on a single value. Therefore, every
# command will have at LEAST this basic structure:
# 
#   action value
#
#
# In almost all cases, 'value' can be substituted with
# an object, or an object's attribute.
#
#   action [object]
#   action [object] [attribute]
#
# Some commands (such as those that change values, or store them)
# take a preposition and second value:
#
#   action [value] [to|at] [value]
#   action [object] [to|at] [value]
#   action [object][attribute] [to|at] [value]
#   ...and so on.

# Therefore, the first thing we have to do is get a list of 
# valid actions.

regexlib["action"]      = "(%s)" % ("|".join(valid_options("action")))
regexlib["object"]      = "(%s)" % ("|".join(valid_options("object")))
regexlib["attribute"]   = "(%s)" % ("|".join(valid_options("attribute")))
regexlib["preposition"] = "(%s)" % ("|".join(valid_options("prepositions")))

# Now, a value can be any of the following:
#   IDENTIFIER
#   VALID_OBJECT
#   VALID_OBJECT > VALID_ATTR
regexlib["value"] = "(%(identifier)s|$(object)s[[ \t]*[ \t]*%(attribute)s]?|%(string_literal)s)" % regexlib 


# And then we can bring all of this in together:
MOGU_CMD = pyRex.Lexer.ParseMap((
    ("begin"    ,   "%(whitespace)s" % regexlib,                IGNORE),
    ("action"   ,   "%(action)s" % regexlib   ,                 syntax.as_integer),
    ("dir_obj"  ,   "%(value)s" % regexlib    ,                 LITERAL),
    ("opt_arg"  ,   "(%(preposition)s %(value)s)?" % regexlib,  LITERAL)
))
