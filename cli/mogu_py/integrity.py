from coloring import *

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

