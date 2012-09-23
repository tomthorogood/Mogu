from snippets import confirm
from db_type import *
from coloring import warn
from syntax import *
from sets import Set
class ImportPackage(object):
    def __init__(self, widgets, tree, events, perspectives, global_events, meta, policies, sessions, validators, data):
        self.widgets = widgets
        self.tree = tree
        self.events = events
        self.meta = meta
        self.global_events = global_events
        self.perspectives = perspectives
        self.policies = policies
        self.sessions = sessions
        self.validators = validators
        self.data = data

#Creates a 
def evaluate_files(filenames):
    packages = []
    files = []
    widgets={}
    tree = {}
    events = {}
    global_events = {}
    meta = {}
    perspectives = {}
    policies = {}
    sessions = {}
    validators = {}
    data = {}

    for filename in filenames:
        
        f = open(filename,'r')
        execfile(filename)
        f.close()
    
    packages.append(ImportPackage(widgets, tree, events, perspectives, global_events, meta, policies, sessions,validators, data))
    return packages

"""
imports a list of files into the redis database.
@db - The connection to the redis database
@args - the argumentParser object used for processing command line arguments
@pyFiles - a list of python files to be evaluated
@moguFiles - a list of mogu scripts to be imported
"""
def import_files(db, args, moguFiles, pyFiles):
    packages = evaluate_files(pyFiles+moguFiles)
    
    for package in packages:
        dflags = 0
        lflags = 0
        sflags = 0

        if args.testing:
            dflags |= bytemaps.DictStorage.is_test
            lflags |= bytemaps.ListStorage.is_test
            sflags |= bytemaps.StrStorage.is_test
        if args.merge:
            dflags |= bytemaps.DictStorage.is_merge
            lflags |= bytemaps.ListStorage.is_merge
        if args.yes:
            dflags |= bytemaps.DictStorage.assume_yes
            lflags |= bytemaps.ListStorage.assume_yes
            sflags |= bytemaps.StrStorage.assume_yes
        if args.verbose:
            dflags |= bytemaps.DictStorage.verbose
            lflags |= bytemaps.ListStorage.verbose
            sflags |= bytemaps.StrStorage.verbose
        
        for widget in package.widgets:
            w = Widget()
            w.build(widget)
            w._import(db, package.widgets[widget], dflags)

        for widget in package.events:
            wevents =0
            if not args.merge:
                # Get rid of all current event nodes to avoid duplications
                current_events = db.keys("*%s.events*" % widget)
                for e in current_events:
                    db.delete(e)
            for event in package.events[widget]:
                wevents += 1
                e =  WidgetEvent()
                e.build(widget, wevents)
                e._import(db, event, dflags)

        for event in package.global_events:
            e = GlobalEvent()
            e.build(event)
            e._import(db, package.global_events[event], dflags)

        for view in package.perspectives:
            pmold = 0
            for mold in package.perspectives[view]:
                pmold += 1
                p = PerspectiveMold()
                p.build(view, pmold)
                p._import(db, mold, dflags)

        for policy in package.policies:
            p = WidgetPolicy()
            p.build(policy)
            p._import(db, package.policies[policy], dflags)

        for widget in package.tree:
            w = WidgetChildren()
            w.build(widget)
            w._import(db, package.tree[widget], lflags)

        for dnode in package.data:
            n = DataNode()
            n.build(dnode)
            dnflags = None
            data = package.data[dnode]
            if (isinstance(data,str)):
                dnflags = sflags
            elif (isinstance(data,list)):
                dnflags = lflags
            elif (isinstance(data,dict)):
                dnflags = dflags
            n._import(db, data, dnflags)

        for session in package.sessions:
            for storage in package.sessions[session]:
                entry = package.sessions[session][storage]
                if isinstance(entry, dict):
                    s = SessionDict()
                    fl = dflags
                elif isinstance(entry, list):
                    s = SessionList()
                    fl = lflags
                elif isinstance(entry, Set):
                    s = SessionSet()
                    fl = lflags
                else:
                    s = SessionStr()
                    fl = sflags
                s.build(session,storage)
                s._import(db, entry, fl)

        for validator in package.validators:
            v = Validator()
            v.build(validator)
            v._import(db, package.validators[validator], dflags)
