import pyRex 
import syntax 

from lex_base import IGNORE
from lex_base import regexlib
from lex_functions import trim


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

MOGU_CMD = pyRex.Lexer.ParseMap((
    ("begin"    ,   "\s*"                                                                           , IGNORE),
    ("action"   ,   "%(action)s" %regexlib                                                          , syntax.as_integer),
    ("ws"       ,   "\s+"                                                                           , IGNORE),
    ("dir_obj"  ,   "%(object_set)s" %regexlib                                                      , trim),
    ("opt_end"  ,   "(\s+%(preposition)s\s+%(value)s)?"%regexlib , trim),
    ("end"      ,   "\s*"                                                                           , IGNORE)
))
