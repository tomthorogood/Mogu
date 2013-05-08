import os
import FileImporter
import pyboro
import sys
def import_path(pathname, verbal=False):
    print("\n")
    path_results = []
    mogu_files = [entry for entry in os.listdir(pathname) if \
            entry.endswith(".mogu")]
    directories = [entry for entry in os.listdir(pathname) if \
            os.path.isdir(os.path.join(pathname,entry))]

    head,tail = os.path.split(pathname)
    shortname = tail
    for directory in directories:
        path_results.extend(import_path(os.path.join(pathname,directory),
            verbal))
    for i,mogufile in enumerate(mogu_files):
        # Display nice progress information so the user doesn't think
        # something is wrong
        sys.stdout.write("\r%s "%(" "*80))
        sys.stdout.flush()
        sys.stdout.write("\r%s Progress: %d%s (%s)" % (shortname, 
            ((i+1.0)/float(len(mogu_files)))*100, "%", mogufile))
        sys.stdout.flush()

        # Append lexed results to results
        path_results.extend(
                FileImporter.import_file(
                    os.path.join(pathname,mogufile),verbal))

    sys.stdout.write("\n")
    return path_results

if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser("Give me a path for testing")
    parser.add_argument(nargs="+",dest="path")
    parser.add_argument("-v",action="store_true",default=False)
    args = parser.parse_args()
    pyboro.Lexer.VERBAL=args.v
    path = args.path[0]
    import_path(path,args.v)

