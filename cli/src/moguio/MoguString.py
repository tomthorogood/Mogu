from multistring import MultiString
from lex_base import regexlib
import string
import syntax
import re

class MoguString(MultiString):
    Integrals = [val[0] for val in syntax.MoguSyntax.values()].extend(syntax.MoguOperators.values())
    ReservedWords = syntax.MoguSyntax.keys()
    Operators = syntax.MoguOperators.keys()

    @staticmethod
    def filter_empty(item):
        res = re.search("[^\s]+",item)
        return (res is not None) and len(item) > 0

    @staticmethod
    def assert_expression_spacing(expr):
        """
            This ensures that each token is appropriately delimited so that
            the integral values can be appropriately determined.

            (17+5) becomes ( 17 + 5 )
            (17 + user dob year) becomes ( 17 + user dob year )
        """
        repl = ""
        max_iter = len(expr)
        iter = 0 
        
        while iter < max_iter:
            char = expr[iter]
            if char in MoguString.Operators:
                if repl and ( repl[-1] != " "):
                    repl += " " 
                repl += char + " "
            else:
                repl += char
            iter+=1
        return repl
    
    @staticmethod
    def getIntegral(keyword):
        if keyword in MoguString.ReservedWords:
            return syntax.as_integer(keyword)
        elif keyword in MoguString.Operators:
            return syntax.MoguOperators[keyword]
        else:
            return 0

    class UnrecognizedContextException(Exception):
        def __init__(self, context):
            self.badcontext = context

        def __str__(self):
            return "%(badcontext)s is unrecognized. Mogu contexts must be either 'script' or 'integral'" % self.__dict__

    def __init__(self, context, initstr=None):
        """
        When created, MoguStrings must pass a default context. A string is optional.
        """
        avail_contexts = ("script","integral")
        if context not in avail_contexts:
            raise UnrecognizedContextException(context)
        if context != "script":
            avail_contexts = ("integral","script")

        super(MoguString, self).__init__(avail_contexts,initstr)

        self.addTranslation('script','integral',self.script_to_integral)
        self.addTranslation('integral','script',self.integral_to_script)
        self.string_literals = []
        self.maths = []


    def script_to_integral(self, script):
        """
        Converts mogu script to integral commands.
        for instance, "set widget foo content to widget bar content" would
            be translated into something like '2 58 foo 6 26 58 bar 6'
        """
        
        # When given scripted syntax, the first thing we have to do 
        # is separate string literals, since they will not be converted.
        temp = self.separate_string_literals()
        
        # Next we must see if there are any math expressions in the string.
        # If so, we'll need to ensure they are spaced properly, since the 
        # lexer will not enforce spacing for mathematical expressions. 
        if self.has_maths(temp):
            for math in self.maths:
                expr = MoguString.assert_expression_spacing(math)
                temp = re.sub(re.escape(math),expr,temp)
        # Now we can replace each of Mogu's reserved words with its integral
        # counterpart
        tokens = temp.split(" ")
        tokens = filter(MoguString.filter_empty,tokens) #Ignore double spaces

        for index,token in enumerate(tokens):
            if token in (MoguString.ReservedWords) or (token in MoguString.Operators):
                tokens[index] = str(MoguString.getIntegral(token))

        translated = " ".join(tokens)

        if self.string_literals:
            return translated % tuple(self.string_literals)
        return translated

    def has_maths(self, string):
        """
            Searches the string for occurences of mathematical expressions.
            If it finds them, it adds them to the instance list self.maths.
            Returns True if expressions are found, False otherwise.
        """
        self.maths = re.findall(regexlib['math_gen_expr'],string)
        return len(self.maths) > 0
        
    def separate_string_literals(self):
        """
            Separates string literals from the 'script' context, 
            replacing them with '%s' so they can be re-integrated.

            The literals themselves are stored in a 
            separate list (self.string_literals), and the temporary 
            string with the literals replaced is returned.
        """

        results = re.findall(regexlib['string_literal'],self.active())
        self.string_literals = filter(MoguString.filter_empty,results)
        temp = self.active()
        for result in results:
            temp = re.sub(result, '%s', temp)
        return temp


    def integral_to_script(integral):
        """
        Converts integral commands to human-readable MoguScript. 
        For instance, "2 58 foo 6 26 58 bar 6" would be translated to:
            'set widget foo content to widget bar content'
        However, this is not a class method because there can be contextual
        differences which may need to be set up, because some integral
        representations have multiple textual representations i
        (content|contents|text, for instance).
        
        If preferences don't exist, Mogu will use the "first' one it finds, 
        but understand that because this information is stored in a dict, this
        is not guaranteed to be any specific keyword.

        setting up the preferences dict is easy, and avoids this problem:
        {
            syntax.as_integer['content']    :   "content"
        } 
        
        will use the word "content" whenever this ambiguity is encountered.
        """
        pass


    def getKeyword(integral):
        if integral in MoguString.Integrals:
            try:
                return self.preferences[integral]
            except KeyError:
                for keyword in MoguString.ReservedWords:
                    i = syntax.as_integer(keyword)
                    if i == integral:
                        return keyword
        return "None"
