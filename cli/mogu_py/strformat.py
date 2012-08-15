def assert_prefix(string, prefix):
    return "%s.%s" % (prefix, string.replace(prefix+".",""))
