import re
import pyboro
from regexlib import *
from lex_functions import *

# Control of debugging vomit
# Can be changed in higher level modules
pyboro.Lexer.VERBAL = False

IGNORE = pyboro.Lexer.ParseMap.IGNORE #alias for ease of reading
LITERAL = pyboro.Lexer.ParseMap.LITERAL #alias for ease of reading

def option_list(grammar_type):
    return "(%s)" % "|".join(valid_options(grammar_type))

regexlib["event_triggers"]  = option_list("event trigger")
regexlib["widget_types"]    = option_list("widget type")
regexlib["datatype"]        = option_list("datatype")
regexlib["validator_type"]  = option_list("validator type")
regexlib["action"]          = option_list("action")
regexlib["object"]          = option_list("object")
regexlib["attribute"]       = option_list("attribute")
regexlib["preposition"]     = option_list("preposition")

# Math in Mogu syntax must be enclosed in parentheses.
#
# widget age
#   type    :   text
#   content :   (2013 - user dob year)
#   

regexlib["math_gen_expr"]        = "\(.*\)"

# OBJECT SET
#   An object set is a phrase which can resolve to an
#   object within the system, consisting of an object
#   type, an identifier, and an attribute of that object
#
#   There are some exceptions, for example the keyword 'own',
#   which acts upon the object using it, and does not require
#   an identifier.
# Examples of valid object sets:
#   data foo
#   data foo bar
#   widget foo css
#   user foo bar
#   group foo
#   group foo bar
#   own content
#regexlib["object_set"]  = "%(object)s\s+(%(identifier)s\s+)?(%(identifier)s(?<!%(preposition)s)|%(attribute)s)?" % regexlib

regexlib["object_set"]  = "%(object)s\s+(%(identifier)s\s+)?(%(attribute)s|%(identifier)s)?" % regexlib
regexlib["object_set"]  = "(%(object_set)s)+" % regexlib

regexlib["signed_obj"] = "-?\s*(%(object_set)s|[0-9\.]+)" % regexlib
regexlib["math_oper"] = "(\*|\+|\-|\/)"

regexlib["math_expr"] = "\(%(signed_obj)s\s*%(math_oper)s(%(signed_obj)s\s*%(math_oper)s\s*)*\s*%(signed_obj)s\)" % regexlib

# VALUE
#   A value in Mogu can consist of any object set, string literal, or integer literal.
#   The object set must resolve to something which can be turned into a string at runtime.
#
# Examples of valid values:
#   17
#   "a literal string!"
#   own content
#   data stringdata somestring
#   user name first
#
regexlib["value"]       = "(%(object_set)s|%(string_literal)s|%(math_expr)s|-?[0-9]+)" % regexlib 

HASH_DEFINITION = pyboro.Lexer.ParseMap((
        ("key",             everything_until(":")   ,   trim),
        ("delim",           ":",                        IGNORE),
        ("value",           everything_until(r"\n") ,   trim),
        ("end",             r"\n\s*"                ,   IGNORE)
))

LIST_DEFINITION = pyboro.Lexer.ParseMap([
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

WIDGET_TYPE = pyboro.Lexer.ParseMap((
        ("begin",   DIRECTIVE_START("type")     , IGNORE),
        ("type",    regexlib["widget_types"]    , syntax.as_integer),
        ("end",     DIRECTIVE_END               , IGNORE)
))

WIDGET_STYLE = pyboro.Lexer.ParseMap((
        ("begin",       DIRECTIVE_START("css|style")    , IGNORE),
        ("css_classes", regexlib['string_literal']      , trim),
        ("end",         DIRECTIVE_END                   , IGNORE)
))


WIDGET_CONTENT = pyboro.Lexer.ParseMap((
        ("begin",       DIRECTIVE_START("content|text")      , IGNORE),
        ("content",     "[^\n]*"                        , trim),
        ("end",         DIRECTIVE_END                   , IGNORE)
))

WIDGET_SOURCE = pyboro.Lexer.ParseMap((
        ("begin",       DIRECTIVE_START("source")       , IGNORE),
        ("source",      "[^\n]*"                        , trim),
        ("end",         DIRECTIVE_END                   , IGNORE)
))

WIDGET_LOCATION = pyboro.Lexer.ParseMap((
        ("begin",       DIRECTIVE_START("location")     , IGNORE),
        ("location",    "[^\n]*"                        , trim),
        ("end",         DIRECTIVE_END                   , IGNORE)
))

WIDGET_TEMPLATE = pyboro.Lexer.ParseMap((
        ("begin",       DIRECTIVE_START("template")     , IGNORE),
        ("template",    regexlib["identifier"]          , reference_template),
        ("end",         DIRECTIVE_END                   , IGNORE)
))

POLICY_MODE  = pyboro.Lexer.ParseMap((
        ("begin",       DIRECTIVE_START("mode")         , IGNORE),
        ("mode",        regexlib["identifier"]          , LITERAL),
        ("end",         DIRECTIVE_END                   , IGNORE)
))

POLICY_DATA = pyboro.Lexer.ParseMap((
        ("begin",           DIRECTIVE_START("data|type")         , IGNORE),
        ("datatype",        regexlib["datatype"]            , trim),
        ("end",             DIRECTIVE_END                   , IGNORE)
))

VALIDATOR_TYPE = pyboro.Lexer.ParseMap((
    ("begin",           DIRECTIVE_START("type")         , IGNORE),
    ("type",            regexlib["validator_type"]      , LITERAL),
    ("end",             DIRECTIVE_END                   , IGNORE)
))

VALIDATOR_TEST = pyboro.Lexer.ParseMap((
    ("begin",           DIRECTIVE_START("test")         , IGNORE),
    ("test",            regexlib["string_literal"]      , LITERAL),
    ("end",             DIRECTIVE_END                   , IGNORE)
))

NEWLINES = pyboro.Lexer.ParseMap([("newline","\n",IGNORE)])
