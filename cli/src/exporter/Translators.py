import syntax
from regexlib import regexlib
import re

class TranslatesIntegralSyntax(object):
    def syntax_to_string(self, value):
        def is_int(string):
            try:
                int(string)
                return True
            except:
                return False
        literal_queue = []
        literals = re.findall(regexlib["string_literal"],value)
        
        for literal in literals:
            while literal in value:
                value = value.replace(literal, " %s ")
                literal_queue.append(literal)
        tokens = value.split(" ")
        for token in tokens:
                if is_int(token):
                    while token in value:
                        value = value.replace(token,syntax.as_string(token))
        if (literal_queue):
            value = value % tuple(literal_queue)
        return value

class GenericTranslator(TranslatesIntegralSyntax):

    @classmethod
    def translate(self, token):
        temp = ExportList([])
        return temp.syntax_to_string(token)

class ExportList(TranslatesIntegralSyntax):
    def __init__(self, list, tabs=1):
        self.list = list
        self.tabs = tabs

    def __str__(self):
        output = []
        for entry in self.list:
            output.append("%s%s" % ("    "*self.tabs, self.syntax_to_string(entry)))
        return "\n".join(output)

class ExportDict(TranslatesIntegralSyntax):
    def __init__(self, dict,tabs=1):
        self.dict = dict
        self.tabs = tabs

    def __str__(self):
        output = []
        for entry in self.dict:
            tr_entry = self.syntax_to_string(entry)
            tr_value = self.syntax_to_string(self.dict[entry])
            output.append("%s%s\t: %s" % ( "    "*self.tabs, tr_entry, tr_value ) )
        return "\n".join(output)

if __name__ == "__main__":
    teststring = '22 1 css 56 "clicked_style"'
    f = ExportDict({})
    print f.syntax_to_string(teststring)
