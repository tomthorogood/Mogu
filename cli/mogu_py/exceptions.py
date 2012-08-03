class MissingArgumentError(Exception):
    def __init__(self, argument, other=None):
        self.value = "Missing CLI Argument: %s "
        self.value += "\n%s"%other

    def __str__(self):
        return self.value
