from redis_cheats import full_list
from sets import Set
import string
from coloring import *
import os

def clean_str(string):
    return string.replace("\"","\\\"")

def dict_str(dict_entries):
    str_entries = []
    for entry in dict_entries:
        value = clean_str(dict_entries[entry])
        as_string = "\t%-15s:\t%s" % ('"'+entry+'"', '"'+value+'"')
        str_entries.append(as_string)
    entries_body = (",\n".join(str_entries))+","
    output = "{\n%s\n}\n" % entries_body
    return output


def set_str(set_entries, quote="\""):
    str_entries = []
    for entry in set_entries:
        as_string = "\t%s%s%s" % (quote, entry, quote)
        str_entries.append(as_string)
    body = (",\n".join(str_entries))+","
    output = "Set([\n%s\n])\n" % body
    return output

def list_str(list_entries, quote="\""):
    str_entries = []
    for entry in list_entries:
        as_string = "\t%s%s%s" % (quote,entry,quote)
        str_entries.append(as_string)
    body = (",\n".join(str_entries))+","
    output = "[\n%s\n]\n" % body
    return output

def dict_entry_line(dict_name, entry_name):
    return "%s[\"%s\"] = \\\n" % (dict_name, entry_name)

def dbl_dict_entry_line(dict_name, innerdict_name, entry_name):
    return "%s[\"%s\"][\"%s\"] = \\\n" % (dict_name, innerdict_name, entry_name)

def empty_dict_entry_line(dict_name, innerdict_name):
    return "%s[\"%s\"] = {}\n" % (dict_name, innerdict_name)

def test(filename):
    widgets = {}
    tree = {}
    events = {}
    meta = {}
    perspectives = {}
    try:
        execfile(filename)
    except Exception as e:
        raise MoguImportException(filename, e.__str__())

def dict_to_string(dict_name,entry_name,entries):
    title = dict_entry_line(dict_name,entry_name)
    body = dict_str(entries)
    output = "%s%s\n\n" % (title,body)
    return output

def export_widget_dict(db,widget):
    return dict_to_string(
            "widgets",
            widget.replace("widgets.",""),
            db.hgetall(widget)
            )

def export_widget_policy(db,policy):
    policy_name = policy.replace("widgets.","").replace(".policy","")
    policy = "widgets.%s.policy" % policy_name
    if db.exists(policy):
        return dict_to_string(
                "policies",
                policy_name,
                db.hgetall(policy)
                )
    else:
        return None
            
def export_validator(db, validator):
    validator_name = validator.replace("validators.","")
    return dict_to_string(
            "validators",
            validator_name,
            db.hgetall(validator)
            )
def export_widget_events(db,widget):
    event_nodes = db.keys("%s.events.*" %widget)
    if (len(event_nodes) is 0):
        return ""
    output = ""
    title = dict_entry_line("events",widget.replace("widgets.",""))
    event_dicts = []
    for node in event_nodes:
        event_dicts.append(dict_str(db.hgetall(node)))
    body = list_str(event_dicts,"")
    output = "%s%s" % (title,body)
    return output


def export_perspective(db,perspective):
    perspective_nodes = db.keys("perspectives.%s.*" % perspective.replace("perspectives.",""))
    if (len(perspective_nodes) is 0):
        return ""
    output = ""
    title = dict_entry_line("perspectives",perspective.replace("perspectives.",""))
    perspective_dicts = []
    for node in perspective_nodes:
        perspective_dicts.append(dict_str(db.hgetall(node)))
    body = list_str(perspective_dicts,"")
    output = "%s%s" % (title,body)
    return output


def export_widget_children(db,widget):
    output = ""
    if db.exists("%s.children" % widget):
        title = dict_entry_line("tree", widget.replace("widgets.",""))
        children = full_list(db,"%s.children" % widget)
        body = list_str([child.replace("widgets.","") for child in children])
        output = "%s%s" % (title,body)
    return output


def export_data(db, data_node):
    output = dict_entry_line("data", data_node)
    lookup = "data.%s" % data_node.replace("data.","")
    dtype = db.type(lookup)
    if dtype == "hash":
        val = db.hgetall(lookup)
        body = dictstr(val)
    elif dtype == "list":
        val = full_list(db,lookup)
        body = list_str(val)
    elif dtype == "set":
        val = db.smembers(lookup)
        body = set_str(val)
    else:
        body = " \t\"%s\"\n" % clean_str(db.get(lookup))

    output += "%s" % (body)
    return output

