import PathImporter
import FileImporter
import SymbolRegistry
import PythonObjectConverter
import RedisWriter
import sys


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

