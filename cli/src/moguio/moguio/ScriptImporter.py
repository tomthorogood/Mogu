import pyboro
import PathImporter
import PythonObjectConverter
import RedisObjects
import RedisWriter
import SharedData
import SymbolRegistry
import sys
import __main__

class ScriptImporter(object):
    class RegistryError(Exception):
        def __init__(self, registry):
            self.registry = registry

        def __str__(self):
            return "Cannot continue: %s has undefined symbols: %s" % \
                (self.registry.label, str(self.registry.undefined()))

    def __init__(self, paths, args=None):
        self.objectConverter = PythonObjectConverter.PythonObjectConverter()
        self.redisWriter = RedisWriter.RedisWriter(args)
        self.results = []
        self.redisObjects = []
        if args:
            if hasattr(args,"v"):
                pyboro.Lexer.VERBAL = args.v
                SharedData.VERBAL = args.v
            elif hasattr(args,"verbal"):
                pyboro.Lexer.VERBAL = args.verbal
                SharedData.VERBAL = args.verbal

        # Force the 'paths' into a list format.
        if isinstance(paths,tuple):
            paths = list(paths)
        elif isinstance(paths,str):
            paths = [paths,]
        elif not isinstance(paths,list):
            raise TypeError("Paths must be in list, tuple, or string format")
        num_paths = len(paths)
        # Lex the files in each of the paths. 
        for i,path in enumerate(paths):
            self.results.extend(PathImporter.import_path(path))

        # Determine if any references have unmet dependencies
        for registry in SymbolRegistry.registries:
            if not registry:
                raise ScriptImporter.RegistryError(registry)

        # Raise a warning if there are unreferenced symbols:
        for registry in SymbolRegistry.registries:
            if registry.nonreferenced():
                for symbol in registry.nonreferenced():
                    sys.stderr.write(
                            "WARNING: %s is defined but never referenced\n" % \
                            (symbol)) 
    def write(self): 
        for result in self.results:
            self.redisObjects.extend(self.objectConverter.convert(result))

        self.redisWriter.write(self.redisObjects)

def doImport(args):
    # args.command[0] will be 'import'
    try:
        assert args.command[0] == 'import'
    except AssertionError:
        sys.stderr.write("Something happened. I was told to import, but not by you. This should probably be reported.\n")
        sys.stderr.write("Please report the following output: %s" % (str(args)))
        sys.exit()

    importer = ScriptImporter(args.command[1:],args)
    importer.write()

if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser("Test the ScriptImporter")
    parser.add_argument(
            "-v", dest="verbal", action="store_true",default=False)
    parser.add_argument(
            nargs="+",dest="paths")
    parser.add_argument(
            "--flush", dest="flushdb", action="store_true",default=False)
    parser.add_argument(
            "--dbconfig", dest="dbconfig", default=None)
    args = parser.parse_args()
    print(args.paths)
    importer = ScriptImporter(args.paths,args)
    importer.write()
elif not hasattr(__main__,"__file__"):
    class Collection(object):
        def __init__(self):
            pass

    args = Collection()
    args.verbal = False
    args.paths="/home/tom/dev/FinancialFirsts/newmogu/widgets"
    args.flushdb=True
    args.dbconfig=None
    importer = ScriptImporter(args.paths,args)
