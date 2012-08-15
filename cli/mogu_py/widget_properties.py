import strformat as str_

def build_node(prefix, body, suffix):
    return "%s.%s" % (str_.assert_prefix(body, prefix),suffix)

def check_widget_suffix(db, widget_name, suffix):
    node = build_node("widgets", widget_name, suffix)
    return len(db.keys(node+"*")) > 0

def has_children(db, widget_name):
    return check_widget_suffix(db, widget_name, "children")

def has_events(db, widget_name):
    return check_widget_suffix(db, widget_name, "events")

def num_children(db, widget_name):
    node = build_node("widgets", widget_name, "children")
    return db.llen(node)

def num_events(db, widget_name):
    node = build_node("widgets", widget_name, "events")
    return len(db.keys(node+"*"))

