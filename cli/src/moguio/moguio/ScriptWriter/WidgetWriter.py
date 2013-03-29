import IndentTracker
from DictWriter import DictWriter
from ListWriter import ListWriter

class TriggerWriter(object):
    """
        The TriggerWriter requires a trigger and
        a list of commands, it will generate a When block
        with the following template:

            when %(trigger)s {
                %(command)s
                [%(command)s...]
            }
    """
    def __init__(self, trigger, cmdlist):
        self.trigger = trigger
        self.commands = cmdlist

    def __str__(self):
        IndentTracker.INDENT_LEVEL += 1
        self.tab = IndentTracker.TAB * IndentTracker.INDENT_LEVEL
        outer = \
"""
%(tab)swhen %(trigger)s {
%(inner)s
%(tab)s}
"""
        lw = ListWriter(self.commands)
        self.inner = str(lw)
        IndentTracker.INDENT_LEVEL -= 1
        return outer % self.__dict__

class ChildrenWriter(object):
    """
        A ChildrenWriter requires a list of child
        names, and may optionally take the parent's name
        as well. Passing in the parent's name will 
        allow the child's name to be truncated if possible
        for more readable shorthand.

        If widgetfoo:childalpha is passed in, and 'widgetfoo'
        is identified as thechildren= parent, only ':childalpha' will be
        written in the output.
    """
    @staticmethod
    def truncate_parent(child,parent):
        keypath = child.split(':')
        try:
            pIndex = keypath.index(parent)
            return ":%s" % ":".join(keypath[pIndex+1:])
        except ValueError:
            return child

    def __init__(self, children, parentname=None):
        self.children = children
        self.parent = parentname

        if self.parent:
            self.children = [
                    ChildrenWriter.truncate_parent(child,self.parent) \
                            for child in self.children]
   
    def __str__(self):
        IndentTracker.INDENT_LEVEL += 1
        self.tabs = IndentTracker.TAB * IndentTracker.INDENT_LEVEL
        outer = \
"""
%(tabs)schildren
%(inner)s
%(tabs)send children
"""
        listWriter = ListWriter(self.children)
        self.inner = str(listWriter)

        output = outer % self.__dict__
        IndentTracker.INDENT_LEVEL -= 1
        return output

class EventWriter(object):
    """
        An EventWriter requires a dictionary of 
        'trigger' : [command, ...] 
        entries, which it will turn into a standard
        events block.
    """

    def __init__(self, eventdict):
        self.eventdict = eventdict

    def __str__(self):
        IndentTracker.INDENT_LEVEL += 1
        self.tab = IndentTracker.TAB * IndentTracker.INDENT_LEVEL
        outer = \
"""
%(tab)sevents
%(inner)s
%(tab)send events
"""
        self.inner = "" 
        for trigger in self.eventdict:
            tw = TriggerWriter(trigger,self.eventdict[trigger])
            self.inner += str(tw)
        IndentTracker.INDENT_LEVEL -= 1

        return outer % self.__dict__

class AbstractWidgetWriter(object):
    """
    The AbstractWidgetWriter creates widget-type elements.
        Requires a dictionary object with the following entries:
            'type'          :   'widget|template',
            'identifier'    :   'validIdentifier:name'
            'attributes'    :   { 'attr' : 'value', [...] },
            'events'        :   { 'trigger' : [command, ...], ...}
            'children'      :   [child1, ...]
        'events' and 'children' are optional, of course.
    """
    def __init__(self, widgetdict):
        self.dict = widgetdict

    def __str__(self):

        outer = \
"""
%(tab)s%(type)s %(identifier)s
%(inner)s
%(tab)send %(type)s
"""
        self.dict['tab'] = IndentTracker.TAB * IndentTracker.INDENT_LEVEL
        self.dict['inner'] = ""

        attrWriter = DictWriter(self.dict['attributes'])
        self.dict['inner'] += str(attrWriter)

        if 'events' in self.dict and self.dict['events'] is not None:
            eventWriter = EventWriter(self.dict['events'])
            self.dict['inner'] += str(eventWriter)
        if 'children' in self.dict and self.dict['children'] is not None:
            childrenWriter = ChildrenWriter(self.dict['children'], self.dict['identifier'])
            self.dict['inner'] += str(childrenWriter)
        return outer % self.dict

    def addEvent(self, trigger,command):
        if 'events' not in self.dict or self.dict['events'] is None:
            self.dict['events'] = {}
        if trigger not in self.dict['events']:
            self.dict['events'][trigger] = []
        self.dict['events'][trigger].append(command)

    def addChild(self, childname):
        if 'children' not in self.dict or self.dict['children'] is None:
            self.dict['children'] = []
        self.dict['children'].append(childname)

class WidgetWriter(AbstractWidgetWriter):
    def __init__(self,identifier,attributes,events=None,children=None):
        dct = {
                'type'      :   'widget',
                'identifier':   identifier,
                'attributes':   attributes,
                'events'    :   events,
                'children'  :   children
        }
        super(WidgetWriter, self).__init__(dct)

class TemplateWriter(AbstractWidgetWriter):
    def __init__(self,identifier,attributes,events=None,children=None):
        dct = {
                'type'      :   'template',
                'identifier':   identifier,
                'attributes':   attributes,
                'events'    :   events,
                'children'  :   children
        }
        super(TemplateWriter,self).__init__(dct)

if __name__ == "__main__":
    attributes = {
            'type'  :   'container',
            'style' :   'some style',
            'tooltip':  '<div class="tooltip">Here is some helpful text</div>'
            }
    events = {
            'clicked'   :   ['a clicky thing','another clicky thing'],
            'mouseover' :   ['a mousy thing', 'another mousy thing'],
            'loaded'    :   ['a post-load callback']
            }
    children = ['widget:childA','widget:childB','widget:childC']

    ww = WidgetWriter('widget',attributes,events,children)
    tw = TemplateWriter('tpl',attributes,events,children)
    print(ww)
    print(tw)
