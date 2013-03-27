import IndentTracker

class DictWriter(dict):
    def __str__(self):
        keys = self.keys()
        tab = IndentTracker.TAB * (IndentTracker.INDENT_LEVEL+1)
        maxlen = max([len(key) for key in keys])
        key_trailing = dict.fromkeys(keys,0);
        template = "%(tab)s%(key)s%(spaces)s : %(value)s"
        for entry in keys:
            l = len(entry)
            key_trailing[entry] = " " * (maxlen-l)

        formatted = []
        for key in keys:
            fdict = {
                    "tab"   :   tab,
                    "key"   :   key,
                    "spaces"    :   key_trailing[key],
                    "value" :   self[key]
                    }
            formatted.append(template % fdict)
        return "\n".join(formatted)

if __name__ == "__main__":
    testdict = {
            "alakazam"  :   "kalamazoo",
            "pert"      :   "plus",
            "fart"      :   "pttttthtphphphph",
            "i am the doctor, the oncoming storm" : "#10"
            }
    dw= DictWriter(testdict)
    print(dw)
