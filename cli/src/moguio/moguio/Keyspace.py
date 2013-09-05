import sys


def nonempty(obj):
    if isinstance(obj,(list,tuple)):
        obj = filter(nonempty,obj)

    if obj:
        return True
    return False

def flatten(container):
    flat = filter(nonempty,container)
    iterflat = []
    for entry in flat:
        if isinstance(entry,(list,tuple)):
            iterflat.extend(flatten(entry))
        else:
            iterflat.append(entry)
    return iterflat

def identifier(prefix, inputstr):
    fstr = "%s." % prefix
    if inputstr.startswith(fstr):
        noprefix = inputstr.split(fstr)[1]
        if "." in noprefix:
            return noprefix.split(".")[0]
        return inputstr.split(fstr)[1]
    return inputstr

def zipstring(*args):
    all_args = []
    try:
        for arg in args:
            if isinstance(arg,str):
                all_args.append(arg)
            elif isinstance(arg,(list,tuple)):
                all_args.extend(flatten(arg))
        return ".".join(all_args)
    except TypeError as e:
        sys.stderr.write("*args:%s\nall_args:%s\n" %(args, all_args))
        raise e

def keyspace(prefix, inputstr, *args):
    identity = identifier(prefix, inputstr)
    return zipstring(prefix,identity, args)


