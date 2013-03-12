class UnimplError(Exception):
    def __init__(self, classname, method):
        self.value = "Realizing %s abstract class requires implementation of %s() method."

    def __str__(self):
        return self.value
