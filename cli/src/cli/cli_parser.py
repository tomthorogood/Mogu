from ConfigParser import SafeConfigParser
from ConfigParser import NoOptionError
from argparse import ArgumentParser
from mogupy_conf import *
from exceptions import *

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

# Options for configuring your Redis connection
    parser.add_argument(
            "--redis-select", 
            type=int, dest="redis_db", 
            action="store", 
            default=config.get("redis_instance","database"), 
            help="Optional Redis database number (default is read from config file)")

    parser.add_argument(
            "--redis-port", 
            type=int, dest="redis_port", 
            action="store", 
            default=config.get("redis_instance","port"),
            help="Optional Redis port (default is read from config file)")

    parser.add_argument(
            "--redis-host", 
            type=str, 
            dest="redis_host", 
            action="store", 
            default=config.get("redis_instance","host"), 
            help="Optional Redis host (default is read from config file")

    parser.add_argument(
            "--verbal",
            dest = "v",
            action = "store_true",
            default=False,
            help="Mogu will talk to you more if you turn on this flag.")

    parser.add_argument(
            "--redis-auth", 
            type=str, 
            dest="redis_auth", 
            action="store", 
            default=auth,help="Optional Redis auth password (can be set in config file)")

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

    parser.add_argument("--merge",
            dest="merge", action="store_true", default=False,
            help="Will not delete nodes before setting values, which could result in unexpected behaviour, but can make things easier when adding one-offs.")

    parser.add_argument("-o",
            dest="output", action="store", default=None,
            help="Will write to this file on export if defined, otherwise will write to stderr")

    return parser.parse_args(), config
