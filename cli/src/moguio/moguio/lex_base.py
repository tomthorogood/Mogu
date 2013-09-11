import pyboro
import syntax
from RegexLib import regexlib
from lex_functions import everything_until
from lex_functions import trim
from lex_functions import directive_start
from lex_functions import add_references
from lex_functions import reference_widget_list
from lex_functions import temp_join


# Control of debugging vomit
# Can be changed in higher level modules
pyboro.Lexer.VERBAL = False

IGNORE = pyboro.Lexer.ParseMap.IGNORE #alias for ease of reading
LITERAL = pyboro.Lexer.ParseMap.LITERAL #alias for ease of reading

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
    ("begin",   directive_start(syntax.as_integer("type"))  , IGNORE),
    ("type",    regexlib["widget_types"]                    , LITERAL),
    ("end",     r"\S*"                                      , IGNORE)
))

WIDGET_SORT = pyboro.Lexer.ParseMap((
    ("begin",   directive_start(syntax.as_integer("sort"))  , IGNORE),
    ("declaration", r"[^\n]*"                               , trim),
    ("end",     r"\S*"                                      , IGNORE)
))

WIDGET_STYLE = pyboro.Lexer.ParseMap((
    ("begin",       directive_start(syntax.as_integer("css"))   , IGNORE),
    ("css_classes", regexlib['value']                           , add_references),
    ("end",         r"\S*"                                      , IGNORE)
))


WIDGET_CONTENT = pyboro.Lexer.ParseMap((
    ("begin",       directive_start(syntax.as_integer("text"))  , IGNORE),
    ("content",     r"[^\n]*"                                   , add_references),
    ("end",         r"\S*"                                      , IGNORE)
))

WIDGET_SOURCE = pyboro.Lexer.ParseMap((
    ("begin",       directive_start(syntax.as_integer('source')), IGNORE),
    ("source",      "[^\n]*"                                    , add_references),
    ("end",         r"\S*"                                      , IGNORE)
))

WIDGET_LOCATION = pyboro.Lexer.ParseMap((
    ("begin",       directive_start(syntax.as_integer("location"))  , IGNORE),
    ("location",    "[^\n]*"                                        , add_references),
    ("end",         r"\S*"                                          , IGNORE)
))

WIDGET_VALIDATOR = pyboro.Lexer.ParseMap((
    ("begin",       directive_start(syntax.as_integer('validator')), IGNORE),
    ("validator",    "[^\n]*" ,\
        lambda s: temp_join(
            add_references, " ", 1, "%d" % syntax.as_integer("validator"), s)),
    ("end",          r"\S*"                                          , IGNORE)
))

WIDGET_TEMPLATE = pyboro.Lexer.ParseMap((
    ("begin",       directive_start(syntax.as_integer("template"))  , IGNORE),
    ("template", regexlib["identifier"],\
        lambda s: temp_join(
            add_references, " ", 1, "%d" % syntax.as_integer("template"), s)),
    ("end",         r"\S*"                                          , IGNORE)
))

POLICY_ENCRYPTION = pyboro.Lexer.ParseMap((
    ("begin",       directive_start(syntax.as_integer("encrypted")), IGNORE),
    ("encrypted",   "(yes|no)"                                     , trim),
    ("end",         r"\S*"                                         , IGNORE)
))

POLICY_DEFAULT  =   pyboro.Lexer.ParseMap((
    ("begin",   directive_start(syntax.as_integer("default"))   , IGNORE),
    ("default", "[^\n]*"                                        , add_references),
    ("end",     r"\S*"                                          , IGNORE)
))

t = "%d|%d" %(syntax.as_integer("data"),syntax.as_integer("type"))
POLICY_DATA = pyboro.Lexer.ParseMap((
        ("begin",       directive_start(t)  , IGNORE),
        ("datatype",    regexlib["datatype"]                        , trim),
        ("end",         r"\S*"                                      , IGNORE)
))

VALIDATOR_TYPE = pyboro.Lexer.ParseMap((
    ("begin",           directive_start(syntax.as_integer("type"))  , IGNORE),
    ("type",            regexlib["validator_type"]                  , trim),
    ("end",             r"\S*"                                      , IGNORE)
))

VALIDATOR_TEST = pyboro.Lexer.ParseMap((
    ("begin",           directive_start(syntax.as_integer("test"))  , IGNORE),
    ("test",            regexlib["string_literal"]                  , add_references),
    ("end",             r"\S*"                                      , IGNORE)
))

NEWLINES = pyboro.Lexer.ParseMap([("newline",r"\n",IGNORE)])

t = "children"
CHILDREN_BLOCK = pyboro.Lexer.ParseMap((
    ("begin",           r"\s*%s\s*"%t                           , IGNORE),
    ("block",           everything_until(r"end\s*%s"%t)           , reference_widget_list),
    ("end",             r"end\s*%s"%t                             , IGNORE)
))
