#!/usr/bin/env python
from snippets import *

class NodeNameParams(object):
    def __init__(self, begin=None, end=None):
        self.begin = begin
        self.end = end

class NodeName(object):
    def __init__(self, string, params):
        self.name = string
        self.node = string

        if params.begin:
            self.name = removebeginning(string, params.begin)
            self.node = assertstart(string, params.begin)

        if params.end:
            self.name = removeending(string, params.end)
            self.node = assertending(string, params.end)

class NodePatterns:
    class Widget(NodeName):
        def __init__(self, string):
            super(NodePatterns.Widget, self).__init__(
                    string,NodeNameParams(begin="widgets."))

    class Template(NodeName):
        def __init__(self, string):
            super(NodePatterns.Template, self).__init__(
                    string, NodeNameParams(begin="templates."))

    class WidgetPolicy(NodeName):
        def __init__(self, string):
            super(NodePatterns.WidgetPolicy, self).__init__(
                    string, NodeNameParams(
                        begin="widgets.",
                        end=".policy"))
    class Validator(NodeName):
        def __init__(self,string):
            super(NodePatterns.Validator, self).__init__(
                    string, NodeNameParams(begin="validators."))

    class Perspective(NodeName):
        def __init__(self, string):
            super(NodePatterns.Perspective, self).__init__(
                    string, NodeNameParams(begin="perspectives."))

    class Data(NodeName):
        def __init__(self, string):
            super(NodePatterns.Data, self).__init__(
                    string, NodeNameParams(begin="data."))

    class Session(NodeName):
        def __init__(self, string):
            super(NodePatterns.Session, self).__init__(
                    string, NodeNameParams(begin="s."))


