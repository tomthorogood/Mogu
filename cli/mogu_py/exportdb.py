from redis_cheats import full_list

class MoguImportException(Exception):
    def __init__(self, filename, error_message):
        self.value = "Unfortunately, there is something wrong with your Mogu import file %s." % filename
        self.value += "This is most likely not your fault. Please let us know of the error at " 
        self.value += "github.com/tomthorogood/mogu"
        self.value += "\n The error report was: \n"
        self.value += error_message
    def __str__(self):
        return self.value

def test(filename):
    widgets = {}
    tree = {}
    events = {}
    meta = {}
    try:
        execfile(filename)
    except Exception as e:
        raise MoguImportException(filename, e.__str__())


def export(db, filename):
    f = open(filename, 'w')
    widgets = [key for key in db.keys('widgets.*') \
            if db.type(key) == 'hash' and \
            'events' not in key and \
            'chilren' not in key
            ]

    for widget in widgets:
        output = "widgets[\"%s\"] = {\n" %widget.replace("widgets.","")
        entries = db.hgetall(widget)
        dict_keys = []
        for entry in entries:
            value = entries[entry].replace("\"","\\\"")
            dict_keys.append("\t\"%s\"   :   \"%s\"" % (entry, value))
        output += ",\n".join(dict_keys)
        output += "}\n"
        f.write(output)

    for widget in widgets:
        event_nodes = db.keys("%s.events.*" %widget)
        if event_nodes:
            output = "events[\"%s\"] = [\n" % widget.replace("widgets.","")
            event_dicts = []
            for event in event_nodes:
                entries = db.hgetall(event)
                event_dict = "\t{\n"
                dict_keys = []
                for entry in entries:
                    value = entries[entry].replace("\"","\\\"")
                    dict_keys.append("\t\t\"%s\"    :   \"%s\"" % (entry,value))
                event_dict += ",\n".join(dict_keys)
                event_dict += "\n}"
                event_dicts.append(event_dict)
            output += ",\n".join(event_dicts)
            output += "\n]\n"
            f.write(output)

    for widget in widgets:
        if db.exists("%s.children" % widget):
            output = "tree[\"%s\"] = [\n" % widget.replace("widgets.","")
            children = db.lrange("%s.children" % widget, 0, db.llen("%s.children" % widget))
            children = ["\t\"%s\"" % child.replace("widgets.","") for child in children]
            output += ",\n".join(children)
            output += "\n]\n"
            f.write(output)

    global_events = db.keys("events.*")
    for event in global_events:
        output = "global_events[\"%s\"] = {\n" % event.replace("events.","")
        entries = db.hgetall(event)
        dict_keys = []
        for entry in entries:
            value = entries[entry].replace("\"","\\\"")
            dict_keys.append("\t\"%s\"   :   \"%s\"" % (entry, value))
        output += ",\n".join(dict_keys)
        output += "}\n"
        f.write(output)

    meta_values = db.keys("meta.*")
    for key in meta_values:
        key_id = key.replace("meta.","")
        key_type = db.type(key)
        wrappers = {
                "hash"  :   ("{\n","\n}"),
                "list"  :   ("[\n","\n]"),
                "string":   ("","")
                }
        wrapper = wrappers[key_type]
        output = "meta[\"%s\"] = %s " % (key_id,wrapper[0])
        if key_type == "string":
            output += db.get(key)
        elif key_type == "list":
            values = full_list(db,key)
            value_list = []
            for value in values:
                value_list.append("\"%s\"" % value)
            output += ",\n".join(value_list)
            output += wrapper[1]
        elif key_type == "hash":
            value_list = []
            for value in values:
                value_list.append("\"%s\"\t:\t\"%s\"" % (value, values[value]))
            output += ",\n".join(value_list)
            output += wrapper[1]
        f.write(output)
        f.write("\n")

    f.close()
    f = open(filename,'r')
    output = f.read()
    output = output.replace("\n\"","\"\n")
    f.close()
    f = open(filename,'w')
    f.write(output)
    f.close()

