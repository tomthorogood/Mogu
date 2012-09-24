from sets import Set
import sys
import subprocess
import bytemaps
import mimport as todb
from snippets import confirm
from snippets import clrln
from exceptions import *
import cityhash

required_event_parameters = {
        "198"       :       ("action","message","nextAction","degradation"),
        "199"       :       ("action","message","nextAction","degradation","trigger"),
        "202"       :       ("action","message","degradation"),
        "203"       :       ("action","message","degradation","trigger"),
        "208"       :       ("action","message","listeners"),
        "209"       :       ("action","message","listeners","trigger"),
        "214"       :       ("action","message","listeners","nextAction","degradation"),
        "215"       :       ("action","message","listeners","nextAction","degradation","trigger"),
        "218"       :       ("action","message","listeners","degradation"),
        "219"       :       ("action","message","listeners","degradation","trigger"),
        "225"       :       ("action","message","listeners","trigger"),
        "233"       :       ("action","message","listeners","trigger")
        }

class Node(object):
    # STATIC ITEMS
    class Wrappers:
        CURLY_BRACES = ('{','}')
        SQUARE_BRACES = ('[',']')
        PARENTHESES = ('(',')')
        ASTERISKS = ('*','*')
        AT_SYMBOLS = ('@','@')
        PERCENT_SYMBOLS = ('%','%')
        CARATS = ('^','^')
        TILDES = ('~','~')
        BACKTICKS = ('`','`')
        NONE = None

        lookup = {
                '{' :   CURLY_BRACES,
                '}' :   CURLY_BRACES,
                '[' :   SQUARE_BRACES,
                ']' :   SQUARE_BRACES,
                '(' :   PARENTHESES,
                ')' :   PARENTHESES,
                '*' :   ASTERISKS,
                '@' :   AT_SYMBOLS,
                '%' :   PERCENT_SYMBOLS,
                '^' :   CARATS,
                '~' :   TILDES,
                '`' :   BACKTICKS
                }

        required_wraps = {
            "type"      :   (CURLY_BRACES,),
            "action"    :   (CURLY_BRACES,),
            "trigger"   :   (CURLY_BRACES,),
            "nextAction":   (CURLY_BRACES,),
            "mode": (CURLY_BRACES,),
            "data_type" :   (CURLY_BRACES,),
            "storage_type" : (CURLY_BRACES,),
            "signal"    :   (CARATS,),
            "degradation":  (CARATS,)
        }

    def unwrap(self,val):
        if val[0] in Node.Wrappers.lookup and val[-1] in Node.Wrappers.lookup:
            if Node.Wrappers.lookup[val[0]] is Node.Wrappers.lookup[val[-1]]:
                wrappers = Node.Wrappers.lookup[val[0]]
                if val[0] == wrappers[0] and val[-1] == wrappers[1]:
                    return val[1:-1]
        return val

    def wrap(self, key, val):
        rerr = False
        if key in Node.Wrappers.required_wraps:
            wrap_styles = Node.Wrappers.required_wraps[key]
            for style in wrap_styles:
                if val[0] == style[0] and val[-1] == style[1]:
                    return val
                elif val[0] == style[0] and not val[-1] == style[1]:
                    print coloring.warn("Fixing %s: %s" % (self.node, val))
                    return val+style[1]
                elif not val[0] == style[0] and val[-1] == style[1]:
                    return style[0]+val
                elif len(wrap_styles) is 1:
                    return style[0]+val+style[1]
                else:
                    rerr = True
        else:
            return val
        if rerr:
            raise AmbiguousDirectiveError(self.node, key, val)
    
    def __init__(self, prefix):
        self.required_parameters = {}
        self.prefix = prefix
        self.node_construction = "%s" % self.prefix
        self.node_type = str
        self.node = None

    def build(self, *args):
        self.node = self.node_construction % args
        return self.node
    
    def check_name(self):
        if not self.node:
            raise NodeFormatError(self)

    def exists(self, db):
        self.check_name()
        return db.exists(self.node)

    def has(self, db, entry, val=None):
        assert(isinstance(entry, str))
        self.check_name()
        entry = self.unwrap(entry)
        if not val:
            if self.node_type is str:
                val = self.unwrap(db.get(self.node))
            
            elif self.node_type is list:
                val = get_full_list(self.node)
                val = [self.unwrap(e) for e in val]
            
            elif self.node_type is dict:
                d = db.hgetall(self.node)
                val = {}
                for e in d:
                    val[e] = self.unwrap(d[e])
            else:
                raise NodeTypeError(self)
        return entry in val

    def _import(self,db, data, flags):
        #clrln()
        #sys.stdout.write("writing node %s" % self.node)
        if self.node_type is str:
            f = bytemaps.StrStorage
            method = todb.import_string
        elif self.node_type is list:
            f = bytemaps.ListStorage
            method = todb.import_list
        elif self.node_type is dict:
            f = bytemaps.DictStorage
            for key in data:
                data[key] = self.wrap(key, data[key])
            method = todb.import_dict
        elif self.node_type is Set:
            f = bytemaps.ListStorage
            method = todb.import_set
        else:
            raise NodeTypeError(self)
        merge = bytemaps.is_set(flags, f.is_merge)
        assume_yes = bytemaps.is_set(flags, f.assume_yes)
        node_exists = self.exists(db)
        verbose = bytemaps.is_set(flags, f.verbose)
        if not merge and node_exists:
            if assume_yes and not verbose:
                c = True
            else:
                c = confirm("%s already exists. It will be DELETED before writing new data." % self.node, assume_yes)
            if not c:
                return
            else:
                db.delete(self.node)
        method(db, self.node, data, flags)

