from coloring import *
from redis_cheats import *
from node_names import NodePattern as Pattern
def find_parent(db,widget):
    children_lists = [key for key in db.keys("*.children")]
    for _list in children_lists:
        if widget in full_list(db, _list):
            return _list.replace(".children","")

def find_common_ancestor(db,widget1,widget2):
    widget1_path = [widget1]
    widget2_path = [widget2]
    common = False
    found = False
    while not found:
        parent = find_parent(db,widget1_path[0])
        if parent is None:
            print widget1_path
            raise Exception #TODO
        widget1_path.insert(0,parent)
        if is_descendent_of(db, parent,widget2):
            common = widget1_path[0]
            print("Common Ancestor: %s" % common)
            found = True
            break
    while not widget2_path[0] == common:
        parent = find_parent(db,widget2_path[0])
        widget2_path.insert(0,parent)
    return [widget1_path,widget2_path]



def is_descendent_of(db,parent,test):
    children = full_list(db, "%s.children" % parent)
    if test in children:
        return True
    for child in children:
        return is_descendent_of(db,child,test)
    return False

def complex_relation_analysis(db, widget1,widget2):
    tree = find_common_ancestor(db,widget1,widget2)
    signal_info = [{},{}]
    signal_info[0]['degradation'] = len(tree[0])-1
    signal_info[0]['action'] = "bubble"
    signal_info[0]['message'] = "rebroadcast_node_here"
    signal_info[0]['listeners'] = "siblings"
    signal_info[1]['degradation'] = len(tree[1])-1
    signal_info[1]['listeners'] = "children"
    signal_info[1]['action'] = "trickle"

    greater = max(len(tree[0]),len(tree[1]))
    for i in range(greater):
        if i is 0:
            print(warn("\t%s" % tree[0][0]))
        else:
            line = ""
            if len(tree[0]) >i:
                line += fail(tree[0][i])
            if len(tree[1]) >i:
                line += "\t\t"
                line += ok(tree[1][i])
            print(line)

def relate(args,db):
    widget1 = Pattern.Widget(args.command[1])
    widget2 = Pattern.Widget(args.command[2])
    parents=[None,None]
    message = None
    children_lists = [key for key in db.keys("*.children")]
    for child in (widget1.name, widget2.name):
        for  _list in children_lists:
            if child in full_list(db, _list):
                if child == widget1:
                    parents[0] = _list.replace(".children","")
                else:
                    parents[1] = _list.replace(".children","")
    if parents[0] == parents[1]:
        message = info(widget1) + " and " +info(widget2) + " are " + info("siblings")
    elif parents[0] == widget2:
        message = info(widget1) + " is a direct descendent of " + info(widget2)
    elif parents[1] == widget1:
        message = info(widget2) + " is a direct descendent of " + info(widget1) 
    else:
        complex_relation_analysis(db,widget1,widget2)
    if message:
        print(message)

