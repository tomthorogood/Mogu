from multistring import MultiString
from lex_base import regexlib
import SymbolRegistry
import SharedData
import string
import syntax
import re

class MoguString(MultiString):
    ReverseLookup = {}
    for keyword in syntax.MoguSyntax:
        value = syntax.MoguSyntax[keyword][0]
        ReverseLookup[str(value)] = keyword
    for op in syntax.MoguOperators:
        value = syntax.MoguOperators[op]
        ReverseLookup[str(value)] = op

    ReservedWords = syntax.MoguSyntax.keys()
    Operators = syntax.MoguOperators.keys()

    ReferencedTypes = [
            syntax.as_integer('widget'),
            syntax.as_integer('data'),
            syntax.as_integer('user'),
            syntax.as_integer('group')
    ]

    ReferenceTables = [
            SymbolRegistry.widgetRegistry,
            SymbolRegistry.dataRegistry,
            SymbolRegistry.policyRegistry,
            SymbolRegistry.policyRegistry
    ]

    @staticmethod
    def reference(integral, symbol):
        integral = int(integral)
        for index,repr_type in enumerate(MoguString.ReferencedTypes):
            if integral == repr_type:
                MoguString.ReferenceTables[index][symbol].append(SharedData.ActiveFile)

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
            return str(syntax.as_integer(keyword))
        elif keyword in MoguString.Operators:
            return str(syntax.MoguOperators[keyword])
        else:
            return 0

    @staticmethod
    def is_integer(token):
        try:
            int(token)
            return True
        except:
            return False

    class UnrecognizedContextException(Exception):
        def __init__(self, context):
            self.badcontext = context

        def __str__(self):
            return "%(badcontext)s is unrecognized. Mogu contexts must be either 'script' or 'integral'" % self.__dict__

    class AlgorithmException(Exception):
        def __init__(self,state):
            self.state = state
        def __str__(self):
            return """
            An error was encountered keeping this program from continuing. This is not your fault, and should be
            reported to the Mogu team.

            Please send the following information:
            %(state)s
            """ % self.__dict__

    def __init__(self, context, initstr=None, prefs=None):
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
        self.preferences = prefs if prefs is not None else {} 
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
                tokens[index] = MoguString.getIntegral(token)

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
            Separates string literals from the active context, 
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

    def integral_to_script(self, original_integral):
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
        
        # First, we must separate the string literals out of the equation, 
        # since they will not be changed in any way.
        integral = self.separate_string_literals()

        # We can then safely explode the integral string, since
        # they are guaranteed by the script -> integral conversion
        # to be space-delimited, so long a there are no string literals.
        tokens = integral.split(' ')

        # In case there are mathematical expressions, we must first
        # resolve any syntax that may be confused with numbers. This
        # is done by reversing the string, finding tokens that are not
        # integers, and then finding the next integral token, which must
        # necessarily be a syntactical integral.
        tokens.reverse()

        searching = False
        for index,token in enumerate(tokens):
            if not MoguString.is_integer(token):
                searching = True
            elif searching is True:
                tokens[index] = self.getKeyword(token)
                searching = False

        tokens.reverse()    # Put the tokens back in ltr order.

        # Next we need to do a search and replace for Mathematical 
        # operators, since they will not be conflicting with any
        # integer literals.
        op_integrals = [self.getIntegral(op) for op in MoguString.Operators]
        for index,token in enumerate(tokens):
            if token in op_integrals:
                tokens[index] = self.getKeyword(token)

        # At this point, any mathematical expressions have been fully realised.
        # We can then replace any syntactical integers outside of parentheses.

        paren_level = 0
        for index,token in enumerate(tokens):
            if token == '(':
                paren_level += 1
            elif token == ')':
                paren_level -= 1
            elif MoguString.is_integer(token):
                if paren_level == 0:
                    tokens[index] = self.getKeyword(token)

        translated = " ".join(tokens)

        if self.string_literals:
            return translated % tuple(self.string_literals)
        else:
            return translated

    def getKeyword(self, integral):
        if int(integral) in self.preferences:
            return self.preferences[int(integral)]
        if str(integral) in MoguString.ReverseLookup:
            return MoguString.ReverseLookup[integral]
        return "None"

    def create_references(self):
        if self.context == 'integral': 
            return #integral context cannot create references

        # Before we can create references, we must
        # ensure that our tokens are properly spaced. This
        # can be done by translating into integral and back again.
        protected = MoguString('script', self.active())
        protected.translate('integral')
        protected.active('integral')
        protected.translate('script', MoguString.OVERWRITE_STORED_VALUE)

        
        splitsafe = protected.separate_string_literals()
        
        # We have now ensured spacing and removed string literals.
        tokens = splitsafe.split(' ')
        for index,token in enumerate(tokens):
            if (not MoguString.is_integer(token) and \
                    MoguString.is_integer(tokens[index-1])) \
                    and index != 0:
                if (int(tokens[index-1]) in MoguString.ReferencedTypes):
                    MoguString.reference(tokens[index-1],token)

def importString(script):
    if not script: return script 
    mogustring = MoguString('script',script)
    integral = mogustring.translate('integral')
    mogustring.create_references()
    return integral