class DictNode(Node):
    def __init__(self, prefix):
        super(DictNode, self).__init__(prefix)
        self.keystone = None
        self.keystone_reqs = {}
        self.node_type = dict

    def assert_requirements(self, db, data=None):
        if not self.keystone_reqs:
            try:
                if data["template"]:
                    return
            except KeyError:
                pass
            return
        self.check_name()
        if self.keystone:
            if not data:
                keystone_val = self.unwrap(db.hget(self.node, self.keystone))
            else:
                keystone_val = self.unwrap(data[self.keystone])
            if '|' in keystone_val:
                keystone_val = keystone_val.split("|")[0]
            reqs = self.keystone_reqs[keystone_val]
        else:
            reqs = self.keystone_reqs
        if reqs[0] is not None:
            for req in reqs:
                if self.has(db, req, data):
                    continue
                else:
                    raise AttributeNotFoundError(self.node, req)

    def _import(self, db, data, flags):
        try:
            self.assert_requirements(db, data)
            super(DictNode, self)._import(db, data, flags)
        except AttributeNotFoundError as e:
            raise e
        except Exception as e:
            message = "Unchecked Error in %s, [%s]" % (self.node, repr(data))
            print(message,e)
            sys.exit()

class Namespace(Node):
    def __init__(self, prefix):
        super(Namespace, self).__init__(prefix)

    def exists(self, db):
        self.check_name()
        return len(db.keys(self.node+".*")) > 0

class Widget(DictNode):
    def __init__(self):
        super(Widget, self).__init__("widgets")
        self.node_construction += ".%s"
        self.node_type = dict
        self.keystone = "type"
        self.keystone_reqs = {
                "container" :   (None,),
                "stacked"   :   (None,),
                "image"     :   ("content", "source"),
                "image_link":   ("content", "source", "location"),
                "link"      :   ("content", "location"),
                "text"      :   ("content",),
                "input"     :   (None,),
                "password"  :   (None,)
                }

class Template(DictNode):
    def __init__(self):
        super(Template, self).__init__("templates")
        self.node_construction += ".%s"
        

class WidgetEvent(Widget):
    def __init__(self):
        super(WidgetEvent, self).__init__()
        self.node_construction += ".events.%d"
        self.keystone = "signal"
        self.keystone_reqs = required_event_parameters

class DynamicDefault(Node):
    def __init__(self, stype):
        super(DynamicDefault, self).__init__("s.global")
        self.node_construction += ".%s"
        self.node_type = stype

    def build(self, arg):
        val = cityhash.to_city_hash(arg)
        super(DynamicDefault, self).build(val)

class WidgetPolicy(Widget):
    def __init__(self):
        super(WidgetPolicy, self).__init__()
        self.node_construction += ".policy"
        self.keystone_reqs = None
        self.keystone = None

    def build(self, *args):
        self.widget_name = args[0]
        super(WidgetPolicy,self).build(args[0])

    def _import(self, db, data, flags):
        if "default" in data:
            stype_str = data["data_type"]
            stype_str = self.unwrap(stype_str)
            if stype_str == "list":
                dyndef = DynamicDefault(list)
            elif stype_str == "hash":
                dyndef = DynamicDefault(dict)
            else:
                dyndef = DynamicDefault(str)
            dyndef.build(self.widget_name)
            dyndef._import(db, data["default"], flags)
        super(WidgetPolicy, self)._import(db, data, flags)




class WidgetChildren(Node):
    def __init__(self):
        super(WidgetChildren, self).__init__("widgets")
        self.node_construction += ".%s.children"
        self.node_type = list

    def _import(self, db, data, flags):
        new_data = ["widgets.%s" % node for node in data]
        super(WidgetChildren, self)._import(db, new_data, flags)

class Perspective(Namespace):
    def __init__(self):
        super(PerspectiveMold, self).__init__("perspectives")
        self.node_construction += ".%s"

class PerspectiveMold(DictNode):
    def __init__(self):
        super(PerspectiveMold, self).__init__("perspectives")
        self.node_construction += ".%s.%d"
        self.keystone_reqs = ("name", "action", "message")

class Session(Namespace):
    def __init__(self):
        super(Session, self).__init__("s")
        self.node_construction += ".%s"

class SessionDict(DictNode):
    def __init__(self):
        super(SessionDict, self).__init__("s")
        self.node_construction += ".%s.%s"
    
class SessionList(Node):
    def __init__(self):
        super(SessionList, self).__init__("s")
        self.node_construction += ".%s.%s"
        self.node_type = list

class SessionSet(Node):
    def __init__(self):
        super(SessionSet, self).__init__("s")
        self.node_construction += ".%s.%s"
        self.node_type = Set

class SessionStr(Node):
    def __init__(self):
        super(SessionStr, self).__init__("s")
        self.node_construction += ".%s.%s"
        self.node_type = str

class GlobalEvent(DictNode):
    def __init__(self):
        super(GlobalEvent, self).__init__("events")
        self.node_construction += ".%s"
        self.keystone = "signal"
        self.keystone_reqs = required_event_parameters

class Validator(DictNode):
    def __init__(self):
        super(Validator, self).__init__("validators")
        self.node_construction += ".%s"
        self.keystone = "type"
        self.keystone_reqs = {
                "regex" :   ("test",)
        }

class DataNode(Node):
    def __init__(self):
        super(DataNode, self).__init__("data")
        self.node_construction += ".%s"
    def _import(self,db, data, flags):
        self.node_type = type(data)
        super(DataNode, self)._import(db, data, flags)
