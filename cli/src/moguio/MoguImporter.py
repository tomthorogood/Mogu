from collections import OrderedDict
from sets import Set

class Importer(object):
    def __init__(self, result_dict={}):
        if not isinstance(result_dict, (OrderedDict,dict)):
            raise TypeError
        self.__unzip__(result_dict)

    def __setattr__(self,key,val):
        if hasattr(self,key):
            raise KeyError
        else:
            self.__dict__[key] = val

    def __unzip__(self,dictObj):
        for key in dictObj:
            if not isinstance(dictObj[key],(OrderedDict,dict)):
                self.__setattr__(key,dictObj[key])
            else:
                self.__unzip__(dictObj[key])

def __test__():    
    from WIDGET_BLOCK import WIDGET_BLOCK
    from RootConsumer import RootConsumer
    results = []
    test = "syntax_test.mogu"
    f = open(test,"r")
    parsed = RootConsumer.parse(f.read())
    for entry in parsed:
        if entry[1] is WIDGET_BLOCK:
            results.append(Importer(entry[0]))
    return results
    
