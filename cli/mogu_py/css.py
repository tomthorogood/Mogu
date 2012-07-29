from redis_cheats import *
import re
def parse_classes(css):
    #first, separate all of comma-separated elements:
    css = css.replace("\n"," ")
    blocks = re.findall("(\{.*?\})",css)
    for block in blocks:
        css.replace(block," ")
    css = css.split(",")
    css_copy = []
    css_copy.extend(css)
    e = 0
    for entry in css_copy:
        css_copy[e] = entry.strip(" ")
        e+=1
        if css_copy[e][0] != '.':
            css.remove(entry)
    return css

def parse_css(db):
    folder = full_list(db,"meta.static")[0]
    _stylesheets = full_list(db,"meta.css")
    stylesheets = []
    classes = []
    for sheet in _stylesheets:
        stylesheets.append("%s%s" %(folder,sheet))
    for sheet in stylesheets:
        f = open(sheet,"r")
        text = f.read()
        classes.extend(parse_classes(text))

    return classes
