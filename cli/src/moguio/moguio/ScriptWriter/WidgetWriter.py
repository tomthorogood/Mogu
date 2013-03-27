import IndentTracker
from DictWriter import DictWriter
from ListWriter import ListWriter
from ChildrenWriter import ChildrenWriter
from EventWriter import EventWriter

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
%(tab)s end %(type)s
"""
        self.dict['tab'] = IndentTracker.TAB * IndentTracker.INDENT_LEVEL
        self.dict['inner'] = ""

        attrWriter = DictWriter(self.dict['attributes'])
        self.dict['inner'] += str(attrWriter)

        if 'events' in self.dict and self.dict['events'] is not None:
            eventWriter = EventWriter(self.dict['events'])
            self.dict['inner'] += str(eventWriter)
        if 'children' in self.dict and self.dict['events'] is not None:
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