def export_session(db,session):
    output = empty_dict_entry_line("sessions", session)
    _session_nodes = db.keys("s.%s.*" % session)
    session_nodes = []
    for n in _session_nodes:
        session_nodes.append(n.split(".")[2])

    for node in session_nodes:
        body = ""
        name = "s.%s.%s" % (session, node)
        t = db.type(name)
        title = dbl_dict_entry_line("sessions", session, node)
        if t == "hash":
            d = db.hgetall(name)
            body = dict_str(d)
        elif t == "list":
            l = full_list(db, name)
            body = list_str(l)
        elif t == "set":
            s = db.smembers(name)
            body = set_str(s)
        else:
            body = " \t\"%s\"\n" % clean_str(db.get(name))
        output += "%s%s" % (title,body)
    return output

def format_filename(f):
    if '.' in f:
        f = ".".join(f.split(".")[1:])
    for char in f:
        if char not in string.ascii_letters\
                and char not in string.digits:
                    f = f.replace(char,"_")
    return f + ".mogu"

def export(db, outInfo, toPackage):
    widgets = [key for key in db.keys('widgets.*') \
            if db.type(key) == 'hash' and \
            '.events' not in key and \
            '.children' not in key and \
            '.policy' not in key
            ]
    
    if not toPackage:
        output = ""

    os.mkdir(outInfo+"/widgets")
    for widget in widgets:
        if toPackage:
            #Convert widgets.foo:bar to foo_bar.mogu
            filename = format_filename(widget)
            write_content(outInfo+"/widgets/"+filename, export_widget_dict(db,widget))
        else:
            output += export_widget_dict(db,widget)
            

    for widget in widgets:
        temp = export_widget_events(db,widget)
        if "\"" in temp:
            if toPackage:
                filename = format_filename(widget)
                write_content(outInfo+"/widgets/"+filename, temp)
            else:
                output += temp

    for widget in widgets:
        temp = export_widget_children(db,widget)
        if "\"" in temp:
            if toPackage:
                filename = format_filename(widget)
                write_content(outInfo+"/widgets/"+filename, temp)
            else:
                output += temp
   
    for widget in widgets:
        temp = export_widget_policy(db,widget)
        if temp:
            if toPackage:
                filename = format_filename(widget)
                write_content(outInfo+"/widgets/"+filename, temp)
            else:
                output += temp
            
    perspective_events = db.keys("perspectives.*")
    perspectives = []
    for ev in perspective_events:
        pers = ev.split('.')[1]
        if pers not in perspectives:
            perspectives.append(pers)
    os.mkdir(outInfo+"/perspectives/")
    for perspective in perspectives:
        if toPackage:
            filename = format_filename(perspective) 
            write_content(outInfo+"/perspectives/"+filename, export_perspective(db,perspective))
        else:
            output += export_perspective(db,perspective)

    validators = db.keys("validators.*")
    os.mkdir(outInfo+"/validators")
    for validator in validators:
        temp = export_validator(db, validator)
        if "\"" in temp:
            if toPackage:
                filename = format_filename(validator)
                write_content(outInfo+"/validators/"+filename, temp)
            else:
                output += temp
        
    global_events = db.keys("events.*")
    os.mkdir(outInfo+"/global_events")
    for event in global_events:
        title = dict_entry_line("global_events", event.replace("events.",""))
        body = dict_str(db.hgetall(event))
        if toPackage:
            filename = format_filename(event)
            write_content(outInfo+"/global_events/"+filename, "%s%s\n\n" % (title,body))
        else:    
            output += "%s%s\n\n" % (title,body)

    __sessions = db.keys("s.*")
    sessions = []
    for sesh in __sessions:
        sid = sesh.split(".")[1]
        if sid not in sessions:
            sessions.append(sid)
    for session in sessions:
        if toPackage:
            write_content(outInfo+"/sessions.mogu", export_session(db,session))
        else:        
            output += export_session(db,session)
        
    data = db.keys("data.*")
    for node in data:
        nname = node.split(".")[1]
        if toPackage:
            write_content(outInfo+"/data.mogu", export_data(db,nname))
        else:
            output += export_data(db,nname)
    
    if not toPackage:
        write_content(outInfo, output, 'w')
    
def write_content(filename, content, writeMode='a'):

    if not filename.endswith(".mogu"):
        filename+=".mogu"
    
    preface = "" if os.path.exists(filename) else "#Mogu Import File: %s \n\n"%filename

    f = open(filename, writeMode)
    f.write(preface+content.replace("\n\"","\"\n"))
    f.close()
