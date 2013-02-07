import pyRex
import syntax
from regexlib import regexlib
from lex_functions import *
import re
# This file contains information for use in PyRex to lex
# *.mogu files

IGNORE = pyRex.Lexer.ParseMap.IGNORE #alias for ease of reading
LITERAL = pyRex.Lexer.ParseMap.LITERAL #alias for ease of reading

def valid_options (string):
    return [key for key in syntax.MoguSyntax if string in syntax.MoguSyntax[key][1]]

def trim(string):
    return string.strip()

def everything_until(string):
    return r"(.|\n)+(?=%s)" % string

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


# A widget block looks like this:
    # widget widgetname 
    #   ...
    # end widget
# where widgetname is a valid identifier, and
# where '...' represents everything until 'end widget' is reached as the
# only entry on a line.


NEWLINES = pyRex.Lexer.ParseMap([("newline","\n",IGNORE)])

WIDGET_BLOCK = pyRex.Lexer.ParseMap ((
        ( "begin",          "widget ",                          IGNORE),
        ( "identifier",     regexlib["identifier"],             register_widget),
        ("widget_def",      everything_until("end widget"),     trim),
        ( "end",            r"end widget",                      IGNORE)
))

EVENT_BLOCK = pyRex.Lexer.ParseMap ((
    ("begin",           r"\s*events\s*"                      , IGNORE),
    ("block",           everything_until(r"end events")      , trim),
    ("end",             r"end events"                        , IGNORE)
))

CHILDREN_BLOCK = pyRex.Lexer.ParseMap((
    ("begin",           r"\s*children\s*"                           , IGNORE),
    ("block",           everything_until(r"end children")           , trim),
    ("end",             r"end children"                             , IGNORE)
))

TEMPLATE_BLOCK = pyRex.Lexer.ParseMap((
        ("begin",           "%(whitespace)s*template " % regexlib,      IGNORE),
        ("identifier",      regexlib["identifier"],                     register_template),
        ("template_def",    everything_until("end template"),           trim),
        ("end",             "end template",                             IGNORE)
))

POLICY_BLOCK = pyRex.Lexer.ParseMap((
        ("begin",           "%(whitespace)s*policy " % regexlib,        IGNORE),
        ("identifier",      regexlib["identifier"],                     register_policy),
        ("policy_def",      everything_until("end policy"),             trim),
        ("end",             "end policy",                               IGNORE)
))

DATA_BLOCK = pyRex.Lexer.ParseMap((
        ("begin",           "%(whitespace)s*data " % regexlib,          IGNORE),
        ("identifier",      regexlib["identifier"],                     register_data),
        ("data_def",        everything_until("end data"),               trim),
        ("end",             "end data",                                 IGNORE)
))

VALIDATOR_BLOCK = pyRex.Lexer.ParseMap((
        ("begin",           "%(whitespace)s*validator " % regexlib,     IGNORE),
        ("identifier",      regexlib["identifier"],                     register_validator),
        ("validator_def",   everything_until("end validator"),          trim),
        ("end",             "end validator",                            IGNORE)
))


VALID_EVENT_TRIGGERS = "(%s)" % "|".join(valid_options("event trigger"))

# a "when" block looks like this:
# when triggered {
#   do this
#   and this
#   and this
# }
WHEN_BLOCK = pyRex.Lexer.ParseMap ((
        ("begin",       "\s*when\s+",                                   IGNORE),
        ("trigger",     VALID_EVENT_TRIGGERS,                           syntax.as_integer),
        ("block_begin", "\s*{",                                         IGNORE),
        ("block",       everything_until("\}"),                         trim),
        ("block_end",   "\}\s*\n",                                      IGNORE)
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
    d =  r"\s*%s\s*:\s*" % token
    return d

DIRECTIVE_END = "\s*"

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

WIDGET_STYLE = pyRex.Lexer.ParseMap((
        ("begin",       DIRECTIVE_START("css")          , IGNORE),
        ("css_classes", "[^\n]+"                        , trim),
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
        ("content",     "[^\n]*"                        , trim),
        ("end",         DIRECTIVE_END                   , IGNORE)
))

WIDGET_SOURCE = pyRex.Lexer.ParseMap((
        ("begin",       DIRECTIVE_START("source")       , IGNORE),
        ("source",      "[^\n]*"                        , trim),
        ("end",         DIRECTIVE_END                   , IGNORE)
))

WIDGET_LOCATION = pyRex.Lexer.ParseMap((
        ("begin",       DIRECTIVE_START("location")     , IGNORE),
        ("location",    "[^\n]*"                        , trim),
        ("end",         DIRECTIVE_END                   , IGNORE)
))

# Widget Template #

# A template value must be a value listed in the "templates" keyspace.
# When importing MoguScript, the argumet will be checked to ensure that
# the template itself exists. 

WIDGET_TEMPLATE = pyRex.Lexer.ParseMap((
        ("begin",       DIRECTIVE_START("template")     , IGNORE),
        ("template",    regexlib["identifier"]          , check_template_exists),
        ("end",         DIRECTIVE_END                   , IGNORE)
))

POLICY_MODE  = pyRex.Lexer.ParseMap((
        ("begin",       DIRECTIVE_START("mode")         , IGNORE),
        ("mode",        regexlib["identifier"]          , check_policy_exists),
        ("end",         DIRECTIVE_END                   , IGNORE)
))

regexlib["datatype"] = "(%s)" % ("|".join(valid_options("datatype")))
POLICY_DATA = pyRex.Lexer.ParseMap((
        ("begin",           DIRECTIVE_START("data")         , IGNORE),
        ("datatype",        regexlib["datatype"]            , trim),
        ("end",             DIRECTIVE_END                   , IGNORE)
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
regexlib["preposition"] = "(%s)" % ("|".join(valid_options("preposition")))
regexlib["object_set"]  = "(%(object)s|%(identifier)s)(\s+%(attribute)s)?" % regexlib
regexlib["value"]       = "(%(object_set)s|%(string_literal)s|[0-9]+)" % regexlib 
MOGU_CMD = pyRex.Lexer.ParseMap((
    ("begin"    ,   "\s*"                                                                           , IGNORE),
    ("action"   ,   "%(action)s" %regexlib                                                          , syntax.as_integer),
    ("ws"       ,   "\s+"                                                                           , IGNORE),
    ("dir_obj"  ,   "%(object_set)s" %regexlib                                                      , trim),
    ("opt_end"  ,   "(\s+%(preposition)s\s+%(value)s)?"%regexlib , trim),
    ("end"      ,   "\s*"                                                                           , IGNORE)
))



if __name__ == "__main__":
    pyRex.Lexer.VERBAL = True
    to_test = [
        WIDGET_BLOCK,
        EVENT_BLOCK,
        TEMPLATE_BLOCK,
        POLICY_BLOCK,
        DATA_BLOCK,
        VALIDATOR_BLOCK,
        CHILDREN_BLOCK,
        WHEN_BLOCK,
        WIDGET_TYPE,
        WIDGET_STYLE,
        WIDGET_CONTENT,
        WIDGET_SOURCE,
        WIDGET_LOCATION,
        WIDGET_TEMPLATE,
        POLICY_MODE,
        POLICY_DATA,
        MOGU_CMD,
        NEWLINES
    ]

    test_consumer = pyRex.Consumer.Consumer(to_test)
    input_string = ""
    with open("syntax_test.mogu") as f:
        input_string = f.read()
    results = test_consumer.parse(input_string)
    for result in results:
        print(result[0])

