import re
import syntax
import lex_base

def MalformedExpressionError(Exception):
    def __init__(self, expression, expecting):
        self.value = "MalformedExpression \"%s\", expecting %s" %(expression,expecting)
    def __str__(self):
        return self.value

    def __repr__(self):
        return self.value

def strip_string_literals(inputstr):
    strlits = re.findall(lex_base.regexlib['string_literal'],inputstr)
    for strlit in strlits:
        while strlit in inputstr:
            inputstr = inputstr.replace(strlit,"__STRLIT__")
    return inputstr

def validate_math(inputstr):
    if inputstr.count("(") != inputstr.count(")"):
        raise MalformedExpressionError(inputstr, "matching parentheses")
    orig = inputstr
    while "(" in inputstr:
        inputstr = inputstr.replace("(","")
    while ")" in inputstr:
        inputstr = inputstr.replace(")","")
    if ("__STRLIT__" in inputstr):
        raise MalformedExpressionError(orig, "no string literals")
    
    res = re.search(lex_base.regexlib['math_expr'], inputstr)
    if res.group() != inputstr:
        raise MalformedExpressionError(inputstr, "mathematical syntax")
    return True



def check_syntax(inputstr):
    orig = inputstr
    inputstr = strip_string_literals(inputstr)
    math_expr = re.findall(lex_base.regexlib['math_gen_expr'],inputstr)
    if not math_expr:
        return True
    
