import os
import FileImporter
def import_path(pathname):
    path_results = []
    mogu_files = [entry for entry in os.listdir(pathname) if entry.endswith(".mogu")]
    directories = [entry for entry in os.listdir(pathname) if os.path.isdir(os.path.join(pathname,entry))]
    for directory in directories:
        path_results.extend(import_path(os.path.join(pathname,directory)))
    for mogufile in mogu_files:
        path_results.extend(FileImporter.import_file(os.path.join(pathname,mogufile)))
    return path_results
