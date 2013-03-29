import IndentTracker
from ListWriter import ListWriter
from DictWriter import DictWriter

class DataWriter(object):
    """
    A DataWriter requires an identifier and a data object,
    which can be one of either a string, int, float, list, or dict
    """
    def __init__(self, identifier, dataobj):
        self.identifier = identifier
        self.data = dataobj

    def __str__(self):
        outer = \
"""
%(tab)sdata %(identifier)s
%(inner)s
%(tab)send data
"""
        self.tab = IndentTracker.TAB * IndentTracker.INDENT_LEVEL
        IndentTracker.INDENT_LEVEL +=1
        localtab = (IndentTracker.TAB * IndentTracker.INDENT_LEVEL)

        if isinstance(self.data,(int,str,float)):
            self.inner = localtab 
            self.inner += str(self.data)

        elif isinstance(self.data,list):
            lw = ListWriter(self.data)
            self.inner = localtab + "list\n"
            self.inner += str(lw)
            self.inner += "\n%send list" % localtab

        elif isinstance(self.data,dict):
            dw = DictWriter(self.data)
            self.inner = localtab + "hash\n"
            self.inner += str(dw)
            self.inner += "\n%send hash" % localtab

        IndentTracker.INDENT_LEVEL -= 1
        return outer % self.__dict__

if __name__ == "__main__":
    listData = DataWriter("listtest", ["alpha","beta","gamma","delta"])
    dictData = DataWriter("dicttest", {"alpha" : "A", "beta" : "B"})
    strData = DataWriter("strtest","Here is a string")
    intData = DataWriter("inttest", "42")
    fltData = DataWriter("flttest", 4815.162342)

    print(listData)
    print(dictData)
    print(strData)
    print(intData)
    print(fltData)
