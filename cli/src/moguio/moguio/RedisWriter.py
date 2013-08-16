#!/usr/bin/env python
import sys
import os
import redis
from sets import Set
from collections import OrderedDict
import RedisObjects

# DEFAULT DBCONFIG.CONF LOCATION:
DB_CONFIG_PATH = "../../../../"
DB_CONFIG_FILE = "dbconfig.conf"


def parseDBConfig(filename):
    """


    :param filename: The full path and filename to be parsed.
    """
    if not filename:
        raise IOError("No filename listed")

    def important(string):
        """Removes comments and blank lines
        :param string: The string to be tested.
        """
        if len(string) <= 1 or string[0] == "#":
            return False
        return True

    lines = None

    with open(filename,"r") as f:
        lines = f.readlines()

    lines = [line.strip() for line in lines]  # Strip whitespace
    lines = filter(important,lines)
    
    confdict    = {}
    prefix      = None
    attr        = None
    value       = None

    for line in lines:
        if line[0] == '@':
            prefix = line[1:]   # '@meta will resolve to 'meta'
            confdict[prefix] = {}
        if ":" in line:         # The line contains an attribute definition
            delim_loc = line.index(":")
            attr = line[:delim_loc].strip()
            value = line[delim_loc+1:].strip()
            confdict[prefix][attr] = value
    return confdict

class RedisWriter(object):
    def __init__(self,args=None):
        """
            args should be the results of the ArgumentParser module run when 
            the 'mogu' command is executed. This is optional. If not passed in,
            sensible defaults will be set:
                verbal = True
        """
        self.dbconfig   = None  # Will be set in try...except below
        self.verbal     = args.v if args else True
        self.yes        = args.y if args else False
        self.flushdb    = args.flushdb if args else False
        self.flushrules = args.flushrules if args else None
        dbconfig        = args.dbconfig if args else os.path.join(
                            DB_CONFIG_PATH,DB_CONFIG_FILE)
        
        try:
            self.dbconfig = parseDBConfig(dbconfig)
        except IOError: # If the file cannot be found, set sensible defaults
            self.dbconfig = {}
            self.dbconfig['default'] = {
                    'host'  :   'localhost',
                    'port'  :   '6379',
                    'number':   '0'
            }
            sys.stderr.write("WARNING: dbconfig file (%s) not found. "
                    "You should only continue if you are a Mogu developer "
                    "testing things.\n" % dbconfig )
                
            if not self.yes:
                cont = raw_input("Continue anyway? type 'continue' to continue :")
                if cont.strip().lower() != 'continue':
                    sys.stderr.write("Exiting.")
                    sys.exit()
                else:
                    sys.stderr.write("Using\n\t"
                            "host: %(host)s\n\t"
                            "port: %(port)s\n\t"
                            "select: %(number)s"
                            % self.dbconfig['default'])
            else:
                sys.stderr.write("'yes' assumed due to -y switch\n")
        
             
        self.databases = {}
        self.pipes = {}
        
        
        # Create the Redis connection pools and pipelines
        for prefix in self.dbconfig:
            db = redis.Redis(
                    host    = self.dbconfig[prefix]['host'],
                    port    = int(self.dbconfig[prefix]['port']),
                    db      = int(self.dbconfig[prefix]['number'])
                    )
            self.databases[prefix] = db
            self.pipes[prefix] = db.pipeline()
        
        if self.flushdb:
            for prefix in self.dbconfig:
                self.flush(prefix)

    def pipe(self,prefix):
        """
            When given a string, determines whether the
            string contains Mogu's keyspace delimiter.
            If so, uses everything up until the first occurence of
            this delimiter as a prefix, otherwise, uses the 
            entire string as a prefix, returning the pipeline
            for the prefix.

            If the prefix does not exist, and there is no default set,
            raises a KeyError

            The default is only set if no dbconfig.conf file was found
            and the user explicitly said to continue anyway.
        """
        if "." in prefix:
            prefix = prefix.split(".")[0]
        if prefix in self.pipes:
            return self.pipes[prefix]
        elif 'default' in self.pipes:
            return self.pipes['default']
        else:
            raise KeyError("Redis prefix %s is not defined" % prefix)

    def stringWriter(self, redis_object):
        pipe = self.pipe(redis_object.node)
        pipe.set(redis_object.node,redis_object.data)

    def listWriter(self, redis_object):
        pipe = self.pipe(redis_object.node)
        pipe.delete(redis_object.node)
        for value in redis_object.data:
            pipe.rpush(redis_object.node, value)

    def hashWriter(self, redis_object):
        pipe = self.pipe(redis_object.node)
        pipe.delete(redis_object.node)
        for key in redis_object.data:
            pipe.hset(redis_object.node, key, redis_object.data[key])

    def flush(self, prefix):
        pipe = self.pipe(prefix)
        # Get a list of all relevant keys.
        keys = pipe.keys("%s.*" % prefix)
        for k in keys:
            pipe.delete(k)

    def askflush(self, prefix):
        if args.y:
            return True
        
        user_input = raw_input("Do you wish to flush the %s prefix?\n[Y]es to continue, anything else to skip: ")
        return user_input.lower()[0] == 'y'

    def checkflush(self, prefix):

        if (not self.flushrules) or \
            (prefix not in self.flushrules) or \
            ('Q' == self.flushrules[prefix].upper()):
                return self.askflush(prefix)

        if 'A' == self.flushrules[prefix].upper():
            return True
        
        return False

    def setLogging(self, level):
        pipe=self.pipe("meta")
        pipe.set("meta.log", level)
        pipe.execute()

    def setRoot(self, root):
        pipe = self.pipe("meta")
        pipe.set("meta.root", root)
        pipe.execute()

    def setWriter(self, redis_object):
        pipe = self.pipe(redis_object.node)
        for value in redis_object.data:
            pipe.sadd(redis_object.node, value)

    def write(self, collection):

        for obj in collection:
            if not isinstance(obj, RedisObjects.RedisObject):
                raise TypeError("Found type %s. Expected type RedisObject" % \
                        str(type(obj)))

        for obj in collection:
            if isinstance(obj, RedisObjects.RedisString):
                self.stringWriter(obj)
            elif isinstance(obj, RedisObjects.RedisList):
                self.listWriter(obj)
            elif isinstance(obj, RedisObjects.RedisHash):
                self.hashWriter(obj)
            elif isinstance(obj, RedisObjects.RedisSet):
                self.setWriter(obj)

        for pipe in self.pipes:
            if self.verbal:
                sys.stderr.write("Executing commands in the %s pipeline..." % pipe)
            self.pipes[pipe].execute()
            if self.verbal:
                sys.stderr.write("success!\n")
