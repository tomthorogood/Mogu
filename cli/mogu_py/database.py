#!/usr/bin/env python
from db_checking import *
import os,sys

class MissingChildError(Exception):
    def __init__(self, widget_name):
        self.value = ("Cannot add child that doesn't exist! %s" % widget_name)
    def __str__(self):
        return self.value

class UnknownWidgetTypeError(Exception):
    def __init__(self, widget_name, widget_type):
        self.value = ("Widget %s is of unknown type %s" % widget_name, widget_type)

    def __str__(self):
        return self.value

def add_tree(tree,db):
    for widget_name in tree:
        if not db.exists("widgets.%s" % widget_name):
            raise MissingChildError(widget_name)

    for widget_name in tree:
        db.delete("widgets.%s.children" % widget_name)
        for branch in tree[widget_name]:
            db.rpush("widgets.%s.children" % widget_name, "widgets.%s" % branch)

def check_widgets(widgets):
    try:
        for widget in widgets:
            args = widgets[widget]
            args = check_widget_values(widget,args)
            widgets[widget] = args
        return widgets
    except MissingParameterError as e:
        print e
        sys.exit()
    except AmbiguousDirectiveError as e:
        print e
        sys.exit()
    except UnknownWidgetTypeError as e:
        print e
        sys.exit()

def check_events(events,db,test=False):
    try:
        for widget in events:
            if not test:
                assert(db.exists("widgets.%s"%widget))
            num_events =0
            for event in events[widget]:
                num_events+=1
                event_name = "widgets.%s.events.%d" % (widget,num_events)
                event = check_event_values(event_name,event)
                events[widget][num_events-1] = event
        return events
    except MissingParameterError as e:
        print e
        sys.exit()

def add_dict(node,dictionary,db):
    db.delete(node)
    for key in dictionary:
        db.hset(node,key,dictionary[key])

def add_widget(short_name,args,db):
    name = "widgets.%s" % short_name
    add_dict(name,args,db)

def add_event(widget_name,event_num,args,db):
    name = "widgets.%s.events.%d" % (widget_name, event_num)
    add_dict(name,args,db)

def add_global_event(event_name,args,db):
    name = "events.%s" % event_name
    add_dict(name,args,db)

def add_widget_set(db, package, test=False, flush=False):
    widgets = package.widgets
    widgets = check_widgets(widgets)
    if not test:
        if flush:
            db.flushdb()
        for widget in widgets:
            add_widget(widget,widgets[widget],db)
    events = package.events
    if events:
        events = check_events(events,db,test)
        if not test:
            for widget in events:
                num_events = 0
                for event in events[widget]:
                    num_events += 1
                    add_event(widget,num_events,event,db)
    children = package.tree
    if children and not test:
        add_tree(children,db)

def add_global_events(db,events,test=False):
    for event in events:
        args = check_event_values(event,events[event])
        if not test:
            add_global_event(event,args,db)
