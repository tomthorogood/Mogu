import syntax
import SymbolRegistry
import SharedData

def DIRECTIVE_START(token):
    d =  r"\s*(%s)\s*:\s*" % token
    return d

def register_widget(string):
    SymbolRegistry.widgetRegistry[string] = SharedData.ActiveFile
    SharedData.ActiveIdentifier = string
    return string

def register_template(string):
    SymbolRegistry.templateRegistry[string]= SharedData.ActiveFile
    return string

def register_data(string):
    SymbolRegistry.dataRegistry[string] = SharedData.ActiveFile
    SharedData.ActiveIdentifier = string
    return string

def register_validator(string):
    SymbolRegistry.validatorRegistry[string] = SharedData.ActiveFile
    return string

def register_policy(string):
    SymbolRegistry.policyRegistry[string] = SharedData.ActiveFile
    return string

def reference_widget(string):
    SymbolRegistry.widgetRegistry[string].append(SharedData.ActiveFile)
    return string

def reference_widget_list(string):
    w_list = newline_list(string)
    for index,entry in enumerate(w_list):
        if entry.startswith(":"):
            w_list[index] = SharedData.ActiveIdentifier+entry
            print("Expanded %s to %s" % (entry, w_list[index]))
    for w in w_list:
        reference_widget(w)
    return w_list

def reference_template(string):
    SymbolRegistry.templateRegistry[string].append(SharedData.ActiveFile)
    return string

def reference_data(string):
    SymbolRegistry.dataRegistry[string].append(SharedData.ActiveFile)
    return string

def reference_validator(string):
    SymbolRegistry.validatorRegistry[string].append(SharedData.ActiveFile)
    return string

def reference_policy(string):
    SymbolRegistry.policyRegistry[string].append(SharedData.ActiveFile)
    return string

def valid_options (string):
    """
        Returns valid integral representations of keywords.
    """
    return [str(syntax.MoguSyntax[key][0]) for key in syntax.MoguSyntax if string in syntax.MoguSyntax[key][1]]
#    return [key for key in syntax.MoguSyntax if string in syntax.MoguSyntax[key][1]]

def trim(string):
    return string.strip()

def everything_until(string):
    return r"(.|\n)+?(?=%s)" % string

def newline_list(string):
    nl_list = string.split("\n")
    nl_list = [e for e in [entry.strip() for entry in nl_list if entry] if e]
    nl_list = [i.strip() for i in nl_list]
    return nl_list
