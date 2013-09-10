import sys

class LogMessage(object):
    def __init__(self, message, level, newline=True):
        self.message = message
        self.level = level
        self.newline = ("\n" * int(newline)) or ""

    def build(self, prefix):
        return "%s: %s%s" % (prefix, self.message, self.newline)

class Loggable(object):
    streams = {0 : sys.stdout, 1 : sys.stderr }
    OUT = 0
    ERR = 1
    ERR_MESSAGES = (
        "Stream object %r has no method \"write\"; logging cannot continue.",
        "Stream object %r is not a known output stream.",
        "Message object %r is not an instance of LogMessage") 
    
    def __init__(self, verbose=False):
        self.verbose = verbose
        self.levels = ("CRITICAL", "ERROR", "WARNING", "NOTICE", "DEBUG")

    def get_stream(self, stream):
        if stream in Loggable.streams:
            return Loggable.streams[stream]
        elif hasattr(stream,"write"):
            return stream

        raise TypeError(Loggable.ERR_MESSAGES[1] % stream)

    def log(self, message, stream):
        if not isinstance(message, LogMessage):
            raise TypeError(Loggable.ERR_MESSAGES[2] % message)
        
        if not self.verbose or message.level > self.verbose:
            return
        
        stream = self.get_stream(stream)
        # The list is indexed from 0, but the v-level starts at 1
        level_prefix = message.level-1
        stream.write(message.build(self.levels[level_prefix]))

