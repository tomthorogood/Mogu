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

class Widget(KeyspaceExporter):
    def __init__(self, keyspace, db):
        super(Widget, self).__init__(db, "widgets", keyspace)
        self.has_events = self.db.exists("%s.events" % self.keyspace)
        self.has_children = self.db.exists("%s.children" % self.keyspace)
        self.attributes = Translators.ExportDict(db.hgetall(self.keyspace))

    def export(self):
        output = {
                "identifier"    :   self.identifier,
                "attributes"    :   str(self.attributes)
                }

        if (self.has_events):
            events = EventBlock(self.identifier, db)
            output["events"] = events.export()
        if (self.has_children):
            children_key = Keyspace.zipstring(self.keyspace, "children")
            num_children = db.llen(children_key)
            children = Translators.ExportList(db.lrange(children_key, 0, num_children), tabs=2)
            output["children"] = str(children)
        output = "widget %(identifier)s\n%(attributes)s\n%(events)s\n    children\n%(children)s\n    end children\nend widget" % output

        return output

class EventBlock(KeyspaceExporter):
    def __init__(self, keyspace, db):
        super(EventBlock, self).__init__(db, "widgets", keyspace, "events")
        self.triggers = [str(trigger) for trigger in db.lrange(
            self.keyspace, 0, db.llen(self.keyspace))]

    def get_when_blocks(self):
        when_blocks = []
        for trigger in self.triggers:
            when = WhenBlock(self.identifier, trigger, db)
            when_blocks.append(when.export())
        return when_blocks

    def export(self):
        output = {
                "when_blocks" : "\n".join(self.get_when_blocks())
                }
        output = "    events\n%(when_blocks)s\n    end events" % output
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

# TESTING
if __name__ == "__main__":
    from redis import Redis
    db = Redis()
    key = "test_widget"
    w = Widget(key,db)
    print(w)
