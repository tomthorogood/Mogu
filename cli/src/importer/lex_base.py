import re
import pyRex
from regexlib import *
from lex_functions import *

pyRex.Lexer.VERBAL = False

IGNORE = pyRex.Lexer.ParseMap.IGNORE #alias for ease of reading
LITERAL = pyRex.Lexer.ParseMap.LITERAL #alias for ease of reading

regexlib["event_triggers"] = "(%s)" % "|".join(valid_options("event trigger"))
regexlib["widget_types"] = "(%s)" % "|".join(valid_options("widget type"))
regexlib["datatype"] = "(%s)" % ("|".join(valid_options("datatype")))
regexlib["validator_type"] = "(%s)" % ("|".join(valid_options("validator type")))
regexlib["action"]      = "(%s)" % ("|".join(valid_options("action")))
regexlib["object"]      = "(%s)" % ("|".join(valid_options("object")))
regexlib["attribute"]   = "(%s)" % ("|".join(valid_options("attribute")))
regexlib["preposition"] = "(%s)" % ("|".join(valid_options("preposition")))
regexlib["object_set"]  = "%(object)s\s*(%(identifier)s|%(string_literal)s)?(\s+%(attribute)s)?" % regexlib
regexlib["value"]       = "(%(object_set)s|%(string_literal)s|[0-9]+)" % regexlib 

HASH_DEFINITION = pyRex.Lexer.ParseMap((
        ("key",             everything_until(":")   ,   trim),
        ("delim",           ":",                        IGNORE),
        ("value",           everything_until(r"\n") ,   trim),
        ("end",             r"\n\s*"                ,   IGNORE)
))

LIST_DEFINITION = pyRex.Lexer.ParseMap([
        ("entry",           everything_until(r"\n\s*"),     trim),
        ("end",             r"\n\s*"                  ,     IGNORE)
])

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

WIDGET_TYPE = pyRex.Lexer.ParseMap((
        ("begin",   DIRECTIVE_START("type")     , IGNORE),
        ("type",    regexlib["widget_types"]    , syntax.as_integer),
        ("end",     DIRECTIVE_END               , IGNORE)
))

WIDGET_STYLE = pyRex.Lexer.ParseMap((
        ("begin",       DIRECTIVE_START("css")          , IGNORE),
        ("css_classes", "[^\n]+"                        , trim),
        ("end",         DIRECTIVE_END                   , IGNORE)
))

WIDGET_CONTENT = pyRex.Lexer.ParseMap((
        ("begin",       DIRECTIVE_START("content|text")      , IGNORE),
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

WIDGET_TEMPLATE = pyRex.Lexer.ParseMap((
        ("begin",       DIRECTIVE_START("template")     , IGNORE),
        ("template",    regexlib["identifier"]          , reference_template),
        ("end",         DIRECTIVE_END                   , IGNORE)
))

POLICY_MODE  = pyRex.Lexer.ParseMap((
        ("begin",       DIRECTIVE_START("mode")         , IGNORE),
        ("mode",        regexlib["identifier"]          , LITERAL),
        ("end",         DIRECTIVE_END                   , IGNORE)
))

POLICY_DATA = pyRex.Lexer.ParseMap((
        ("begin",           DIRECTIVE_START("data|type")         , IGNORE),
        ("datatype",        regexlib["datatype"]            , trim),
        ("end",             DIRECTIVE_END                   , IGNORE)
))

VALIDATOR_TYPE = pyRex.Lexer.ParseMap((
    ("begin",           DIRECTIVE_START("type")         , IGNORE),
    ("type",            regexlib["validator_type"]      , LITERAL),
    ("end",             DIRECTIVE_END                   , IGNORE)
))

VALIDATOR_TEST = pyRex.Lexer.ParseMap((
    ("begin",           DIRECTIVE_START("test")         , IGNORE),
    ("test",            regexlib["string_literal"]      , LITERAL),
    ("end",             DIRECTIVE_END                   , IGNORE)
))

NEWLINES = pyRex.Lexer.ParseMap([("newline","\n",IGNORE)])
