import TriggerWriter
import IndentTracker

class EventWriter(object):
    """
        An EventWriter requires a dictionary of 
        'trigger' : [command, ...] 
        entries, which it will turn into a standard
        events block.
    """

    def __init__(self, eventdict):
        self.eventdict = eventdict

    def __str__(self):
        IndentTracker.INDENT_LEVEL += 1
        self.tab = IndentTracker.TAB * IndentTracker.INDENT_LEVEL
        outer = \
"""
%(tab)sevents
%(inner)s
%(tab)send events
"""
        self.inner = "" 
        for trigger in self.eventdict:
            tw = TriggerWriter.TriggerWriter(trigger,self.eventdict[trigger])
            self.inner += str(tw)
        IndentTracker.INDENT_LEVEL -= 1

        return outer % self.__dict__

if __name__ == "__main__":
    testdict = {
            "clicked"   :   ["do a clicky thing", "do another cicky thing"],
            "succeeded test"    :   ["give positive feedback"],
            "failed test"       :   ["give negative feedback"],
            }
    ew = EventWriter(testdict)
    print(ew)
