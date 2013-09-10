import os
import FileImporter
import sys
import PythonObjectConverter
import SharedData
import ScriptImporter


def import_path(pathname, verbal=False, root=True):
    sys.stdout.write("\n")
    path_results = []
    mogu_files = []
    directories = []
    single_file = pathname.endswith(".mogu")
    
    if not single_file:
        mogu_files = [entry for entry in os.listdir(pathname) if \
                entry.endswith(".mogu")]
        directories = [entry for entry in os.listdir(pathname) if \
                os.path.isdir(os.path.join(pathname,entry))]
    else:
        mogu_files = [pathname]
    head,tail = os.path.split(pathname)
    shortname = tail
    for directory in directories:
        path_results.extend(import_path(os.path.join(pathname,directory),
            verbal,False))
    for i,mogufile in enumerate(mogu_files):
        # Display nice progress information so the user doesn't think
        # something is wrong
        sys.stdout.write("\r%s "%(" "*80))
        sys.stdout.flush()
        sys.stdout.write("\r%s Progress: %d%s (%s)" % (shortname, 
            ((i+1.0)/float(len(mogu_files)))*100, "%", mogufile))
        sys.stdout.flush()

        # Append lexed results to results
        if single_file:
            sys.stdout.write("Importing single file: %s" % mogufile)
            sys.stdout.flush()
            path_results.extend(FileImporter.import_file(mogufile))
        else:
            path_results.extend(
                    FileImporter.import_file(
                        os.path.join(pathname,mogufile),verbal))
    # The results will be a list of tuples, each of which will contain two
    # entries:
    # index 0 will contain the OrderedDict of token names: tokens.
    # index 1 will contain the actual map used to parse the tokens
    if root:
        registries = SharedData.symbols.values()
        for registry in registries:
            if not registry:
                raise ScriptImporter.RegistryError(registry)
            if registry.nonreferenced():
                for symbol in registry.nonreferenced():
                    sys.stderr.write(
                            "WARNING: %s is defined but never referenced\n" % \
                            (symbol)) 
    return path_results

def flatten(container):
    def is_flat(container):
        for entry in container:
            if isinstance(entry,(list,tuple)):
                return False
        return True
    flat = []
    for entry in container:
        if isinstance(entry,(list,tuple)):
            flat.extend(entry)
        else:
            flat.append(entry)
    while not is_flat(flat):
        flatten(flat)
    return flat


def convert_results(results):
    converter = PythonObjectConverter.PythonObjectConverter() 
    conversions = []
    num_conversions = 0
    for result in results:
        conversions.append(converter.convert(result))
        num_conversions += 1
        sys.stdout.write("\r%s "% (" "*80))
        sys.stdout.flush()
        sys.stdout.write("\rResults converted: %d/%d" % (num_conversions, len(results)))
    sys.stdout.write("\n")
    conversions = flatten(conversions)
    return conversions

if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser("Give me a path for testing. I'll only do one for right now.")
    parser.add_argument(nargs="+",dest="path")
    parser.add_argument("-v",action="store_true",default=False)
    parser.add_argument("--flush", action="store_true", default=False, dest="flushdb")
    parser.add_argument("--config", action="store", dest="dbconfig", help="The"
            "absoltue path to your dbconfig.conf file")
    parser.add_argument("-y", action="store_true", default=False, dest="y")
    args = parser.parse_args()
#    pyboro.Lexer.VERBAL=args.v
    path = args.path[0]
    results = import_path(path,args.v)
#    conversions = convert_results(results)
#    writer = RedisWriter.RedisWriter(args)
#    writer.write(conversions)
