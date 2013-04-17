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
    global CONFIG_FILE    
    config = SafeConfigParser()
    config.read(CONFIG_FILE)

    try:
        auth = config.get("redis_instance", "auth")
    except NoOptionError:
        auth = None

    parser = ArgumentParser(description="The command line utility for interacting with your Mogu web aplication")
    
    parser.add_argument(
            "command", 
            nargs='+', 
            type=str, 
            action="store", 
            help="The command you want to run (see readme for available commands")
    
    parser.add_argument("--dbconfig", dest="dbconfig",
            default=os.path.join(CURDIR,"dbconfig.conf")


# Boolean options for interacting with the Redis environment
    parser.add_argument("--redis-flush", 
            dest="flushdb", action="store_true", 
            default=False, 
            help="If importing a Mogu database, flushes the selected database first. CAREFUL!")

    parser.add_argument("--assume-yes", 
            dest="yes", action="store_true", default=False, 
            help="Will assume yes to all questions. EXTRA CAREFUL!")

    parser.add_argument("-v", "--verbose",
            dest="verbose", action="store_true", default=False,
            help="Will make the import command more verbose when assuming yes")

    parser.add_argument("--test-only", 
            dest="testing", action="store_true", default=False, 
            help="Displays various information depending on the command, making no changes to your database.")


    return parser.parse_args()
