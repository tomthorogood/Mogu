# TODO : Read db info from dbconfig.conf
import sys
import PythonObjectConverter
import SharedData
import FileImporter
import PathImporter
import RedisWriter

def display_undefined_symbols(registry):
    output = ""
    for symbol in registry:
        if not registry[symbol]:
            output += "\n"
            output += str(registry[symbol])
            output += "\n\tReferenced in: \n"
            for reference in registry[symbol]:
                output += "\t\t%s\n" % reference
    return output

def mogu_import(args):
    results = []    # Holds the final result of all consumption operations
    redis_objects = [] #Holds the objects that can be written to Redis
    write = not args.testing # If testing, don't actually write to Redis

    for path in args.command[1:]:
        if path.endswith(".mogu"): # If the path points to a specific file
            results.extend(FileImporter.import_file(path,args.v))
        else:   # The path is a directory
            results.extend(PathImporter.import_path(path,args.v))

    converter = PythonObjectConverter.PythonObjectConverter()
    for result in results:
        redis_objects.extend(converter.convert(result))

    # SANITY CHECKS #

    # First, make sure that all symbols referenced are defined.
    for registry in [
            SharedData.symbols
            ]:
        if not registry: # Returns false if a symbol in the registry is not defined
            sys.stderr.write(display_undefined_symbols(registry))
            sys.stderr.write("\n== REFUSING TO CONTINUE ==\n")
            sys.exit()

        if registry.nonreferenced(): # Warns the user if something was defined but never used
            i = None
            sys.stderr.write("\n== WARNING: %s contains the following symbols that are defined but never referenced == \n" %
                    registry.label)
            for symbol in registry.nonreferenced():
                sys.stderr.write("\t- %s\n" % symbol)
            # Give the user a chance to halt the import and fix the problem
            if not args.yes:
                i = raw_input("Continue anyway? [y to continue, anything else to cancel]: ")
            if i != 'y' and not args.yes:
                sys.stderr.write("Exiting...")
                sys.exit()

    # Sanity checks complete. Now the process of actually writing
    # to Redis 

    # TODO Don't forget to make RedisWriter deal with purging/merging
    if write: 
        writer = RedisWriter.RedisWriter(args) #TODO RedisWriter should read dbconfig.conf instead
        if args.v:
            sys.stderr.write("Writing imported files to database!\n")
        writer.write(redis_objects)

