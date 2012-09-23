import getch as get
from coloring import *
from ConfigParser import SafeConfigParser
from os import getcwd
"""
Tom A. Thorogood
11 September 2012

Either creates a new config file if one does not exist in /etc/mogu, or checks/upgrades an existing one. 
This file needs to be updated whenever the config file layout changes.
"""

class NoDefault(object):
    def __init__(self, required=False):
        self.required = required


"""
Each entry in the CONFIG_TEMPLATE is a section of the config file, which has as
its definition a tuple of tuples. Each of the inner tuples has the argument name
at index 0, and the default at index 1. If an argument has no default, a NoDefault
object is used instead, which can be set to be required by passing in True on
instantiation. Index 2 is helptext which is used when getting input from the user.
"""
CONFIG_TEMPLATE = {

        "local_server"  :  (
            (
                "port",
                "9090", 
                "The port for your Mogu instance."),
            (
                "root", 
                getcwd(),
                "The root directory of your Mogu project"),
            (
                "address", 
                "0.0.0.0", 
                "The IP address of your Mogu instance."),
            (
                "debug", 
                "on", 
                "If on, Mogu will stop when exceptions are met, otherwise it will try to recover gracefully without crashing. On is recommended.")
            ),

        "redis_instance" : (
            (
                "database", 
                "0", 
                "The database within your Redis instance you want to use by default."),
            (
                "port", 
                "6379", 
                "The port for your Redis instance."),
            (
                "host", 
                "localhost", 
                "The address of your Redis instance.")
            ),

        "meta_keys" : (
            (
                "analytics_id", 
                NoDefault(), 
                "If you have a Google Analytics account you wish to use, please enter the ID here."),
            (
                "analytics_domain", 
                NoDefault(), 
                "If you have a Google Analytics you wish to use, please enter the domain for your application here.")
            )
        }

class ConfigImporter(object):
    def __init__(self, filename=None):
        if not filename:
            self.filename = "/etc/mogu/mogu.conf"
        else:
            self.filename = filename
        self.f = None
        self.get_char = get._Getch()
        self.try_import()

    def __getitem__(self,entry):
        return self.entries[entry]

    def __str__(self):
        return "ConfigImporter : config_generator.py"

    def try_import(self):
        try:
            self.f = open(self.filename, "r")
            self.mode = "import"
        except IOError:
            self.confirm_create_new()

        self.entries = {}
        global CONFIG_TEMPLATE
        
        if self.mode == "import":
            config = SafeConfigParser()
            config.read(self.filename)
        else:
            config = None
        
        for entry in CONFIG_TEMPLATE:
            section = CONFIG_TEMPLATE[entry]
            self.entries[entry] = []
            __section = self.entries[entry]

            for possible in section:
                option = possible[0]
                default = possible[1]
                tip = possible[2]
                setting = None
                
                if config and config.has_option(entry,option):
                    setting = config.get(entry,option)
                else:
                    setting = self.get_option(entry,option,default, tip)

                __section.append( (option, setting) )

    def confirm_create_new(self):
        print( fail("The file %s does not exist. You can either:" % self.filename))
        print( info("\t[D]") + "efine the full path of your mogu.conf file or")
        print( info("\t[C]") + "reate a new file in the default location.")

        ch = self.get_char() 
        if ch.upper() == 'D':
            self.filename = raw_input("Please enter the complete path to your mogu.conf file:\n")
            return self.try_import()
        elif ch.upper() == 'C':
            self.mode = "create"
            return
        else:
            raise InvalidInputError(self, ('C','D'))

    def get_option(self, section, option, default, tip):
        print("\n\n")
        message = "%s: %s | Default is: %s"
        no_default = False
        if isinstance(default, NoDefault):
            no_default = True
            option_required = False
            if default.required:
                default_str = fail("REQUIRED")
                option_required = True
            else:
                default_str = ok("OPTIONAL")
        else:
            default_str = default

        message = message % (section,option,default_str)
        message += "\n%s" % info(tip)

        print(message)

        if not no_default:
            choice = "You can: " 
            choice += "\n\t" + info("[K]") + "eep the current option, or"
            choice += "\n\t" + info("[E]") + "dit it..."
            print(choice)
            c = self.get_char() 
            if c.upper() == 'K':
                return default
            elif c.upper() == 'E':
                defined = raw_input("Please enter your settings for %s: %s: \t" % (section,option))
            else:
                raise InvalidInputError(self, ('K','E'))
        elif option_required:
            defined = raw_input("Please enter your settings for %s: %s" % (section,option))
        else:
            defined = raw_input("If wish to set %s: %s, you may do so now. Otherwise, just press 'enter'" % (section,option))

        return defined

def write_config_file(config, filename=None):
    if not filename:
        filename = config.filename
    f = open(filename, 'w')
    for section in config.entries:
        f.write("[%s]\n" % section)
        for option in config[section]:
            if option[1] is not "" and option[1] is not None:
                f.write("%s = %s\n" % (option[0], option[1]))
        f.write("\n\n")
    f.close()

def create_file(args,db):
    write_config_file(ConfigImporter(), getcwd()+"/"+args[1])

if __name__ == "__main__":
    config = ConfigImporter()
    write_config_file(config, getcwd()+"/mogu.conf")
