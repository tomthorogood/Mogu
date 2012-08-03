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
