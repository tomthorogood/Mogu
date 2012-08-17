#!/usr/bin/env python

from db_checking import *
from redis_cheats import *
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
