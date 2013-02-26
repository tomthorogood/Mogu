#!/usr/bin/env python
from sets import Set
from collections import OrderedDict

class RedisObject(object):
    def __init__(self, node_name, dataformat):
        self.node = node_name
        self.data = None
        self.dataformat = dataformat

    def setData(self,value=None):
        if value is None:
            return self.data
        elif not isinstance(value,self.dataformat):
            raise TypeError
        self.data = value

    def __str__(self):
        return repr(self.data)

class RedisHash(RedisObject):
    def __init__(self, node_name, data={}):
        super(RedisHash, self).__init__(node_name, OrderedDict)
        self.setData(data)

class RedisList(RedisObject):
    def __init__(self, node_name, data=[]):
        super(RedisList, self).__init__(node_name, list)
        self.setData(data)

class RedisSet(RedisObject):
    def __init__(self, node_name, data=Set()):
        super(RedisSet, self).__init__(node_name, Set)
        self.setData(data)

class RedisString(RedisObject):
    def __init__(self, node_name, data=""):
        super(RedisString, self).__init__(node_name, (int,str,float))
        self.setData(data)
