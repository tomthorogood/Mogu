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
