import ListWriter
import IndentTracker

class TriggerWriter(object):
    """
        The TriggerWriter requires a trigger and
        a list of commands, it will generate a When block
        with the following template:

            when %(trigger)s {
                %(command)s
                [%(command)s...]
            }
    """
    def __init__(self, trigger, cmdlist):
        self.trigger = trigger
        self.commands = cmdlist

    def __str__(self):
        IndentTracker.INDENT_LEVEL += 1
        self.tab = IndentTracker.TAB * IndentTracker.INDENT_LEVEL
        outer = \
"""
%(tab)swhen %(trigger)s {
%(inner)s
%(tab)s}
"""
        lw = ListWriter.ListWriter(self.commands)
        self.inner = str(lw)
        IndentTracker.INDENT_LEVEL -= 1
        return outer % self.__dict__

if __name__ == "__main__":
    tw = TriggerWriter("clicked",
            ["do something special", "do something else", "do one more thing"]
            )
    print(tw)
