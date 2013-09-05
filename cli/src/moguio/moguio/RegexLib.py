import syntax

def valid_options (string):
    """
        Returns valid integral representations of keywords.
    """
    return [str(syntax.MoguSyntax[key][0]) \
            for key in syntax.MoguSyntax if string in syntax.MoguSyntax[key][1]]


def option_list(grammar_type):
    return "(%s)" % "|".join(valid_options(grammar_type))

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
    "comment"       :   "^#.*\n",
    "location"      :   str(syntax.as_integer("location")),
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

regexlib["value"] = "((%(hash)s )?%(object_set)s|%(string_literal)s|%(math_gen_expr)s|-?[0-9]+)" % regexlib 
