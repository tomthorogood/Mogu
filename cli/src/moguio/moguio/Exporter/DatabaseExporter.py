import redis
from moguio import Keyspace
from moguio import ScriptWriter
from moguio.MoguString import MoguString

class DatabaseExporter(object):
    EXPORT_KEYSPACES = (
            "widget",
            "template",
            "validator",
            "data",
            "policy",
            "perspective",
            "meta"
    )

    @staticmethod
    def root_object(string):
        tokens = string.split('.')
        if len(tokens) == 2 and tokens[0] in DatabaseExporter.EXPORT_KEYSPACES:
            return True
        return False

    def __init__(
            self,
            redis_db_num=0,
            redis_host='localhost',
            redis_port=6379
            ):
        # Connect to the proper Redis database
        self.db = redis.Redis(db=redis_db_num,host=redis_host,port=redis_port)

        # Create a list of all root objects in the database, meaning those
        # that have a keyspace prefix and an identifier only
        roots = filter(DatabaseExporter.root_object, self.db.keys('*'))

        # Create a dictionary for each of the keyspaces, and populate the
        # dictionary with the root objects for each prefix
        self.roots = dict.fromkeys(DatabaseExporter.EXPORT_KEYSPACES,None)
        for root in self.roots:
            self.roots[root] = []
        for root in roots:
            prefix = root.split('.')[0]
            self.roots[prefix].append(root)

    def convert_output(self,container):
        self.scripts = []
        for formatter in container:
            mogustring = MoguString("integral", str(formatter))
            mogustring.translate("script")
            self.scripts.append(mogustring.script)

    def export_abstract_widget(self,datatype,container,callback=None):
        container = []
        for widget in self.roots[datatype]:
            identifier = Keyspace.identifier(datatype,widget)
            events = Keyspace.zipstring(datatype,identifier,'events')
            children = Keyspace.zipstring(datatype,identifier,'children')
            
            has_events = self.db.exists(events)
            has_children = self.db.exists(children)
            if has_events:
                triggerlist = self.db.lrange(events,0,self.db.llen(events))
                eventdict = {}
                for trigger in triggerlist:
                    commandlist = Keyspace.zipstring(datatype,identifier,'events',trigger)
                    eventdict[trigger] = self.db.lrange(commandlist,0,self.db.llen(commandlist))
                events = eventdict
            else:
                events = None
            if has_children:
                childlist = self.db.lrange(children,0,self.db.llen(children))
                children = childlist
            else:
                children = None

            attributes = self.db.hgetall(widget)
            
            if datatype == 'widget':
                container.append(ScriptWriter.WidgetWriter(
                    identifier,attributes,children=children,events=events)
                )
            elif datatype == 'template':
                container.append(ScriptWriter.TemplateWriter(
                    identifier,attributes,children=children,events=events)
                )

        self.convert_output(container)
        if callback:
            return callback(self.scripts)
        else:
            return self.scripts

    def export_widgets(self,callback=None):
        self.widgetWriters = []
        return self.export_abstract_widget('widget',self.widgetWriters,callback)

    def export_templates(self, callback=None):
        self.templateWriters = []
        return self.export_abstract_widget('template',self.templateWriters,callback)

    def export_data(self, callback=None):
        self.dataWriters = []
        for data in self.roots['data']:
            identifier = Keyspace.identifier('data',data)
            redistype = self.db.type(data)
            data_object = None
            if redistype == 'string':
                data_object = self.db.get(data)
            elif redistype == 'hash':
                data_object = self.db.hgetall(data)
            elif redistype == 'list':
                data_object = self.db.lrange(data,0,self.db.llen(data))
            self.dataWriters.append(ScriptWriter.DataWriter(identifier,data_object))

        self.convert_output(self.dataWriters)
        if callback:
            return callback(self.scripts)
        else:
            return self.scripts

    def export_validators(self,callback=None):
        self.validatorWriters = []
        for validator in self.roots['validator']:
            identifier = Keyspace.identifier('validator',validator)
            attrs = self.db.hgetall(validator)
            self.validatorWriters.append(ScriptWriter.ValidatorWriter(identifier,attrs))
        self.convert_output(self.validatorWriters)
        if callback:
            return callback(self.scripts)
        else:
            return self.scripts

    def export_policies(self,callback=None):
        self.policyWriters = []
        for policy in self.roots['policy']:
            identifier = Keyspace.identifier('policy',policy)
            attrs = self.db.hgetall(policy)
            self.policyWriters.append(ScriptWriter.PolicyWriter(identifier,attrs))
        self.convert_output(self.policyWriters)
        if callback:
            return callback(self.scripts)
        else:
            return self.scripts

