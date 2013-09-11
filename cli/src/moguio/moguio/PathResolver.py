from Loggable import Loggable
from Loggable import LogMessage
import os

class PathResolver(Loggable):
    def __init__(self, verbose=False):
        super(PathResolver, self).__init__(verbose)
    
    def path_is_directory (self, path):
        if not os.path.exists(path):
            raise IOError("Path: \"%s\" cannot be found." % path)

        if path.endswith(".mogu"):
            self.log(
                LogMessage("PathResolver: Found single file %s" % path, 5),
                self.OUT)
            return False
        
        if not os.path.isdir(path):
            raise IOError(
                "Path \"%s\" is not a .mogu file or directory." % path)

        self.log,(
            LogMessage("PathResolver: Found directory %s" % path, 5), self.OUT)
        
        return True

    def get_filtered_files(self, path, func):
        return [os.path.join(path,f) for f in os.listdir(path) if func(f)]

    def get_files(self, path):
        files = self.get_filtered_files(path, lambda f: f.endswith(".mogu"))
        dirs = self.get_filtered_files(path,\
                lambda f: os.path.isdir(os.path.join(path,f)))

        for d in dirs:
            files.extend(self.get_files(d))
        
        files.sort()
        return files
