#!/usr/bin/env python

from redis_cheats import *
from sets import Set

"""
Searches the mogu database for children without parents.
"""

def not_attr_node(node):
    EXTRA_ATTRS = (
            "events",
            "children",
            "policy"
            )
    for attr in EXTRA_ATTRS:
        if attr in node:
            return False
    return True

def event_nodes(node):
    return "events" in node

def filter_birthers(db,events):
    def event_adds_child(node):
        e_dict = db.hgetall(node) #use the
        return e_dict["action"] == "{add_child}"
    return filter(event_adds_child,events)

def birth_children(db,events):
    unborn_children = Set()
    for event in events:
        unborn_children.add(db.hget(event,"message"))
    return unborn_children

def find_orphans(args, db):
    # First, get a list of all nodes in the widgets keyspace.
    w_keyspace = db.keys("widgets.*")
    widgets = Set(filter(not_attr_node, w_keyspace))
    events = filter(event_nodes,w_keyspace) #We'll need these later

    
    # Now, we need to populate a list of all nodes that are children of another node
    kids = Set()
    lists_of_kids = db.keys("widgets.*.children")
    for node in lists_of_kids:
        for kid in full_list(db,node):
            kids.add(kid)

    # Then, we simply need to get the difference between the sets.
    # We want to see which elements of widgets are not found in kids:
    orphans = widgets.difference(kids)

    # However, we also have to check to make sure that these nodes
    # are never added later on. Therefore, we have to also check all
    # of the event nodes and perspectives.
    # To do this, we're going to add all of the event nodes we can find
    # into the events list from before:
    events.extend(db.keys("perspectives.*"))
    events.extend(db.keys("events.*"))
    events = filter_birthers(db,events)
    fetuses = birth_children(db,events)
    orphans = orphans.difference(fetuses)

    for orphan in orphans:
        if "wrapper" not in orphan:
            print orphan

if __name__ == "__main__":
    import redis
    db = redis.Redis()
    find_orphans(1,db)
