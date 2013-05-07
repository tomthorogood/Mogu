import os
import FileImporter
import pyboro

def import_path(pathname, verbal=False):
    path_results = []
    mogu_files = [entry for entry in os.listdir(pathname) if entry.endswith(".mogu")]
    directories = [entry for entry in os.listdir(pathname) if os.path.isdir(os.path.join(pathname,entry))]
    for directory in directories:
        path_results.extend(import_path(os.path.join(pathname,directory), verbal))
    for mogufile in mogu_files:
        path_results.extend(FileImporter.import_file(os.path.join(pathname,mogufile),verbal))

    for result in path_results:
        print(result)
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

