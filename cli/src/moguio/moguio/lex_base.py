import re
import pyboro
from lex_functions import *
import syntax

# Control of debugging vomit
# Can be changed in higher level modules
pyboro.Lexer.VERBAL = False

IGNORE = pyboro.Lexer.ParseMap.IGNORE #alias for ease of reading
LITERAL = pyboro.Lexer.ParseMap.LITERAL #alias for ease of reading


def option_list(grammar_type):
    return "(%s)" % "|".join(valid_options(grammar_type))

#STRLIT = r'''(?x)   # verbose mode
#    (?<!\\)    # not preceded by a backslash
#    "          # a literal double-quote
#    .*?        # non-greedy 1-or-more characters
#    (?<!\\)    # not preceded by a backslash
#    "          # a literal double-quote
#    ''' 

regexlib = {
    "identifier"    :   "[a-zA-Z:_][a-zA-Z:_0-9]*",
    "string_literal":   r'''(?<!\\)".*?(?<!\\)"''',
    "math_operator" :   r'''(\(|\)|\+|\-|\*|\/)''',
    "math_begin"    :   r'''(\(|\-)''',
    "math_end"      :   r'''(\))''',
    "reserved"      :   "(%s)" % "|".join(syntax.MoguSyntax.keys()),
    "event_triggers":   option_list("event trigger"),
    "widget_types"  :   option_list("widget type"),
    "datatype"      :   option_list("datatype"),
    "validator_type":   option_list("validator type"),
    "action"        :   option_list("action"),
    "object"        :   option_list("object"),
    "attribute"     :   option_list("attribute"),
    "preposition"   :   option_list("preposition"),
    "comment"       :   r"#.*\n",
    "math_gen_expr" :   r"%d.*%d" % (syntax.MoguOperators["("],syntax.MoguOperators[")"]),
#    "math_gen_expr" :   r"\(.*\)",
    "comment"       :   "^#.*\n"
}

regexlib["math_oper"] = "(%(*)d|%(+)d|%(-)d|%(/)d)" % syntax.MoguOperators
regexlib["op_paren"] = str(syntax.MoguOperators["("])
regexlib["cl_paren"] = str(syntax.MoguOperators[")"])

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

regexlib["object_set"]  = "%(object)s(\s+%(identifier)s)?(\s+(%(attribute)s|%(identifier)s))?" % regexlib
regexlib["object_set"]  = "(%(object_set)s\s*)+" % regexlib
regexlib["signed_obj"] = "-?\s*(%(object_set)s|[0-9\.]+)" % regexlib
regexlib["math_expr"] = "%(op_paren)s%(signed_obj)s\s*%(math_oper)s(%(signed_obj)s\s*%(math_oper)s\s*)*\s*%(signed_obj)s%(cl_paren)s" % regexlib
regexlib["hash"] = syntax.as_integer("hash")

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
regexlib["value"]       = "((%(hash)s )?%(object_set)s|%(string_literal)s|%(math_gen_expr)s|-?[0-9]+)" % regexlib 

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
    ("begin",   DIRECTIVE_START(syntax.as_integer("type"))  , IGNORE),
    ("type",    regexlib["widget_types"]                    , LITERAL),
    ("end",     r"\S*"                                      , IGNORE)
))

WIDGET_STYLE = pyboro.Lexer.ParseMap((
    ("begin",       DIRECTIVE_START(syntax.as_integer("css"))   , IGNORE),
    ("css_classes", regexlib['value']                           , trim),
    ("end",         r"\S*"                                      , IGNORE)
))


WIDGET_CONTENT = pyboro.Lexer.ParseMap((
    ("begin",       DIRECTIVE_START(syntax.as_integer("text"))  , IGNORE),
    ("content",     r"[^\n]*"                                   , trim),
    ("end",         r"\S*"                                      , IGNORE)
))

WIDGET_SOURCE = pyboro.Lexer.ParseMap((
    ("begin",       DIRECTIVE_START(syntax.as_integer('source')), IGNORE),
    ("source",      "[^\n]*"                                    , trim),
    ("end",         r"\S*"                                      , IGNORE)
))

WIDGET_LOCATION = pyboro.Lexer.ParseMap((
    ("begin",       DIRECTIVE_START(syntax.as_integer("location"))  , IGNORE),
    ("location",    "[^\n]*"                                        , trim),
    ("end",         r"\S*"                                          , IGNORE)
))

WIDGET_VALIDATOR = pyboro.Lexer.ParseMap((
    ("begin",       DIRECTIVE_START(syntax.as_integer('validator')), IGNORE),
    ("validator",    "[^\n]*"                                        , trim),
    ("end",          r"\S*"                                          , IGNORE)
))

WIDGET_TEMPLATE = pyboro.Lexer.ParseMap((
    ("begin",       DIRECTIVE_START(syntax.as_integer("template"))  , IGNORE),
    ("template",    regexlib["identifier"]                          , reference_template),
    ("end",         r"\S*"                                          , IGNORE)
))

WIDGET_TOOLTIP = pyboro.Lexer.ParseMap((
    ("begin",       DIRECTIVE_START(syntax.as_integer("tooltip"))   , IGNORE),
    ("tooltip",     "[^\n]*"                                        , trim),
    ("end",         r"\S*"                                          , IGNORE)
))

POLICY_MODE  = pyboro.Lexer.ParseMap((
    ("begin",       DIRECTIVE_START(syntax.as_integer("mode"))  , IGNORE),
    ("mode",        "[0-9]+"                                    , LITERAL),
    ("end",         r"\S*"                                      , IGNORE)
))

POLICY_ENCRYPTION = pyboro.Lexer.ParseMap((
    ("begin",       DIRECTIVE_START(syntax.as_integer("encrypted")), IGNORE),
    ("encrypted",   "(yes|no)"                                     , trim),
    ("end",         r"\S*"                                         , IGNORE)
))

POLICY_DEFAULT  =   pyboro.Lexer.ParseMap((
    ("begin",   DIRECTIVE_START(syntax.as_integer("default"))   , IGNORE),
    ("default", "[^\n]*"                                        , trim),
    ("end",     r"\S*"                                          , IGNORE)
))

# Data or type:
t = "%d|%d" %(syntax.as_integer("data"),syntax.as_integer("type"))
POLICY_DATA = pyboro.Lexer.ParseMap((
        ("begin",       DIRECTIVE_START(t)  , IGNORE),
        ("datatype",    regexlib["datatype"]                        , trim),
        ("end",         r"\S*"                                      , IGNORE)
))

VALIDATOR_TYPE = pyboro.Lexer.ParseMap((
    ("begin",           DIRECTIVE_START(syntax.as_integer("type"))  , IGNORE),
    ("type",            regexlib["validator_type"]                  , LITERAL),
    ("end",             r"\S*"                                      , IGNORE)
))

VALIDATOR_TEST = pyboro.Lexer.ParseMap((
    ("begin",           DIRECTIVE_START(syntax.as_integer("test"))  , IGNORE),
    ("test",            regexlib["string_literal"]                  , LITERAL),
    ("end",             r"\S*"                                      , IGNORE)
))

NEWLINES = pyboro.Lexer.ParseMap([("newline",r"\n",IGNORE)])

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


t = "children"
CHILDREN_BLOCK = pyboro.Lexer.ParseMap((
    ("begin",           r"\s*%s\s*"%t                           , IGNORE),
    ("block",           everything_until(r"end\s*%s"%t)           , reference_widget_list),
    ("end",             r"end\s*%s"%t                             , IGNORE)
))
