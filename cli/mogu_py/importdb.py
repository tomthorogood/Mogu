class ImportPackage(object):
    def __init__(self, widgets, tree, events, global_events):
        self.widgets = widgets
        self.tree = tree
        self.events = events
        self.global_events = global_events

def evaluate_file(filename):
    widgets={}
    tree = {}
    events = {}
    global_events = {}
    meta = {}
    
    f = open(filename,'r')
    execfile(filename)

    return ImportPackage(widgets, tree, events, global_events)


