from sets import Set
from RegexLib import regexlib
import syntax
from Loggable import Loggable
from Loggable import LogMessage
import re
import sys

class ReferenceFinder(Loggable):
    """Given a string, attempts to deduce references to Mogu objects."""


    referencable_objects = (
        syntax.as_integer("widget"),
        syntax.as_integer("validator"),
        syntax.as_integer("data"),
        syntax.as_integer("template"),
        syntax.as_integer("user"),
        syntax.as_integer("group")
    )

    def __init__(self, string, verbose=0):
        super(ReferenceFinder,self).__init__(verbose)
        self.refs = {}
        self.parse_string(string)

    def remove_string_literals(self, string):
        found = re.findall(regexlib["string_literal"], string)
        for f in found:
            string = string.replace(f, "")
        return string

    def get_object_indexes(self, tokens):
        indexes = []
        for i,t in enumerate(tokens):
            m = re.match(regexlib["object"],t)
            if m:
                indexes.append(i)
        return indexes

    def is_identifier(self, string):
        m = re.match(regexlib["identifier"],string)
        return (m.group()==string) if m else False

    def add_reference(self, obj, identifier):
        if not self.is_identifier(identifier):
            return
        self.log(
            LogMessage("Found reference to symbol %s" % identifier,5),self.OUT)
        # Policies are never referred to directly, but inferred from field names
        # for user/group fields.
        if obj in (syntax.as_integer("user"),syntax.as_integer("group")):
            obj = "policy"
        self.log(
            LogMessage("Symbol is of type %s",str(obj)),self.OUT)
        if obj not in self.refs:
            self.refs[obj] = Set() 
        self.refs[obj].add(identifier)

    def find_references(self, indexes, tokens):
        for index in indexes:
            token = int(tokens[index])
            if token in ReferenceFinder.referencable_objects:
                try:
                    self.add_reference(token,tokens[index+1])
                except IndexError:
                    if token not in (
                            syntax.as_integer("group")
                            ,syntax.as_integer("user")):
                        sys.stderr.write(
                            "WARNING: %s not followed by identifier\n" %\
                                    syntax.as_string(token))
    def parse_string(self, string):
        string = self.remove_string_literals(string)
        tokens = string.split(" ")
        tokens = filter(lambda s: len(s)>0, tokens)
        obj_indexes = self.get_object_indexes(tokens)
        self.find_references(obj_indexes, tokens)
