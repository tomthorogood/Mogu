#!/usr/bin/env python

import redis
from sets import Set
from collections import OrderedDict
import RedisObjects

class RedisWriter(object):
    def __init__(self, db=0, host="localhost",port=6379):
        self.db = redis.Redis(db=db,host=host,port=port)

    def write(self, collection):
        for obj in collection:
            if not isinstance(obj, RedisObjects.RedisObject):
                raise TypeError("Must be in RedisObject format.")
        for obj in collection:
            if isinstance(obj, RedisObjects.RedisString):
                self.db.set(obj.node,obj.data)

            elif isinstance(obj, RedisObjects.RedisList):
                for value in obj.data:
                    self.db.rpush(obj.node, value)

            elif isinstance(obj, RedisObjects.RedisHash):
                for value in obj.data:
                    self.db.hset(obj.node,value,obj.data[value])

            elif isinstance(obj, RedisObjects.RedisSet):
                for value in obj.data:
                    self.db.sadd(obj.node, value)
