import IndentTracker

class ListWriter(list):
    def __str__(self):
        indent = IndentTracker.INDENT_LEVEL + 1
        tab = IndentTracker.TAB * indent
        output = ""
        template = "%(tab)s%(entry)s"
        formatted = [template % {"entry":entry,"tab" :tab} for entry in self]
        return "\n".join(formatted)

if __name__ == "__main__":
    testlist = ["alpha","beta","gamma","delta"]
    lw = ListWriter(testlist)
    print(lw)
