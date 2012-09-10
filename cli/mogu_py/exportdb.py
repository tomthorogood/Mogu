from redis_cheats import full_list

def clean_str(string):
    return string.replace("\"","\\\"")

def dict_str(dict_entries):
    str_entries = []
    for entry in dict_entries:
        value = clean_str(dict_entries[entry])
        as_string = "\t\"%s\"\t:\t\"%s\"" % (entry, value)
        str_entries.append(as_string)
    entries_body = ",\n".join(str_entries)
    output = "{\n%s\n}\n\n" % entries_body
    return output

def list_str(list_entries, quote="\""):
    str_entries = []
    for entry in list_entries:
        as_string = "\t%s%s%s" % (quote,entry,quote)
        str_entries.append(as_string)
    body = ",\n".join(str_entries)
    output = "[\n%s\n]\n\n" % body
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
    return dict_to_string(
            "policies",
            policy_name,
            db.hgetall(policy)
            )
            
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

        

def clean_file(filename):
    f = open(filename,'r')
    output = f.read()
    output = output.replace("\n\"","\"\n")
    f.close()
    f = open(filename,'w')
    f.write(output)
    f.close()


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
        else:
            body = " \t\"%s\"\n" % clean_str(db.get(name))
        output += "%s%s" % (title,body)
    return output

def export(db, filename):
    f = open(filename, 'w')
    f.write("#!/usr/bin/env python\n")
    f.write("#Mogu Import File: %s \n\n" % filename)
    widgets = [key for key in db.keys('widgets.*') \
            if db.type(key) == 'hash' and \
            '.events' not in key and \
            '.children' not in key and \
            '.policy' not in key
            ]

    for widget in widgets:
        output = export_widget_dict(db,widget)
        f.write(output)

    for widget in widgets:
            output = export_widget_events(db,widget)
            if "\"" in output:
                f.write(output)

    for widget in widgets:
        output = export_widget_children(db,widget)
        if "\"" in output:
            f.write(output)
    
    policies = db.keys("*.policy")
    for p in policies:
        output = export_widget_policy(db,p)
        if "\"" in output:
            f.write(output)

    perspectives = db.keys("perspectives.*")
    perspectives_ = []
    for p in perspectives:
        ex = p.split('.')
        perspectives_.append(ex[1])
    perspectives = perspectives_
    for perspective in perspectives:
        output = export_perspective(db,perspective)
        f.write(output)

    validators = db.keys("validators.*")
    for validator in validators:
        output = export_validator(db, validator)
        if "\"" in output:
            f.write(output)

    global_events = db.keys("events.*")
    for event in global_events:
        title = dict_entry_line("global_events", event.replace("events.",""))
        body = dict_str(db.hgetall(event))
        output = "%s%s\n\n" % (title,body)
        f.write(output)

    meta_values = db.keys("meta.*")
    for key in meta_values:
        key_id = key.replace("meta.","")
        key_type = db.type(key)
        title = dict_entry_line("meta", key_id)
        if key_type == "string":
            body = "\"%s\"" % (db.get(key))
        elif key_type == "list":
            body = list_str(db,full_list(key))
        else:
            body = dict_str(db.hgetall(key))
        output = "%s%s" % (title,body)
        f.write(output)

    __sessions = db.keys("s.*")
    sessions = []
    for sesh in __sessions:
        sid = sesh.split(".")[1]
        if sid not in sessions:
            sessions.append(sid)
    for session in sessions:
        output = export_session(db,session)
        f.write(output)


    f.close()
    clean_file(filename)
