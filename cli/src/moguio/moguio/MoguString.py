from multistring import MultiString
from lex_base import regexlib
import SymbolRegistry
import SharedData
import string
import syntax
import re
from sets import Set

class EscapeError(Exception):
    def __init__(self, needle, haystack):
        self.needle = needle
        self.haystack = haystack
    def __str__(self):
        return """
There is a problem with your escape sequences.

    %(needle)s

could not be found in:

    %(haystack)s
""" %(self.__dict__)

class UnescapeError(Exception):
    def __init__(self, needle, haystack):
        self.needle = needle
        self.haystack = haystack

    def __str__(self):
        return """
There is a problem with your escape sequences.

    %(needle)s should have been substituted, but was not in: 

    %(haystack)s
"""

class MoguString(MultiString):
    ReverseLookup = {}
    for keyword in syntax.MoguSyntax:
        value = syntax.MoguSyntax[keyword][0]
        ReverseLookup[str(value)] = keyword
    for op in syntax.MoguOperators:
        value = syntax.MoguOperators[op]
        ReverseLookup[str(value)] = op

    ReservedWords = syntax.MoguSyntax.keys()
    CompoundWords = [entry for entry in ReservedWords if len(entry.split(" ")) >1]
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

        super(MoguString, self).__init__(avail_contexts,initstr.replace("\t","    "))

        self.addTranslation('script','integral',self.script_to_integral)
        self.addTranslation('integral','script',self.integral_to_script)
        self.preferences = prefs if prefs is not None else {} 
        self.string_literals = {} 
        self.maths = []
        self.temp = None


    def script_to_integral(self, script):
        """
        Converts mogu script to integral commands.
        for instance, "set widget foo content to widget bar content" would
            be translated into something like '2 58 foo 6 26 58 bar 6'
        """
        
        # When given scripted syntax, the first thing we have to do 
        # is separate string literals, since they will not be converted.
        self.temp = self.separate_string_literals()
        
        # Next we must see if there are any math expressions in the string.
        # If so, we'll need to ensure they are spaced properly, since the 
        # lexer will not enforce spacing for mathematical expressions. 
        if self.has_maths(self.temp):
            for math in self.maths:
                expr = MoguString.assert_expression_spacing(math)
                self.temp = re.sub(re.escape(math),expr,self.temp)
        # Now we can replace each of Mogu's reserved words with its integral
        # counterpart

        # We start by replacing entries that have more than one word
        for entry in MoguString.CompoundWords:
            self.temp = re.sub(entry,MoguString.getIntegral(entry),self.temp)

        # and then move onto the single-word entries
        tokens = self.temp.split(" ")
        tokens = filter(MoguString.filter_empty,tokens) #Ignore double spaces

        for index,token in enumerate(tokens):
            if token in (MoguString.ReservedWords) or (token in MoguString.Operators):
                tokens[index] = MoguString.getIntegral(token)

        translated = " ".join(tokens)

        if self.string_literals:
            return translated % (self.string_literals)
        return translated

    def has_maths(self, string):
        """
            Searches the string for occurences of mathematical expressions.
            If it finds them, it adds them to the instance list self.maths.
            Returns True if expressions are found, False otherwise.

            Because format strings that are generated during the translation
            process resemble math operations, this also generates a list of 
            format strings "%(foo)s". This prevents false positives. If the
            intersection of the two is the same, then we don't actually have
            any mathematical operations.
        """
        self.maths = re.findall("\(.*\)",string)
        formats = re.findall("%\(.*\)s",string)

        if len(self.maths) == len(formats):
            return False

        if self.maths and self.formats:
            for entry in self.formats:
                format_id = entry[1:-1]
                if format_id in self.maths:
                    self.maths.remove(format_id)

        return len(self.maths) > 0

    def separate_string_literals(self):
        """
            Separates string literals from the active context, 
            replacing them with '%s' so they can be re-integrated.

            The literals themselves are stored in a 
            separate list (self.string_literals), and the temporary 
            string with the literals replaced is returned.
        """
        self.string_literals = {} 
        # Finds all string literals contained in the input string
        results = re.findall(regexlib['string_literal'],self.active())

        # Makes things nicer by removing empty tokens (' ', '')
        string_literals = Set(filter(MoguString.filter_empty,results))
        for literal in string_literals:
            literal_id = "l%d" % len(self.string_literals)
            self.string_literals[literal_id] = literal
        # Copy the current MoguString 
        self.temp = self.active()
        if r"%s" in self.temp:
            raise SyntaxError("The format string '%s' token is already present"
                    " in the input string. Either you have some weird text, or"
                    " something went very wrong somewhere. Exiting.")
        for entry in self.string_literals :
            literal = self.string_literals[entry]
            if literal not in self.temp:
                # We haven't done any substitutions yet, so this
                # is a check to make sure that there is not an issue with
                # the regex and escape sequences.
                raise EscapeError(literal,self.temp)
           
            # Remove all string literals and substitute the '%s' token
            # in their places. 
            self.temp = re.sub(
                    re.escape(literal), 
                    "%(" + "%s" % entry + ")s",
                    self.temp)

            # Ensure that the string literal was in fact substituted; 
            # a second regex escape sequence check.
            if literal in self.temp:
                raise UnescapeError(literal,self.temp)

        return self.temp

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
        filter(lambda s: s not in (""," "),tokens)

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
            return translated % (self.string_literals)
        else:
            return translated

    def getKeyword(self, integral):
        # Since we have to strip the token in order to ensure that
        # we get an actual integer, we want to preserve any newline
        # characters we come across
        newline = "\n" if \
                (isinstance(integral,str) and integral.endswith("\n")) else ""
        integral = integral.strip()
        if int(integral) == 0:
            return "0%s" % newline
        if int(integral) in self.preferences:
            return "%s%s" % (
                    self.preferences[int(integral)], newline)
        if str(integral) in MoguString.ReverseLookup:
            return "%s%s" % (
                    MoguString.ReverseLookup[integral],newline)
        raise SyntaxError(
                "%d is not recognized as Mogu integral syntax." % integral)

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
