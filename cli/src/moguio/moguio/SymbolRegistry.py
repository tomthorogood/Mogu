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

class SymbolReferenceTracker(object):
    """
    The SymbolReferenceTracker tracks both references to a symbol and
    places where the symbol is defined. As a rule, a symbol may only be
    defined in exactly one place. Therefore, after a symbol is defined, 
    it's definition cannot be changed. 

    You may instantiate the reference tracker without a definition. Doing so
    will return "False" on all boolean tests of the reference tracker. The only
    way to assign to the definition is with the define() method, which will raise
    an AttributeError if that is already defined.

    An AttributeError will also be raised if anything attemps to overwrite
    the definition directly using the "obj.definition = " assignment.

    The definition may be accessed at any time for reading, however.

    References to the symbol may be modified, but the references attribute 
    itself may not be reassigned. In addition, calling the 'append' method
    on this object will append to the list of references, and iterating over
    this object will iterate over its references.
    """
    def __init__(self, symbol_name=None, filename = None, dict=None):
        if (dict):
            self.symbol = dict["symbol"]
            self.__dict__["definition"] = dict["definition"]
            self.__dict__["references"] = dict["references"]
        elif not symbol_name:
            raise AttributeError("dict or symbol name required")
        else:
            self.symbol = symbol_name 
            self.__dict__["definition"] = None
            if filename is not None:
                self.define(filename)
            self.__dict__["references"] = []

    def __str__(self):
        if self.definition:
            return "SymbolReferenceTracker for %s defined in %s, with %d references" % (
                    self.symbol, self.definition, len(self))

        else:
            return "Undefined symbol %s with %d references" % (self.symbol, len(self))

    def __repr__(self):
        return "SymbolReferenceTracker(dict={\"symbol\" : \"%s\", \"definition\" : \"%s\" , \"references\" : %s})"\
            % (self.symbol, self.definition, repr(self.references))
    
    def __setattr__(self,name,val):
        if name not in ("definition", "references"):
            self.__dict__[name] = val
        else:
            raise AttributeError

    def __eq__(self, other):
        if isinstance(other,str):
            return self.symbol == other
        elif isinstance(other, SymbolReferenceTracker):
            return self.symbol == other.symbol
    
    def __ne__(self, val):
        if isinstance(val,str):
            return self.symbol != val

    def __nonzero__(self):
        return (self.symbol is not None and self.definition is not None)

    def __len__(self):
        return len(self.references)

    def __iter__(self):
        for x in self.references:
            yield x

    def __contains__(self, other):
        return other in self.references

    def __iadd__(self, val):
        self.references.append(val)
   
    def define(self,filename):
        """
        Mogu symbols must be defined in mogu files, otherwise
        chaos could ensue. This is an added check to ensure the
        integrity of the symbol table.
        """
        if not self.definition:
            if not filename.endswith(".mogu"):
                raise TypeError("Attempted to define %s in non-mogu file %s" % (self.symbol, filename))
            self.__dict__["definition"] = filename
        else:
            raise AttributeError("Cannot redefine symbol %s in %s - Currently defined in %s" % (
                self.symbol, filename, self.definition))

    def append(self, other):
        self.references.append(other)


class SymbolRegistry(object):
    def __init__(self, label=None, dict=None):
        if not dict:
            self.symbols = {}
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
            reprs.append("\"%s\" : %s" % (symbol, repr(self[symbol])))
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
        for key in self:
            if not self[key]:
                return False
        return True

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

widgetRegistry      = SymbolRegistry("widgets")
templateRegistry    = SymbolRegistry("templates")
dataRegistry        = SymbolRegistry("data")
validatorRegistry   = SymbolRegistry("validators")
policyRegistry      = SymbolRegistry("policies")

registries = (
    widgetRegistry,
    templateRegistry,
    dataRegistry,
    validatorRegistry,
    policyRegistry
)

# TESTING #
if __name__ == "__main__":
    import random
    import string
    def randomfilename():
        name = ""
        for i in range(10):
            name += random.choice(string.letters)
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
