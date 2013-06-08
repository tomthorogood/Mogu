import IndentTracker
from DictWriter import DictWriter

class AttributeWriter(object):
    """
    An AttributeWriter can take any 'object type',
    provided the object does not have any nested 
    inner blocks, and only has key->value attributes.
    """
    def __init__(self, objname, identifier, attrdict):
        self.name = objname
        self.identifier = identifier
        self.attributes = attrdict

    def __str__(self):
        attr_str = DictWriter(self.attributes)
        self.tab = IndentTracker.TAB * IndentTracker.INDENT_LEVEL
        self.inner = str(attr_str)
        outer =\
"""
%(tab)s%(name)s %(identifier)s
%(inner)s
%(tab)send %(name)s
""" % self.__dict__
        return outer

    def setAttr(self,key,value):
        self.attributes[key] = value
        

class PolicyWriter(AttributeWriter):
    def __init__(self,identifier,attrdict):
        super(PolicyWriter,self).__init__('policy',identifier,attrdict)

class ValidatorWriter(AttributeWriter):
    def __init__(self,identifier,attrdict):
        super(ValidatorWriter,self).__init__('validator',identifier,attrdict)

if __name__ == "__main__":
    validator = AttributeWriter('validator','testvalid',{
        'test'  :   'regexexp',
        'type'  :   'regex'
    })
    validator.setAttr('comment','something something something dark side')
    print(validator)
