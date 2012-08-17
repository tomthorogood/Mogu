class AttributeNotFoundError(Exception):
    def __init__(self, node, attr):
        self.value = "%s is missing required node %s" % (node, attr)

    def __str__(self):
        return self.value
