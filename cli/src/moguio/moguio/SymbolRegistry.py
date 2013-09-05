from SymbolReferenceTracker import SymbolReferenceTracker

"""
This file contains the necessary classes to keep track of symbols
across multiple files during the import process. During the import
process, whenever a new object is defined, the importer will add
the symbol to the regstiry of its object type.

If the symbol is already defined (for instance, if you have two
widgets named 'foo'), an error will be raised. Anytime the symbol
is encountered during the import process, a reference to that
symbol will be added.

At the end of the import process, before writing to the database, 
Mogu will check to ensure that all symbols referenced are defined.
In addition, it will raise warnings if defined symbols are never 
referenced.
"""



class SymbolRegistry(object):
    def __init__(self, label=None, initdict=None):
        self.symbols = initdict if initdict else {}
        self.label = label

    def __len__(self):
        return len(self.symbols)

    def __contains__(self, symbol):
        return symbol in self.symbols

    def __iter__(self):
        for x in self.symbols:
            yield x

    def __str__(self):
        label = self.label if self.label else "Anonymous"
        return "%s SymbolRegistry containing %d symbols" % (label,len(self))

    def __repr__(self):
        label = "\"%s\"" % (self.label) if self.label else "None"
        out = "SymbolRegistry(label=%s,dict={" % label
        reprs = []
        for symbol in self:
            reprs.append("\"%s\" : %r" % (symbol, self[symbol]))
        out += ", ".join(reprs) + "})"
        return out

    def __setitem__(self, key, val):
        """
        Assigns a new symbol to the registry. If
            registryObject["foo"] = "bar"
        is called, the registry will create a new SymbolRefernceTracker,
        where "foo" is the symbol, and "bar" is the filename in which 
        the symbol was defined. If the symbol name is already in the registry,
        but was not defined, it will thus be defined.

        If the symbol name is already in the regstiry and it is defined, then
        the reference tracker itself will raise an error.

        Alternatively, you may also directly assign a SymbolReferenceTracker:
            registryObject["foo"] = SymbolReferenceTracker("bar","baz")
        """
        if isinstance(key,(str,unicode)) and isinstance(val,(str,unicode)):
            if key in self:
                self.symbols[key].define(val)
            else:
                self.symbols[key] = SymbolReferenceTracker(key,val)
        elif isinstance(val,SymbolReferenceTracker) and key not in self:
            self.symbols[key] = val
        else:
            raise TypeError("%s%s"%(key,val))

    def __getitem__(self, key):
        if key not in self:
            self.symbols[key] = SymbolReferenceTracker(key)
        return self.symbols[key]

    def __nonzero__(self):
        """
        In order for a Registry to be considered "true", all of its
        components must also be true. 
        That is to say: all symbols must be defined. 
        """
        return False if self.undefined() else True

    def undefined(self):
        return [key for key in self if not self[key]]

    def reference(self, key, reference):
        """
        Mogu allows for symbols to be referenced before they are defined. 
        Therefore if a key does not already exist in the table, we can 
        add it here.
        """
        if key not in self:
            self[key] = SymbolReferenceTracker(key)
        self[key].append(reference)

    def nonreferenced(self):
        """
        Returns a list of all symbols that are defined, but never referenced.
        """
        nonrefs = []
        for key in self:
            if len(self[key]) == 0:
                nonrefs.append(key)
        return nonrefs

# TESTING #
if __name__ == "__main__":
    import random
    import string
    def randomfilename():
        name = "".join([random.choice(string.letters) for i in range(10)])

        return name + ".mogu"
    symbols = (
            "singular_symbol_a",
            "singular_symbol_b",
            "singular_symyol_c",
            "repeat_symbol"
            )
    testRegistry = SymbolRegistry()
    for symbol in symbols:
        testRegistry[symbol] = randomfilename()
        for i in range(3):
            testRegistry.reference(symbol, randomfilename())

    bad = symbols[-1]
    try:
        testRegistry[bad] = randomfilename()
    except Exception as e:
        raise e
    print(testRegistry)
    print(repr(testRegistry))
