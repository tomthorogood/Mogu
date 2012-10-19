from redis_cheats import full_list
from node_name import NodePatterns as Pattern
from coloring import *
from snippets import *
from sets import Set

import string
import os


def export_all(args, db):
    if args.single:
        if len(args.command) is 1:
            error = fail(
                    "You have told me that you want to export the database, but have not given me a filename to write to.")
            sys.exit() 
        
        #check to see if the file exists (adds .mogu extension to the filename if it's not present)
        
        if not confirm_overwrite(args.command[1],args.yes):
                sys.exit()
        
    else:
        if len(args.command) is 1:
            error = fail("You have told me that you want to export the database to a directory, but have not given me a directory to write to.")
            sys.exit()
            
        #Check if directory exists
        if not os.path.isdir(args.command[1]):
            os.makedirs(args.command[1]) #if not, create it/them
        
    export(db,args.command[1], not args.single)

def export_widget(args,db):
    try:
        widgets = args.command[1:]
    except:
        error = fail("You have told me that you want to export a widget from the database, but have not told me what widget to export.")
        print("%s\nYou can fix this by making sure to pass the widget name: mogu export-widget widget_name --filename file")
        sys.exit()
    filename = assertending(args.filename, ".mogu")
    if not args.filename:
        error = fail("You have told me that you want to export a widget from the database, but have not given me a filename to write to.")
        print("%s\n You can fix this by using the --filename option. ex: ./mogu export --filename /path/to/file")%error
        sys.exit()

    if not confirm_overwrite(args.filename, args.yes):
        sys.exit()
    
    f = open(filename, "w")
    f.write("# Mogu Import File %s" % filename)
    f.write("\n\n")
    for widget in widgets:
        #widget = "widgets.%s" % (widget.replace("widgets.",""))
        output = []
        output.append( export_widget_dict(db,widget) )
        output.append( export_widget_properties(db, widget) )
        output.append( export_widget_events(db, widget) )
        output.append( export_widget_children(db, widget) )
        output_str = "".join(output)
        f.write(output_str)
    f.close()

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
        as_string = "\t" + "%s" * 3 % (quote, entry, quote)
        str_entries.append(as_string)
    body = (",\n".join(str_entries))+","
    output = "Set([\n%s\n])\n" % body
    return output

def list_str(list_entries, quote="\""):
    str_entries = []
    for entry in list_entries:
        as_string = "\t" + "%s" * 3 % (quote,entry,quote)
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

def dict_to_string(dict_name,entry_name,entries):
    title = dict_entry_line(dict_name,entry_name)
    body = dict_str(entries)
    output = "%s" * 2 % (title,body) + "\n\n" 
    return output

def export_widget_dict(db,widget):
    pattern = Pattern.Widget(widget)
    return dict_to_string(
            "widgets",
            pattern.name,
            db.hgetall(pattern.node)
            )

def export_widget_policy(db,policy):
    pattern = Pattern.WidgetPolicy(policy)
    if db.exists(pattern.node):
        return dict_to_string(
                "policies",
                pattern.name,
                db.hgetall(pattern.node)
                )
    else:
        return None
            
def export_validator(db, validator):
    pattern = Pattern.Validator(validator)
    return dict_to_string(
            "validators",
            pattern.name,
            db.hgetall(pattern.node)
            )
def export_widget_events(db,widget):
    pattern = Pattern.Widget(widget)
    event_nodes = db.keys("%s.events.*" % pattern.node)
    if (len(event_nodes) is 0):
        return ""
    output = ""
    title = dict_entry_line("events", pattern.name)
    event_dicts = []
    for node in event_nodes:
        event_dicts.append(dict_str(db.hgetall(node)))
    body = list_str(event_dicts,"")
    output = "%s" * 2 % (title,body)
    return output


def export_widget_properties(db, widget):
    pattern = Pattern.Widget(widget)
    property_node = "%s.properties" % pattern.node
    if not db.exists(property_node): return None
    title = dict_entry_line("properties", pattern.name)
    data = db.smembers(property_node)
    body = set_str(data)
    output = "%s%s" % (title, body)
    return output

def export_perspective(db,perspective):
    pattern = Pattern.Perspective(perspective)
    perspective_nodes = db.keys(pattern.node+".*")
    if (len(perspective_nodes) is 0):
        return ""
    output = ""
    title = dict_entry_line("perspectives", pattern.name)
    perspective_dicts = []
    for node in perspective_nodes:
        perspective_dicts.append(dict_str(db.hgetall(node)))
    body = list_str(perspective_dicts,"")
    output = "%s" * 2 % (title,body)
    return output


def export_widget_children(db,widget):
    pattern = Pattern.Widget(widget)
    output = ""
    if db.exists("%s.children" % widget):
        title = dict_entry_line("tree", pattern.name)
        children = full_list(db,"%s.children" % pattern.node)
        strlst = []
        for child in children:
            ptrn = Pattern.Widget(child)
            strlst.append(ptrn.name)
        body = list_str(strlst)
        output = "%s" * 2 % (title,body)
    return output

def export_data(db, data_node):
    pattern = Pattern.Data(data_node)
    output = dict_entry_line("data", pattern.name)
    dtype = db.type(pattern.node)
    if dtype == "hash":
        val = db.hgetall(pattern.node)
        body = dict_str(val)
    elif dtype == "list":
        val = full_list(db,pattern.node)
        body = list_str(val)
    elif dtype == "set":
        val = db.smembers(pattern.node)
        body = set_str(val)
    else:
        body = " \t\"%s\"\n" % clean_str(db.get(pattern.node))

    output += "%s" % (body)
    return output

def export_template(db, tname):
    pattern = Pattern.Template(tname)
    output = dict_entry_line("templates", pattern.name)
    val = db.hgetall(pattern.node)
    body = dict_str(val)
    output += body
    return output

def export_session(db,session):
    pattern = Pattern.Session(session)
    output = empty_dict_entry_line("sessions", pattern.name)
    _session_nodes = db.keys("%s.*" % pattern.node)
    session_nodes = []
    for n in _session_nodes:
        session_nodes.append(n.split(".")[2])

    for node in session_nodes:
        body = ""
        name = "%s.%s" % (pattern.node, node)
        t = db.type(name)
        title = dbl_dict_entry_line("sessions", pattern.name, node)
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
        temp = export_widget_properties(db, widget)
        if temp is not None:
            if toPackage:
                filename = format_filename(widget)
                write_content(outInfo+"/widgets/"+filename, temp)
            else:
                output += temp

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
    
    templates = db.keys("templates.*")
    for t in templates:
        tname = t.split(".")[1]
        if toPackage:
            write_content(outInfo+"/templates.mogu", export_template(db,tname))
        else:
            output += export_data(db,tname)

    if not toPackage:
        write_content(outInfo, output, 'w')
    
def write_content(filename, content, writeMode='a'):

    if not filename.endswith(".mogu"):
        filename+=".mogu"
    
    preface = "" if os.path.exists(filename) else "#Mogu Import File: %s \n\n"%filename

    f = open(filename, writeMode)
    f.write(preface+content.replace("\n\"","\"\n"))
    f.close()
