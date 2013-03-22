# 29 Aug 2012
# Tom A. Thorogood
# 
# Session merging condenses the linked list of user sessions. Doing this can
# save valuable memory by purging unneeded data and keeping only the most recent
# writes for any given node.
#
# Currently, if user 'Fooman' logs in 8 times, and changes his name each time,
# his session history would like this:
#
# Session 1 -> name: 'Fooman'
# Session 2 -> name: 'Booman'
# Session 3 -> name: 'Doris'
# Session 4 -> name: 'Balthazar'
# Session 5 -> name: 'Hank'
# Session 6 -> name: 'Leeroy Jenkins'
# Session 7 -> name: 'Georgette'
# Session 8 -> name: 'Conqistador'
#
# Because of the way Mogu sessions work, when he logs in next, if the name
# node is read, it will only reach Conquistador. This means that all seven
# other nodes will be inaccessible to the user (until we write that functionality)
# and will simply be consuming memory. A merge will do the following, always 
# keeping the first session:
# Session 1 -> name: 'Fooman'
# Session 8 -> name: 'Conquistador'
#
# Doing this will also reset the session pointers:
# Whereas before, it would have looked like this:
# 
# Session 1 -> prev: global
# Session 2 -> prev: Session 1
# session 3 -> prev: Session 2 (and so on)
#
# It will now look like this:
# 
# Session 1 -> prev: global
# Session 8 -> prev: Session 1
#
from cityhash import Hash
from collections import OrderedDict
from sets import Set
from redis_cheats import full_list

def get_previous_session(db, session):
    meta_node = "s.%s.%s" % (session, Hash.meta)
    return db.hget(meta_node, Hash.prev)

def get_user_linkedlist(db, endpoint):
    """
    Traverses the user's sessions, generating a list of all the session ids.
    """
    # First, get a list of all the user's sessions, in order from 'endpoint' to 'global':
    previous = endpoint
    linkedlist = [endpoint]
    while previous != "global":
        current = previous
        previous = get_previous_session(db,previous)

        # If something has happened and we have a list that references itself, 
        # Cut ties and move on.
        if current == previous:
            previous = "global"
        linkedlist.append(previous)
    return linkedlist

def get_node_data(db, node):
    """
    Based on the Redis type of a node, populates a Python
    container with the requisite data
    """
    nodetype = db.type(node)

    if nodetype == "hash":
        return db.hgetall(node)
    elif nodetype == "set":
        return Set(db.smembers(node))
    elif nodetype == "list":
        return full_list(db, node)
    else:
        return db.get(node)

def get_session_node_data(db, session):
    """
    Finds all of the data in a session
    """
    nodes = db.keys("s.%s.*" % session)
    names = [node.replace("s.%s." % session ,"") for node in nodes]
    data = {}
    i = 0
    while i < len(nodes):
        data[names[i]] = get_node_data(db, nodes[i])        
        i+= 1
    return data

def get_user_data(db, sessionlist):
    """
    Returns a list where each entry is a dictionary of
    all of the data in a user's session.
    """
    alldata = []
    for session in sessionlist:
        alldata.append(get_session_node_data(db,session))

    return alldata

def merge_data(list_of_session_data_dicts):
    """
    The most recent session will be the first dictionary in the list.
    That is where all of the new data will be stored. However, we will
    never overwrite data in the original node, which is the last session
    in the list.
    """
    data = list_of_session_data_dicts[1:-1]
    FINAL = list_of_session_data_dicts[0]
    for session in data[1:-1]:
        for node in session:
            # If the data doesn't exist at all in the 
            # most recent node, add it in.
            if node not in FINAL:
                FINAL[node] = session[node]

            # If the node does exist in the final node, 
            # but it's a dictionary, check to see if each
            # entry exists.
            elif isinstance(session[node], dict):
                for key in session[node]:
                    if key not in FINAL[node]:
                        FINAL[node][key] = session[node][key]


def del_session(db, session):
    keys = db.keys("s.%s.*" % session)
    for key in keys:
        db.delete(key)

def write_hashkey(db, node, pydict):
    for entry in pydict:
        db.hset(node, entry, pydict[entry])

def write_listkey(db, node, pylist):
    for entry in pylist:
        db.rpush(node, entry)

def write_setkey(db, node, pyset):
    for entry in pyset:
        db.sadd(node, entry)

def write_strkey(db, node, val):
    db.set(node,val)


def write_session(db, session, data):
    print(data)
    prefix = "s.%s." % session
    func = None
    for nodename in data:
        node = "%s%s" % (prefix,nodename)
        nodetype = db.type(node)
        if isinstance(data[nodename],dict):
            func = write_hashkey
        elif isinstance(data[nodename], Set):
            func = write_setkey
        elif isinstance(data[nodename], list):
            func = write_listkey
        else:
            func = write_strkey
        func(db, node, data[nodename])


def merge_user(db, endpoint):
    linkedlist = get_user_linkedlist(db, endpoint)
    userdata = get_user_data(db, linkedlist)
    merge_data(userdata)
    userdata[0][Hash.meta][Hash.prev] = "global"
    redundant = linkedlist[1:-1]
    write_session(db, linkedlist[0], userdata[0])
    for session in redundant:
        del_session(db,session)

def merge_all(args, db):
    """
    Hashes the phrase 'session_lookup', 
    which is where Mogu stores all of the most recent sessions and 
    the usernames that go with them, and then gets the list of all 
    the sessions stored in that node. None of this data will be modified.
    """
    node = "s.global.%s" % Hash.session_lookup
    session_list = db.hvals(node)
    
    for session in session_list:
        print("Merging session with endpoint %s" %  session)
        merge_user(db, session)
        print "Finished merging session with endpoint %s" % session

    SESSION_SET = "s.global.6250022396821446324.10275542420063459543"
    for session in db.smembers(SESSION_SET):
        if session not in session_list:
            db.srem(SESSION_SET,session)

    active_tokens =[]
    AUTH_SET ="s.global.1031436179838087849.10275542420063459543" 
    for session in session_list:
        session_auth = db.hget("s.%s.%s" % (session, Hash.meta), Hash.auth)
        active_tokens.append(session_auth)

    for token in db.smembers(AUTH_SET):
        if token not in active_tokens:
            db.srem(AUTH_SET, token)

if __name__ == "__main__":
    from redis import Redis
    db = Redis()
    merge_all(1,db)
