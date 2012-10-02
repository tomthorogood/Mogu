from coloring import *
import sys
from os import path
def clrln():
    sys.stdout.write("\r")
    sys.stdout.flush()

def assertending(string,ending):
    if not string.endswith(ending):
        string += ending
    return string

def removeending(string,ending):
    while string.endswith(ending):
        index = string.rfind(ending)
        string = string[:index]
    return string

def assertstart(string,beginning):
    if not string.startswith(beginning):
        string = beginning+string
    return string

def removebeginning(string,beginning):
    while string.startswith(beginning):
        string = string[len(beginning):]
    return string

def confirm(message, assume_yes=False):
    message = warn(message)
    message += "\nPlease confirm that you would like to continue. ('Y' or 'y')"
    if assume_yes:
        print(message)
        confirm = 'y'
        print info("...asuming yes.")
    else:
        confirm = raw_input(message).lower()
    return confirm == 'y'

def confirm_overwrite(filename, yes):
    mogufile    = assertending(filename, ".mogu")
    dir_name    = removeending(filename, ".mogu")
    if path.exists(mogufile) \
            or path.exists(dir_name) \
            and not yes:
                if not confirm(
                        warn(
                            "File %s or %s already exists. Are you sure you want to overwrite it?" % (mogufile,dir_name))):
                        return False
    return True
