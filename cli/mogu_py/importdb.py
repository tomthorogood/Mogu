class ImportPackage(object):
    def __init__(self, widgets, tree, events, perspectives, global_events, meta, policies, sessions):
        self.widgets = widgets
        self.tree = tree
        self.events = events
        self.meta = meta
        self.global_events = global_events
        self.perspectives = perspectives
        self.policies = policies
        self.sessions = sessions

def evaluate_file(filename):
    widgets={}
    tree = {}
    events = {}
    global_events = {}
    meta = {}
    perspectives = {}
    policies = {}
    sessions = {}
    
    f = open(filename,'r')
    execfile(filename)

    return ImportPackage(widgets, tree, events, perspectives, global_events, meta, policies, sessions)


