import ListWriter
import IndentTracker

class ChildrenWriter(object):
    """
        A ChildrenWriter requires a list of child
        names, and may optionally take the parent's name
        as well. Passing in the parent's name will 
        allow the child's name to be truncated if possible
        for more readable shorthand.

        If widgetfoo:childalpha is passed in, and 'widgetfoo'
        is identified as the parent, only ':childalpha' will be
        written in the output.
    """
    @staticmethod
    def truncate_parent(child,parent):
        keypath = child.split(':')
        try:
            pIndex = keypath.index(parent)
            return ":%s" % ":".join(keypath[pIndex+1:])
        except ValueError:
            return child

    def __init__(self, children, parentname=None):
        self.children = children
        self.parent = parentname

        if self.parent:
            self.children = [
                    ChildrenWriter.truncate_parent(child,self.parent) \
                            for child in self.children]
   
    def __str__(self):
        IndentTracker.INDENT_LEVEL += 1
        self.tabs = IndentTracker.TAB * IndentTracker.INDENT_LEVEL
        outer = \
"""
%(tabs)schildren
%(inner)s
%(tabs)send children
"""
        listWriter = ListWriter.ListWriter(self.children)
        self.inner = str(listWriter)

        output = outer % self.__dict__
        IndentTracker.INDENT_LEVEL -= 1
        return output

if __name__ == "__main__":
    testlist = ["widget:alpha","widget:beta","widget:gamma","widget:delta"]
    cw = ChildrenWriter(testlist,"widget")
    print(cw)
