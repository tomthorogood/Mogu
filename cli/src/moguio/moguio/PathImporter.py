import FileImporter
import sys
import PythonObjectConverter
import SharedData
import PathResolver
import LineDisplay

from MoguString import MoguString

from Loggable import Loggable
from Loggable import LogMessage

from pyboro.src.Consumer import PyBoroSyntaxError

class RegistryError(Exception):
    def __init__(self, registry):
        self.label = registry.label
        self.symbols = registry.undefined()
        self.references = [registry[s].references for s in self.symbols]
        self.reg_info = str(registry)

    def __str__(self):
        strs = [str(s) for s in self.symbols]
        strs = [MoguString("integral",s) for s in strs]
        strs = [s.translate("script") for s in strs]
        output = self.reg_info
        for i,s in enumerate(strs):
            refs = self.references[i]
            refs_as_str = "\n\t".join(refs)
            output += \
                "\nSymbol %s undefined, found in:\n\t%s" % (s, refs_as_str)
        return output


class PathImporter(Loggable):
    def __init__(self, path, verbose=0):
        super(PathImporter, self).__init__(verbose)
        self.path_resolver = PathResolver.PathResolver(verbose)
        self.files = self.path_resolver.get_files(path)
        self.display = LineDisplay.LineDisplay()
        SharedData.verbose = verbose

    def display_progress(self, filename):
        # Get the "percentage complete" based on the current filename
        i = self.files.index(filename)
        p = int(( (i+1.0)/len(self.files)) * 100)

        # Build a message and update the last_update_len so it can be 
        # effectively cleared in the next round
        message = "Progress: %d%% (%s)" % (p, filename)
        self.display.clear().write(message)

    def get_results(self):
        results = []
        for f in self.files:
            try:
                self.display_progress(f)
                fi = FileImporter.FileImporter(f,self.verbose)
                results.extend(fi.get_results())
            except PyBoroSyntaxError as e:
                # For imports, don't print the entire traceback.
                # Instead, just print the syntax error.
                sys.exit(str(e))

        registries = SharedData.symbols.values()
        for r in registries:
            if not r:
                raise RegistryError(r)
            if r.nonreferenced():
                for sym in r.nonreferenced():
                    self.log(LogMessage(
                        "%s is defined but never referenced" % sym, 3), self.OUT)
        return results


    def flatten(self, container):
        self.log(LogMessage("Flatting container...",5,newline=False),self.OUT)
        flat = []
        for i in container:
            if isinstance(i,(list,tuple)):
                flat.extend(self.flatten(i))
            else:
                flat.append(i)
        self.log(LogMessage("DONE!",5,newline=True),self.OUT)
        return flat

    def convert_results(self, results):
        c = PythonObjectConverter.PythonObjectConverter(self.verbose)
        conversions = []
        for i,r in enumerate(results):
            self.display.clear().write(
                "Converting result %d/%d..." % (i+1,len(results)))
            conversions.append(c.convert(r))
        self.display.clear().write("\n")
        return self.flatten(conversions)
