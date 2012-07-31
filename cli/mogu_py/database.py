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


def add_list(node,entries,db,merge=False):
    if not merge:
        db.delete(node)
    for entry in entries:
        db.rpush(node,entry)

def add_dict(node, dictionary, db, merge=False):
    if not merge:
        db.delete(node)
    for key in dictionary:
        db.hset(node,key,dictionary[key])

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


def add_tree(tree, db, merge=False):
    for widget_name in tree:
        if not db.exists("widgets.%s" % widget_name):
            raise MissingChildError(widget_name)

    for widget_name in tree:
        node = "widgets.%s.children" % widget_name.replace("widgets.","")
        add_list(node, tree[widget_name], db, merge)

def add_widget(short_name,args,db, merge=False):
    name = "widgets.%s" % short_name
    add_dict(name,args,db,merge)

def add_event(widget_name, event_num, args, db, merge=False):
    name = "widgets.%s.events.%d" % (widget_name, event_num)
    add_dict(name,args,db,merge)

def add_perspective_mold(perspective_name, mold_num, mold, db, merge=False):
    name = "perspectives.%s.%d" % (perspective_name, mold_num)
    add_dict(name,mold,db,merge)

def add_perspective(perspective_name,molds, db, test=False, merge=False):
    i = 0
    for mold in molds:
        i+=1
        mold = check_perspective_mold(mold)
        if not test:
            add_perspective_mold(perspective_name, i, mold, db, merge)

def add_global_event(event_name, args, db, merge=False):
    name = "events.%s" % event_name
    add_dict(name, args, db, merge)

def add_widget_set(db, package, test=False, flush=False, merge=False):
    widgets = package.widgets
    widgets = check_widgets(widgets)
    if not test:
        if flush:
            db.flushdb()
        for widget in widgets:
            add_widget(widget, widgets[widget], db, merge)
    events = package.events
    if events:
        events = check_events(events,db,test)
        if not test:
            for widget in events:
                num_events = 0
                for event in events[widget]:
                    num_events += 1
                    add_event(widget, num_events, event,db, merge)
    children = package.tree
    if children and not test:
        add_tree(children, db, merge)

def add_global_events(db,events,test=False, merge=False):
    for event in events:
        args = check_event_values(event,events[event])
        if not test:
            add_global_event(event, args,db, merge)

def add_perspectives(db, perspectives, test=False, merge=False):
    for perspective in perspectives:
        add_perspective(perspective, perspectives[perspective], db, test, merge)


def add_meta_value(db, name, value, test=False, merge=False):
    node = "meta.%s" % name
    if isinstance(value,str) and not test:
        db.set(node,value)
    elif isinstance(value,dict):
        add_dict(node,value,merge)
    elif isinstance(value,list):
        add_list(node,value,merge)

def add_meta_values(db, values, test=False, merge=False):
    for entry in values:
        add_meta_value(db,entry,values[entry],test, merge)
