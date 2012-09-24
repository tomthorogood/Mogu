"""
Defines better syntax for mogu
"""



def enum(keyword):
    return "{%s}" % keyword
Enum = enum

def Number(num):
    if isinstance(num, int):
        return "^%d^" % num
    elif isinstance(num, float):
        return "~%f~" % num
    else:
        raise InvalidFormatException(num,int)
number = Number

def widget(name):
    return "@%s@" % name
Widget = widget

def State(name):
    return "$%s$" % name

def dynamic(wtype):
    return enum(wtype+"|dynamic")

class remove(object):
    style   = enum("remove_class")
    child   = enum("remove_child")
    value   = enum("delete_value")

class change(object):
    style   = enum("set_style")
    index   = enum("set_index")
    session = enum("change_session")
    text    = enum("set_text")

class test(object):
    text    = enum("test_text")

class key(object):
    enter   = enum("enter")

class next(object):
    action  = "nextAction"

class add(object):
    child   = enum("add_child")

class new(object):
    style   =   enum("style_changed")
    index   =   enum("index_changed")

class storage(object):
    list    = enum("list")
    string  = enum("string")
    hash    = enum("hash")

class of(object):
    none    = enum("none")
    string  = enum("string")
    node    = enum("static_node")
    dynamic = enum("dynamic_node")
    integer = enum("integer")
    float   = enum("float")
    file    = enum("file")
    enum    = enum("enum")

class animate(object):
    class start(object):
        left    = enum("slideInFromLeft")
        right   = enum("sideInFromRight")
        bottom  = enum("slideInFromBottom")
        top     = enum("slideInFromTop")
    pop     = enum("pop")
    fade    = enum("fade")

class link(object):
    text    = enum("link")
    image   = enum("image_link")

class do(object):
    events  = "events"

pytype      = type      #provides access to python's definition of type
type        = "type"    #overrides python's definition of 'type'
pyreload    = reload    #provides access to python's definition of reload
reload      = enum("reload") #overrides python's definition of 'reload'
name        = "name"
styles      = "class"   # can't override python's `class` keyword
signal      = "signal"
on          = "on"
off         = "off"
message     = "message"
trigger     = "trigger"
listeners   = "listeners"
listener    = listeners
action      = "action"
block       = "block"
location    = "location"
source      = "source"
tooltip     = "tooltip"
content     = "content"
degradation = "degradation"
strength    = degradation
animation   = "animation"
auto        = "auto"
clear       = enum("clear")

this        = enum("self")
children    = enum("children")
siblings    = enum("siblings")
parent      = enum("parent")
app         = enum("app")

bubble      = enum("bubble")
trickle     = enum("trickle")
rebroadcast = enum("rebroadcast")
store       = enum("store")
register    = enum("register_user")
slot        = enum("slot")
match       = enum("match")
TERMINATE   = enum("TERMINATE")

click       = enum("click")
mouseover   = enum("mouseover")
mouseout    = enum("mouseout")
newlook     = enum("style_changed")
fail        = enum("fail")
succeed     = enum("succeed")
keyup       = enum("keyup")

append      = enum("append")
replace     = enum("replace")

regex       = enum("regex")

text        = enum("text")
image       = enum("image")
submit      = enum("submit")
radio       = enum("radio button")
textarea    = enum("textarea")
field       = enum("input")
dropdown    = enum("dropdown")
checkbox    = enum("checkbox")
multi       = enum("multi_select")
container   = enum("container")
password    = enum("password")
stack       = enum("stack")

if __name__ == "__main__":
    widgets = {}
    events = {}

    widgets["foo"] = {
            do.events   :   on,
            type        :   text,
            content     :   "Hello, World!",
            styles      :   "red_letter green_border",
    }

    events["foo"] = [
    {
            signal  : number(209),
            listener: this,
            action  : change.style,
            message : "black_letter red_border",
            trigger : click
    }]
