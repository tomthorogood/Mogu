#!/usr/bin/env python

class MoguObject(object):
    def __init__(self, attributes = []):
        self.allowed = ["moguscript"]
        self.allowed.extend(attributes)
        self.moguScript = None


    def __setattr__(self,name,val):
        if name not in self.allowed:
            raise AttributeError
        else:
            self.__dict__[name] = val
