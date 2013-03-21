import PathImporter
import FileImporter
import SymbolRegistry
import PythonObjectConverter
import RedisWriter
import sys
import MoguExporter
import redis
import re


def display_undefined_symbols(registry):
    for symbol in registry:
        if not registry[symbol]:
            sys.stderr.write("\n")
            sys.stderr.write(str(registry[symbol]))
            sys.stderr.write("\n\tReferenced in: \n")
            for reference in registry[symbol]:
                sys.stderr.write("\t\t%s\n" % reference)

def mogu_import(args):
    results = []
    write = not args.testing
    for path in args.command[1:]:
        if path.endswith(".mogu"):
            results.extend(FileImporter.import_file(path, args.v))
        else:
            results.extend(PathImporter.import_path(path, args.v))
    converter = PythonObjectConverter.PythonObjectConverter()
    redis_objects = []
    for result in results:
        redis_objects.extend(converter.convert(result))
    
    for registry in [
            SymbolRegistry.widgetRegistry, 
            SymbolRegistry.templateRegistry,
            SymbolRegistry.dataRegistry,
            SymbolRegistry.validatorRegistry,
            SymbolRegistry.policyRegistry
            ]:
        if not registry:
            display_undefined_symbols(registry)
            sys.stderr.write("\n== REFUSING TO CONTINUE ==\n")
            sys.exit()
        if registry.nonreferenced():
            i = None
            sys.stderr.write("\n== WARNING: %s contains the following symbols that are defined but never referenced == \n" %
                    registry.label)
            for symbol in registry.nonreferenced():
                sys.stderr.write("\t- %s\n" % symbol)
            if not args.yes:
                i = raw_input("Continue anyway? [y to continue, anything else to cancel]: ")
            i = "n" if not i else i
            if not i and not args.yes:
                print("Exiting...")
                sys.exit()

    if write:
        writer = RedisWriter.RedisWriter(
            db = args.redis_db,
            port = args.redis_port,
            host = args.redis_host,
            verbal = args.v
        )
        if args.flushdb:
            i = None
            if not args.yes:
                i = raw_input("Are you sure you want to flush your entire database? [y|Y] to do so, anything else to cancel: ")
            i = "n" if not i else i
            if (i.lower() == 'y' or args.yes):
                sys.stderr.write("Flushing database %d\n" % args.redis_db)
                writer.db.flushdb()
        if (args.v):
            sys.stderr.write("Writing imported files to database!\n")
        writer.write(redis_objects)


prefix_index = {
    "widgets"       :   MoguExporter.Widget,
    "templates"     :   MoguExporter.Template,
    "policies"      :   MoguExporter.Policy,
    "validators"    :   MoguExporter.Validator,
    "data"          :   MoguExporter.Data
        }

def is_root(string):
    return len(re.findall("\.",string)) == 1

def prefix(string):
    return string.split(".")[0].strip()

def export(args):
    db = redis.Redis(
            db = args.redis_db,
            host=args.redis_host,
            port=args.redis_port
            )
    roots = filter(is_root,db.keys("*"))
    if args.output:
        os.system("touch %s" % args.output)
    for root in roots:
        cls = prefix_index[prefix(root)]
        obj = cls(root,db)
        if not args.output:
            try:
                sys.stderr.write(obj.export())
            except redis.exceptions.ResponseError as e:
                sys.stderr.write("Cannot export root: %s" % root)
                raise e
        else:
            with open(args.output,"a") as f:
                f.write(obj.export())
