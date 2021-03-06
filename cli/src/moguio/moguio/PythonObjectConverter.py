import RedisObjects
from collections import OrderedDict
import syntax
import lex_base
import Lex
import pyboro
class PythonObjectConverter(object):
    """
    Each entry passed into this converter must be a tuple
    in the form of (OrderedDict, ParseMap), where
    OrderedDict will be the lexed symbols obtained from 
    the RootConsumer-valid block, and ParseMap will be 
    a reference to the actual ParseMap that lexed the block.
    """
    def __init__(self):
        pass

    def convert_to_redis_object(self, obj_name, obj):
        if isinstance(obj, dict):
            return RedisObjects.RedisHash(obj_name,obj)
        elif isinstance(obj, list):
            return RedisObjects.RedisList(obj_name,obj)
        elif isinstance(obj, (str,int,float)):
            return RedisObjects.RedisString(obj_name,obj)
        elif isinstance(obj, Set):
            return RedisObjects.RedisSet(obj_name,obj)
        else: 
            raise TypeError

    def convert(self, obj):

        lexed_results = obj[0]
        assert isinstance(lexed_results,OrderedDict), \
                "Type %s not an OrderedDict, which is required" % (str(type(lexed_results)))
        lex_map = obj[1]
        assert isinstance(lex_map, pyboro.Lexer.ParseMap), \
                "Type %s not a Pyboro ParseMap, which is required" % (str(type(lex_map)))
        if lex_map is Lex.WIDGET_BLOCK:
            return self.convert_widget(lexed_results)
        elif lex_map is Lex.TEMPLATE_BLOCK:
            return self.convert_widget(lexed_results, prefix="template")
        elif lex_map is Lex.POLICY_BLOCK:
            return self.convert_policy(lexed_results)
        elif lex_map is Lex.DATA_BLOCK:
            return self.convert_data(lexed_results)
        elif lex_map is Lex.VALIDATOR_BLOCK:
            return self.convert_validator(lexed_results)
        elif lex_map is Lex.PERSPECTIVE_BLOCK:
            return self.convert_perspective(lexed_results)
        else:
            raise TypeError

    def convert_data(self, result_dict):
        """
        The result_dict is the orderedDict instance returned from using
        the DATA_BLOCK.parse method, and contains the building blocks
        of a storage policy.
        """

        master_dict = OrderedDict()
        redis_objects = []
        identifier = result_dict["identifier"]
        master_key = "data.%s" % identifier
        params = result_dict["data_def"]
        # params will be a list of consumed blocks, however
        # this should only be a list of length 1 (one block of data)
        assert(len(params) == 1)
        lexed_data = params[0][0]
        lexed_data_type = params[0][1]
        if lexed_data_type is Lex.HASH_BLOCK:
            master_dict[master_key] = self.convert_hash_block(lexed_data)
        elif lexed_data_type is Lex.LIST_BLOCK:
            master_dict[master_key] = self.convert_list_block(lexed_data)
        elif lexed_data_type is Lex.VALUE_DEFINITION:
            master_dict[master_key] = lexed_data["value_def"]

        for entry in master_dict:
            redis_objects.append(self.convert_to_redis_object(entry, master_dict[entry]))
        return redis_objects 

    def convert_list_block(self, list_block):
        """
        The list block dictionary passed in will have one entry that is 
        relevant: 'list_def'.
        The value for this entry will be another list of lexed results, 
        each of which a tuple, the dict of which will have a single entry, 'entry'.
            { "list_def" : ({"entry": ...},LIST_DEFINITION)}
        """
        return_list = []
        definition = list_block["list_def"]
        for line in definition:
            return_list.append(line[0]['entry'])
        return return_list

    def convert_hash_block(self, hash_block):
        """
        The hash block dictionary passed in will have one entry that is relevant:
            "hash_def"
        The value for this entry will be another list of lexed results,
        each of which a tuple, the dict of which will have two entries:
            "key", and "value":

            {"hash_def" : ({"key": ..., "value":...}, HASH_DEFINITION)}
        """
        return_dict = OrderedDict()
        definition = hash_block["hash_def"]
        for line in definition:
            key = line[0]["key"]
            value = line[0]["value"]
            return_dict[key] = value
        return return_dict

    def convert_perspective(self, result_dict):
        identifier = result_dict["identifier"]
        node_name = "perspective.%s" % identifier
        event_list = []
        for result_tuple in result_dict["block"]:
            result = result_tuple[0]
            event = " ".join([val.strip() for val in result.values()])
            event_list.append(event)
        return self.convert_to_redis_object(node_name, event_list)


    def convert_validator(self, result_dict):
        """
        The result_dict is the OrderedDict instance returned from using
        the VALIDATOR_BLOCK.parse method, and contains the building blocks
        of a validator.
        """
        master_dict = OrderedDict()
        redis_objects = []
        identifier = result_dict["identifier"]
        master_key = "validator.%s" % identifier
        master_dict[master_key] = OrderedDict()
        params = result_dict["validator_def"]
        for param in params:
            o_dict = param[0]
            parsemap = param[1]
            if parsemap is lex_base.VALIDATOR_TEST:
                master_dict[master_key][syntax.as_integer("test")] = o_dict["test"]
            elif parsemap is lex_base.VALIDATOR_TYPE:
                master_dict[master_key][syntax.as_integer("type")] = o_dict["type"]
        
        for entry in master_dict:
            redis_objects.append(self.convert_to_redis_object(entry, master_dict[entry]))
        return redis_objects 


    def convert_policy(self, result_dict):
        """
        The result_dict is the OrderedDict instance returned from using
        the POLICY_BLOCK.parse method, and contains the building blocks
        of a storage policy.
        """

        def init_default(master,key):
            if key not in master:
                master[key] = OrderedDict()

        master_dict = OrderedDict()
        redis_objects = []
        identifier = result_dict["identifier"]
        master_key = "policy.%s" % identifier
        default_key = "%s.%d" % (master_key, syntax.as_integer("default"))
        master_dict[master_key] = OrderedDict()
        params = result_dict["policy_def"]

        for param in params:
            o_dict = param[0]
            parsemap = param[1]
            if parsemap is lex_base.POLICY_MODE:
                master_dict[master_key][syntax.as_integer("mode")] = \
                        o_dict["mode"]
            elif parsemap is lex_base.POLICY_DATA:
                master_dict[master_key][syntax.as_integer("type")] = \
                        o_dict["datatype"]
            elif parsemap is lex_base.POLICY_ENCRYPTION:
                master_dict[master_key][syntax.as_integer("encrypted")] = \
                        o_dict["encrypted"]
            elif parsemap is lex_base.POLICY_DEFAULT:
                init_default(master_dict,default_key)
                master_dict[default_key] = o_dict["default"]
            elif parsemap is Lex.HASH_BLOCK:
                init_default(master_dict,default_key)
                master_dict[default_key] = self.convert_hash_block(o_dict)
            elif parsemap is Lex.LIST_BLOCK:
                init_default(master_dict, default_key)
                master_dict[default_key] = self.convert_list_block(o_dict)

        for entry in master_dict:
            redis_objects.append(
                self.convert_to_redis_object(entry, master_dict[entry]))
        return redis_objects 
        
    def convert_widget(self, result_dict, prefix="widget"):
        """
        The result_dict is the OrderedDict instance returned from using 
        the WIDGET_BLOCK.parse method, and contains all of the building
        blocks of a widget.
        """
        master_dict = OrderedDict()
        redis_objects = []

        identifier = result_dict["identifier"]
        master_key = "%s.%s" % (prefix,identifier)
        master_dict[master_key] = OrderedDict()

        # Contains information about the widget, and
        # is a list of other result dicts.
        key = "widget_def" if prefix == "widget" else "template_def"
        params = result_dict[key]
        for param in params:
            o_dict = param[0]
            parsemap = param[1]

            if parsemap is lex_base.WIDGET_TYPE:
                master_dict[master_key][syntax.as_integer("type")] = o_dict["type"]
            elif parsemap is lex_base.WIDGET_CONTENT:
                master_dict[master_key][syntax.as_integer("content")] = o_dict["content"]
            elif parsemap is lex_base.WIDGET_SOURCE:
                master_dict[master_key][syntax.as_integer("source")] = o_dict["source"]
            elif parsemap is lex_base.WIDGET_SORT:
                master_dict[master_key][syntax.as_integer("sort")] = o_dict["declaration"]
            elif parsemap is lex_base.WIDGET_LOCATION:
                master_dict[master_key][syntax.as_integer("location")] = o_dict["location"]
            elif parsemap is lex_base.WIDGET_TEMPLATE:
                master_dict[master_key][syntax.as_integer("template")] = o_dict["template"]
            elif parsemap is lex_base.WIDGET_STYLE:
                master_dict[master_key][syntax.as_integer("css")] = o_dict["css_classes"]
            elif parsemap is lex_base.WIDGET_TOOLTIP:
                master_dict[master_key][syntax.as_integer("tooltip")] = o_dict["tooltip"]
            elif parsemap is Lex.EVENT_BLOCK:
                event_o_dict = self.convert_events(master_key, o_dict["block"])
                master_dict.update(event_o_dict)
            elif parsemap is Lex.WIDGET_VALIDATOR:
                master_dict[master_key][syntax.as_integer("validator")] = o_dict["validator"]
            elif parsemap is lex_base.CHILDREN_BLOCK:
                master_dict["%s.children" % master_key] = [val for val in o_dict['block'] if val]
        for entry in master_dict:
            redis_objects.append(self.convert_to_redis_object(entry, master_dict[entry]))
        return redis_objects 

    def convert_events(self, prefix, result_list):
        """
        result_list will be a list of (OrderedDict,ParseMap) tuples. Each OrderedDict
        in the list will contain a 'trigger' and a 'block'.
        'block' will be another list, this time of MOGU_CMD lexed objects, which will 
        have to be glued back together.
        
        The goal is to return
        an OrderedDict in the following format:
            {"widget.[identifier].events.[trigger]" : [cmd A ... cmd Z], }
        """
        event_dict = OrderedDict()
        triggers = []
        for result_tuple in result_list:
            result = result_tuple[0]
            triggers.append(result['trigger'])
            key = "%s.events.%d" % (prefix, int(result['trigger']))
            when_commands = []
            block = result['block']
            for line in block:
                command_dict = line[0]
                command = " ".join([str(value) for value in command_dict.values()])
                when_commands.append(command)
            event_dict[key] = when_commands
        event_dict["%s.events" % prefix] = triggers
        return event_dict
