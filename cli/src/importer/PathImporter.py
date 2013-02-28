import os
import FileImporter
def import_path(pathname, verbal=False):
    path_results = []
    mogu_files = [entry for entry in os.listdir(pathname) if entry.endswith(".mogu")]
    directories = [entry for entry in os.listdir(pathname) if os.path.isdir(os.path.join(pathname,entry))]
    for directory in directories:
        path_results.extend(import_path(os.path.join(pathname,directory), verbal))
    for mogufile in mogu_files:
        path_results.extend(FileImporter.import_file(os.path.join(pathname,mogufile),verbal))
    return path_results
