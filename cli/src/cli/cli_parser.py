from ConfigParser import SafeConfigParser
from argparse import ArgumentParser
import os
from mogupy_conf import *
from exceptions import *

CURDIR = os.getcwd()

# TODO
# This will be replaced upon install within the build
# directory to instead use the config installation directory
DB_CONFIG_DIRECTORY = CURDIR

def getCLIArgs():
    parser = ArgumentParser("Utility for interacting with Mogu via command line")
    parser.add_argument(
            "command", 
            nargs='+', 
            type=str, 
            action="store", 
            help="The command you want to run (see readme for available commands")
    
    parser.add_argument("--dbconfig", dest="dbconfig",
            default=os.path.join(CURDIR,"dbconfig.conf"))


# Boolean options for interacting with the Redis environment
    parser.add_argument("--flush", 
            dest="flushdb", action="store_true", 
            default=False, 
            help="If importing a Mogu database, flushes the selected database first. CAREFUL!")

    parser.add_argument("-y", "--yes",
            dest="y", action="store_true", default=False, 
            help="Will assume yes to all questions. EXTRA CAREFUL!")

    parser.add_argument("-v", "--verbose",
            dest="v", action="store_true", default=False,
            help="Will make the import command more verbose")

    return parser.parse_args()
