#!/usr/bin/env python
from redis import Redis
from collections import OrderedDict
from syntax import syntaxMapStrings
import re

def enums(string):
    regex = "{[a-zA-Z_]+}"
    return re.findall(regex,string)

def check_syntax(args=None, db=Redis()):
    bad_syntax = OrderedDict()
    hashes = [key for key in db.keys("*") if db.type(key) == "hash"]
    for node in hashes:
        values = db.hvals(node)
        for value in values:
            syntax = enums(value)
            if syntax:
                for enum in syntax:
                    if enum not in syntaxMapStrings:
                        if node not in bad_syntax:
                            bad_syntax[node] = []
                        bad_syntax[node].append(enum)
    if not bad_syntax: return
    syntaxcheck = open("syntax_report.txt", "w")
    for node in bad_syntax:
        syntaxcheck.write("%s:\n" % node)
        for enum in bad_syntax[node]:
            syntaxcheck.write("\t%s\n" % enum)
    syntaxcheck.close()

if __name__ == "__main__":
    check_syntax()

