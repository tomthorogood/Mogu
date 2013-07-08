from ConfigParser import SafeConfigParser
import AUTOCONFIG
import os

# A generic container that will hold the final output of the
# config/cli arg parsing.
class Args(object):
    def __init__(self):
        pass

def parsedConfiguration():
    config_file = os.path.join(AUTOCONFIG.CONF_DIR, "mogu.conf")
    parser = SafeConfigParser()
    parser.read(config_file)
    return parser

args = ARGS()

clargs = cli_parser.getCLIArgs()
conf = parsedConfiguration()

args.docroot = conf.get("runtime options", "document-root")
args.docsubs = ";".join(
        conf.get("runtime options", "document-subs").split(","))

args.v = clargs.v
args.flushdb = clargs.flushdb
