import syntax

def DIRECTIVE_START(token):
    d =  r"\s*%s\s*:\s*" % token
    return d

def register_widget(string):
    return string

def register_template(string):
    return string

def register_data(string):
    return string

def register_validator(string):
    return string

def register_policy(string):
    return string

def check_template_exists(string):
    return string

def check_policy_exists(string):
    return string

def valid_options (string):
    return [key for key in syntax.MoguSyntax if string in syntax.MoguSyntax[key][1]]

def trim(string):
    return string.strip()

def everything_until(string):
    return r"(.|\n)+?(?=%s)" % string

def newline_list(string):
    nl_list = string.split("\n")
    nl_list = [entry.strip() for entry in nl_list if entry is not ""]
    return nl_list
