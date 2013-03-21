from redis_cheats import *
from coloring import *

class MissingChildError(Exception):
    def __init__(self, widget_name):
        self.value = ("Cannot add child that doesn't exist! %s" % widget_name)
    def __str__(self):
        return self.value

class UnknownWidgetTypeError(Exception):
    def __init__(self, widget_name, widget_type):
        self.value = ("Widget %s is of unknown type %s" % widget_name, widget_type)

    def __str__(self):
        return self.value
class MissingArgumentError(Exception):
    def __init__(self, argument, other=None):
        self.value = "Missing CLI Argument: %s "
        self.value += "\n%s"%other

    def __str__(self):
        return self.value

class MoguImportException(Exception):
    def __init__(self, filename, error_message):
        self.value = "Unfortunately, there is something wrong with your Mogu import file %s." % filename
        self.value += "This is most likely not your fault. Please let us know of the error at " 
        self.value += "github.com/tomthorogood/mogu"
        self.value += "\n The error report was: \n"
        self.value += error_message
    def __str__(self):
        return self.value

class AmbiguousDirectiveError(Exception):
    def __init__(self, key, value):
        self.key = key
        self.value = value
    def __str__(self):
        value = ("Unable to successfully parse %s directive." %key)
        value += ("\nMust be of a specific type, which was not clearly defined in %s" % value)
        return value

class MissingParameterError(Exception):
    def __init__(self, widget_name, missing_parameter):
        self.value = ("Widget %s is missing parameter %s" % (widget_name, missing_parameter))

    def __str__(self):
        return self.value

class BadMoldDirectiveError(Exception):
    def __init__(self, parameter, mold):
        self.value = "The %s parameter is missing from %s" % (parameter, repr(mold))
    def __str__(self):
        return self.value

class AttributeNotFoundError(Exception):
    def __init__(self, node, attr):
        self.value = "%s is missing required node %s" % (node, attr)

    def __str__(self):
        return self.value

class AmbiguousDirectiveError(Exception):
    def __init__(self, node, key, val):
        self.value = "%s has a directive that Mogu cannot translate without some assistance: %s = %s" % (node, key, val)

    def __str__(self):
        return self.value
