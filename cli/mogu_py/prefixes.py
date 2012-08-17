MOGU_VALID_PREFIXES = (
        "widgets",
        "s",
        "templates",
        "events",
        "perspectives"
        )



def prefix_lookup (db, string):
    ret = []
    for prefix in MOGU_VALID_PREFIXES:
        lookup = "%s.%s.*" % (prefix, string)
        if len (db.keys(lookup)) > 0:
            ret.append(prefix)
    return ret
