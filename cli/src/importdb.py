from snippets import confirm
from db_type import *
from coloring import warn
from syntax import *
from sets import Set


def _import (args, db):
    pyFiles = []
    moguFiles = []

    for filename in args.command[1:]:
        if filename.endswith(".py"):
            pyFiles.append(filename)
        elif filename.endswith(".mogu"):
            moguFiles.append(filename)
    importdb.import_files(db, args, moguFiles, pyFiles)


def import_directory(args, db):
    if args.flushdb:
        message = warn(
                "WARNING: This will overwrite all data in datbase number %d  of your Redis instance at %s:%d" % (
                    args.redis_db, args.redis_host, args.redis_port))
        if confirm(message,args.yes):
            db.flushdb()

        else:            
            print("No problem. If you're nervous, you can always remove the --redis-flush argument from your command")
            sys.exit()

    for directory in args.command[1:]:
        dir_dict = {}
        for root, subs, files in os.walk(directory):
            dir_dict[root] = ([],[])
            for filename in files:
                fname = os.path.join(root,filename)
                if filename.endswith(".py"):
                    dir_dict[root][0].append(fname)
                elif filename.endswith(".mogu"):
                    dir_dict[root][1].append(fname)
        for d in dir_dict:
            importdb.import_files(db, args, dir_dict[d][1], dir_dict[d][0])


def m_import (args, db):
    if not args.filename:
        message = "You have told me that you want to import a file, but have not given me\
                a file to import."
        raise MissingArgumentError("--filename", message)

    if args.flushdb:
        message = warn(
                "WARNING: This will overwite ALL data in database %d \
                        of the Redis instance at %s:%d" % (
                    args.redis_db, args.redis_host, args.redis_port))
        if not confirm(message, args.yes):
            print( ok("No changes were made."))
            sys.exit()
    
    package = importdb.evaluate_file(args.filename)
    
    add_widget_set (db, package, args.testing, args.flushdb, args.merge)
    add_global_events (db, package.global_events, arge.testing, args.merge)
    add_meta_values (db, package.meta, args.testing, args.merge)
    add_perspectives(db, package.perspectives, args.testing, args.merge)
    add_policies(db, package.policies, args.testing)



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
