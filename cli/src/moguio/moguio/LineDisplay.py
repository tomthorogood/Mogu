import sys

class LineDisplay(object):
    def __init__(self):
        self.last_len = 0

    def clear(self):
        sys.stdout.write("\r%s" % (" " * self.last_len))
        sys.stdout.flush()
        return self

    def write(self, message):
        self.last_len = len(message)
        sys.stdout.write("\r%s" % message)
        sys.stdout.flush()
        return self
