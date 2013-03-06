import Keyspace
import Translators
import sys

class Exports(object):
    def export(self):
        raise UnimplError("Exports", "export")

class RealizesKeyspace(object):
    def __init__(self, prefix, inputstr, *args):
        self.identifier = Keyspace.identifier(prefix,inputstr)
        self.keyspace = Keyspace.keyspace(prefix,inputstr, args)

class KeyspaceExporter(Exports, RealizesKeyspace):
    def __init__(self, db, prefix, inputstr, *args):
        super(KeyspaceExporter,self).__init__(prefix,inputstr,args)
        self.db = db

    def __str__(self):
        return self.export()

    def __repr__(self):
        return self.export()


class AbstractWidget(KeyspaceExporter):
    def __init__(self, keyspace, db, db_prefix, script_type):
        self.db_prefix = db_prefix
        self.script_type = script_type
        super(AbstractWidget, self).__init__(db, self.db_prefix, keyspace)
        self.has_events = self.db.exists("%s.events" % self.keyspace)
        self.has_children = self.db.exists("%s.children" % self.keyspace)
        self.attributes = Translators.ExportDict(db.hgetall(self.keyspace))

    def export(self):
        output = {
                "type"          :   self.script_type,
                "identifier"    :   self.identifier,
                "attributes"    :   str(self.attributes),
                "events"        :   "",
                "children"      :   ""
                }

        if (self.has_events):
            events = EventBlock(self.identifier, self.db)
            output["events"] = events.export()
        if (self.has_children):
            children_key = Keyspace.zipstring(self.keyspace, "children")
            num_children = self.db.llen(children_key)
            children = Translators.ExportList(self.db.lrange(children_key, 0, num_children), tabs=2)
            output["children"] = "    children\n%s\n    end children\n" % str(children)
        output = "\n%(type)s %(identifier)s\n%(attributes)s\n%(events)s%(children)send %(type)s\n" % output

        return output

class Widget(AbstractWidget):
    def __init__(self, keyspace, db):
        super(Widget, self).__init__(keyspace, db, "widgets", "widget")

class Template(AbstractWidget):
    def __init__(self,keyspace, db):
        super(Template, self).__init__(keyspace, db, "templates", "template")

class EventBlock(KeyspaceExporter):
    def __init__(self, keyspace, db):
        super(EventBlock, self).__init__(db, "widgets", keyspace, "events")
        self.triggers = [str(trigger) for trigger in db.lrange(
            self.keyspace, 0, db.llen(self.keyspace))]

    def get_when_blocks(self):
        when_blocks = []
        for trigger in self.triggers:
            when = WhenBlock(self.identifier, trigger, self.db)
            when_blocks.append(when.export())
        return when_blocks

    def export(self):
        output = {
                "when_blocks" : "\n".join(self.get_when_blocks())
                }
        output = "    events\n%(when_blocks)s\n    end events\n" % output
        return output

class WhenBlock(KeyspaceExporter):
    def __init__(self, keyspace, trigger, db):
        super(WhenBlock, self).__init__(db, "widgets", keyspace, "events", trigger)
        self.trigger = trigger
        self.commands = Translators.ExportList(
                self.db.lrange(self.keyspace, 0, self.db.llen(self.keyspace)), tabs=3)

    def export(self):
        output = {
                "trigger" : Translators.GenericTranslator.translate(self.trigger), 
                "commands": str(self.commands)
                }
        output = "        when %(trigger)s {\n%(commands)s\n        }" % output
        return output

class Validator(KeyspaceExporter):
    def __init__(self, keyspace, db):
        super(Validator, self).__init__(db, "validators", keyspace)
        self.attributes = Translators.ExportDict(self.db.hgetall(self.keyspace))

    def export(self):
        output = {
                "identifier" : self.identifier,
                "attributes" : str(self.attributes)
                }
        return "\nvalidator %(identifier)s\n%(attributes)s\nend validator\n" % output

class Policy(KeyspaceExporter):
    def __init__(self, keyspace, db):
        super(Policy, self).__init__(db, "policies", keyspace)
        self.attributes = Translators.ExportDict(self.db.hgetall(self.keyspace))

    def export(self):
        output = {
                "identifier" : self.identifier,
                "attributes" : str(self.attributes)
                }
        return "\npolicy %(identifier)s\n%(attributes)s\nend policy\n" % output

class ListData(KeyspaceExporter):
    def __init__(self, keyspace, db):
        super(ListData, self).__init__(db, "data", keyspace)
        self.data = Translators.ExportList(
                self.db.lrange(self.keyspace, 0, self.db.llen(self.keyspace)),tabs=2)

    def export(self):
        return str(self.data)

class HashData(KeyspaceExporter):
    def __init__(self, keyspace, db):
        super(HashData, self).__init__(db, "data", keyspace)
        self.data = Translators.ExportDict(
                self.db.hgetall(self.keyspace), tabs=2)

    def export(self):
        return str(self.data)

class Data(KeyspaceExporter):
    def __init__(self, keyspace, db):
        super(Data,self).__init__(db, "data", keyspace)
        self.datatype = self.db.type(self.keyspace)

    def export(self):
        output = {
                "type" : self.datatype,
                "identifier" : self.identifier,
                "wrap_start" : None,
                "wrap_end"   : None,
                "block"      : None
        }
        
        if self.datatype == "hash":
            data = HashData(self.keyspace, self.db)
            output["data"] = data.export()
        elif self.datatype == "list":
            data = ListData(self.keyspace, self.db)
            output["data"] = data.export()
        elif self.datatype == "string":
            del output['type']
            output['data'] = "%s%s" % ("    "*2, self.db.get(self.keyspace))
        elif self.datatype == "set":
            pass
        if "type" in output:
            output["wrap_start"] = output["type"]
            output["wrap_end"] = "end %s" % (output['type'])
        if output["wrap_start"]:
            tabs = "    "
            try:
                output["block"] = tabs + "%s\n%s\n" % (output["wrap_start"],output["data"])+ tabs + output["wrap_end"]
            except KeyError:
                print(output)
        else:
            output["block"] = output['data']
        return "\ndata %(identifier)s\n%(block)s\nend data\n" % output

# TESTING
if __name__ == "__main__":
    from redis import Redis
    db = Redis()
    key = "test_widget"
    w = Widget(key,db)
    print(w)

    key = "a_validator"
    v = Validator(key,db)
    print(v)

    key = "a_policy"
    p = Policy(key,db)
    print(p)

    key = "test_template"
    t = Template(key,db)
    print(t)

    key = "test_datalist"
    d = Data(key,db)
    print(d)
    key = "test_datahash"
    d = Data(key,db)
    print(d)
    key = "test_datavalue"
    d = Data(key,db)
    print(d)

