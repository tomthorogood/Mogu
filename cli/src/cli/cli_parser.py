import os
import AUTOCONFIG

from ConfigParser import SafeConfigParser
from argparse import ArgumentParser

def getCLIArgs():
    config = SafeConfigParser()
    config.read(os.path.join(AUTOCONFIG.CONFIG_DIR, "mogu.conf"))
    parser = ArgumentParser("Utility for interacting with Mogu via command line")
    parser.add_argument(
            "command", 
            nargs='+', 
            type=str, 
            action="store", 
            help="The command you want to run (see readme for available commands")
    
    parser.add_argument("--dbconfig", dest="dbconfig",
            default=os.path.join(AUTOCONFIG.CONFIG_DIR, "dbconfig.conf"))


# Boolean options for interacting with the Redis environment
    parser.add_argument("--flush", 
            dest="flushdb", action="store_true", 
            default=config.get("cli options", "always-flush").lower()=="true",
            help="If importing a Mogu database, flushes the database "\
                "first based on the rules defined in the config file.")

    parser.add_argument("-y", "--yes",
            dest="y", action="store_true",
            default=config.get("cli options", "assume-yes").lower()=="true",
            help="Will assume yes to all questions. EXTRA CAREFUL!")

    parser.add_argument("-v", "--verbose",
            dest="v", action="store_true",
            default=config.get("cli options", "verbose").lower()=="true",
            help="Will make the import command more verbose")

    args = parser.parse_args()
    args.dbroot = config.get("runtime options", "root")
    args.docroot = config.get("runtime options", "document-root")
    args.docsubs = config.get("runtime options", "document-subs")
    args.port = config.get("runtime options", "http-port")
    args.address = config.get("runtime options", "http-address")

    args.flushrules = {}
    configured_prefixes = config.options("flush configuration")
    for prefix in configured_prefixes:
        args.flushrules[prefix] = config.get("flush configuration", prefix)
    return args

if __name__ == "__main__":
    args = getCLIArgs()
    print args.docsubs
