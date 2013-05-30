import redis
from moguio import Keyspace
from moguio import ScriptWriter

class DatabaseExporter(object):
    EXPORT_KEYSPACES = (
            "widgets",
            "templates",
            "validators",
            "data",
            "policies"
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
            
            if datatype == 'widgets':
                container.append(ScriptWriter.WidgetWriter(
                    identifier,attributes,children=children,events=events)
                )
            elif datatype == 'templates':
                container.append(ScriptWriter.TemplateWriter(
                    identifier,attributes,children=children,events=events)
                )
        if callback:
            callback(container)

    def export_widgets(self,callback=None):
        self.widgetWriters = []
        self.export_abstract_widget('widgets',self.widgetWriters,callback)

    def export_templates(self, callback=None):
        self.templateWriters = []
        self.export_abstract_widget('templates',self.templateWriters,callback)

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
        if callback:
            callback(self.dataWriters)

    def export_validators(self,callback=None):
        self.validatorWriters = []
        for validator in self.roots['validators']:
            identifier = Keyspace.identifier('validators',validator)
            attrs = self.db.hgetall(validator)
            self.validatorWriters.append(ScriptWriter.ValidatorWriter(identifier,attrs))
        if callback:
            callback(self.validatorWriters)

    def export_policies(self,callback=None):
        self.policyWriters = []
        for policy in self.roots['policies']:
            identifier = Keyspace.identifier('policies',policy)
            attrs = self.db.hgetall(policy)
            self.policyWriters.append(ScriptWriter.PolicyWriter(identifier,attrs))
        if callback:
            callback(self.policyWriters)

