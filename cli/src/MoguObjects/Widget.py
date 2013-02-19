#!/usr/bin/env python

from MoguObject import MoguObject


class Widget(MoguObject):
    def __init__(self, identifier, name=None, css=None, children=None, attributes = []):
        attrs = [
                "identifier",
                "name"
                ,"css"
                ]
        attrs.extend(attributes)
        
        super(Widget,self).__init__(attrs)

        # The widget's name in the database
        self.identifier = identifier

        # The widget's common name
        self.name = name

        # The widget's styling classes
        self.css = css


class ContainerWidget(Widget):
    def __init__(self, identifier, name=None, css=None, children=[]):
        super(ContainerWidget, self).__init__(identifier,name=name,css=css,attributes=["children",])
        self.children = children

class StackedContainerWidget(Widget):
    def __init__(self, identifier, name=None, css=None, children=[]):
        super(StackedContainerWidget,self).__init__(identifier,name=name,css=css,children=children)

