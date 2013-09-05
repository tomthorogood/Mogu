
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
    def __init__(self, symbol_name=None, filename = None, initdict=None):
        if (initdict):
            self.symbol = initdict["symbol"]
            self.__dict__["definition"] = initdict["definition"]
            self.__dict__["references"] = initdict["references"]
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
