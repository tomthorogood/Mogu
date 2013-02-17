#!/usr/bin/env python

import lex
import consumers

def parse_root(filename):
    with open(flename,"r") as f:
        text = f.read()
        return consumers.RootConsumer.parse(text)

def import(filename):
    root_results = parse_root(filename)

