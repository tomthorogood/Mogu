from markdown import markdown
from sets import Set
from lex_functions import everything_until
import re
import os

class Preprocessor(object):

    def __init__(self,verbose=False):
        self.verbose = verbose
        self.inputstream = None
        self.symbols = {}
        self.refs = []

    def log(self, *args):
        if self.verbose:
            msg = " ".join(args)
            print(msg)


    def filename_from_include(self,string):
        quoted = re.search("\".+\"",string)
        return quoted.group()[1:-1]

    def find_references(self):
        refs= Set(re.findall("@[a-zA-Z_:][a-zA-Z0-9:_]*", self.inputstream))
        refs = list(refs)
        refs.sort(lambda x,y: cmp(len(x), len(y)))
        refs.reverse()
        self.refs = refs

    def replace_contents(self,depr,repl):
        with open(repl) as f:
            text = f.read()
            self.log("Replacing", depr, "with", text)
            while (depr in self.inputstream):
                self.inputstream = self.inputstream.replace(depr, text)

    def find_declarations(self):
        regex = "<<<.*?>>>"
        declarations = re.findall(regex,self.inputstream, re.MULTILINE|re.DOTALL)
        self.log("Declarations found:", repr(declarations))
        return declarations

    def warn_if_not_found(self, symbol_ref):
        if symbol_ref not in self.symbols:
            raise ValueError(
                    "Symbol %s was referred to but never defined." %symbol_ref)

    def unpack_declaration(self, declaration):
        self.log("Unpacking Declaration:", declaration)
        id_group = re.search("<<<[a-zA-Z:_][a-zA-Z:_0-9]*", declaration)
        try:
            identifier = id_group.group()[3:]
        except AttributeError:
            raise AttributeError("File not found in %s" % declaration)
        self.log("IDENTIFIER:", identifier)
        id_excluded = declaration.replace(id_group.group(),"").strip()
        self.log("ID_EXCLUDED:",id_excluded)
        def_group = re.search(".*\n>>>",id_excluded, re.MULTILINE|re.DOTALL)
        definition = def_group.group().replace(">>>","").strip()
        self.log("DEFINITION:",definition)
        return identifier, definition

    def symbol_is_markdown(self,symbol):
        if "\n" in symbol:
            self.log("Markdown Found:\n", symbol)
            return True
        return False

    def process_inclusions(self,path):
        includes = re.findall("<<<@include \".+\">>>", self.inputstream)
        for include in includes:
            self.log("Processing Inclusion:", include)
            fname = self.filename_from_include(include)
            fpath = os.path.join(path,fname)
            if os.path.exists(fpath):
                self.replace_contents(include, fpath)

    def create_symbols(self):
        declarations = self.find_declarations()
        for declaration in declarations:
            self.log("REMOVING: ", declaration)
            self.inputstream = self.inputstream.replace(declaration,"")
            identifier, definition = self.unpack_declaration(declaration)
            if self.symbol_is_markdown(definition):
                definition = markdown(definition)
                definition = definition.replace("\n"," ")
                definition = definition.replace('"','\\"')
                self.log("Processed Markdown:", definition)
            self.symbols[identifier] = '"%s"' % definition
            self.log("Added symbol",identifier)

    def expand_symbols(self):
        for symbol in self.refs:
            error = False
            sym_name = symbol.replace("@","")
            self.log("Expanding", symbol)
            try:
                self.warn_if_not_found(sym_name)
            except ValueError as e:
                self.log("\nWARNING\n\t",str(e))
                error = True
            if error: 
                self.log("Not attempting to expand", sym_name)
                continue 
            while symbol in self.inputstream:
                self.inputstream = self.inputstream.replace(
                    symbol, self.symbols[sym_name])

    def strip_comments(self):
        comments = re.findall("^#.*",self.inputstream,re.MULTILINE)
        for comment in comments:
            self.log("Found Comment:",comment)
            while comment in self.inputstream:
                self.inputstream = self.inputstream.replace(comment,"")

    def processInput(self, inputstream, path):
        assert isinstance(inputstream,str), \
            "Expected string, %s found" % repr(type(inputstream))
        self.inputstream = inputstream
        self.process_inclusions(path)
        self.find_references()
        self.create_symbols()
        self.expand_symbols()
        self.strip_comments()
        self.inputstream = self.inputstream.replace("&quot;","")
        return self.inputstream.strip()

