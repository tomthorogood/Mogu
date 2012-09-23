class TermColors:
    OK = '\033[92m'
    FAIL = '\033[91m'
    WARN = '\033[93m'
    DONE = '\033[0m'
    INFO = '\33[96m'

def tint (value,color):
    return ("%s%s%s" % (color,value,TermColors.DONE))

def info (value):
    return tint(value,TermColors.INFO)

def warn (value):
    return tint(value, TermColors.WARN)

def fail (value):
    return tint(value, TermColors.FAIL)

def ok (value):
    return tint(value, TermColors.OK)
