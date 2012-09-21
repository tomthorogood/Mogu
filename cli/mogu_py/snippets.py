from coloring import *
import sys
def clrln():
    sys.stdout.write("\r")
    sys.stdout.flush()

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
