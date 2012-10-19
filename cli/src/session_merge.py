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
        print("Mergeing session with endpoint %s" %  session)
        merge_user_sessions(db, session)
        print "Finished merging session with endpoint %s" % session

def merge_user_sessions(db, endpoint):
    """
    'endpoint' is the user's most recent session. This is the main engine
    the performs the session merge.
    """
    linked_list = []
    pruned_sessions = []
    previous = endpoint
    session_data = OrderedDict()

    # Assembles the linked list of all the user's sessions.
    while previous != "global":
        thisone = previous
        linked_list.append(previous)
        metanode = "s.%s.%s" % (previous, Hash.meta)
        previous = db.hget(metanode, Hash.prev)
        if previous == thisone:
            db.hset(metanode, Hash.prev, "global")

    # Next, weed out all of the sessions that have only the 'meta'
    # data attached, meaning no data was recorded from the user. Those
    # will be skipped in all subsequent steps, since they can be deleted harmlessly.
    # The exception is, of course, the first session, which will never
    # be deleted

    for node in linked_list:
        if node in linked_list[1:-1]:
            # A prunable session will have exactly 1 key in the database:
            node_keys = db.keys("s.%s.*")
            if len(node_keys) is 1:
                pruned_sessions.append(node)
            # If a node does not need to be pruned, we can add it to the 
            # session data ordered dict, corresponding to each of the data
            # points which are stored there. 
            else:
                session_data[node] = []
                for key in node_keys:
                    # By default, these will look like s.12345.67890
                    # we only want to store '67890'
                    hashed_widget = key.replace("s.%s." % node, "")
                    session_data[node].append(hashed_widget)

    # Now, we have an ordered dictionary (with the most recent session first)
    # of each important node and the data stored there. The next task is to
    # iterate through each of these and prune old data -- that is to say,
    # if a hashed_widget exists in one node, we don't have to preserve that
    # same data stored somewhere else.

    # For each session containing data, beginning with the most recent:
    for session in session_data:
        # For each data point in that session:
        for data_point in session:
            # For all other sessions except this one:
            for older_session in session_data:
                if older_session is not session:
                    # If the data point exists, remove it
                    if data_point in session_data[older_session]:
                        session_data[older_session].remove(data_point)

    # Now, we have a dictionary of the most recent occurences of each 
    # data point that exists for this user. The next task is to
    # write all of these data points to the most recent session.
    # We can obviously skip the first session, as it already contains
    # that data.
    for session in session_data:
        if session != endpoint:
            for data_point in session_data[session]:
                # First, determine the type of data
                # living at that particular node:
                node_name = "s.%s.%s" % (session, data_point)
                write_to = "s.%s.%s" % (endpoint, data_point)
                node_type = db.type(node_name)
                
                if node_type == "string":
                    data = db.get(node_name)
                    db.set(write_to, data)

                elif node_type == "list":
                    listlen = db.llen(node_name)
                    data = db.lrange(node_name, 0, listlen)
                    for entry in data:
                        db.rpush(write_to, entry)

                elif node_type == "hash":
                    data = db.hgetall(node_name)
                    for entry in data:
                        db.hset(write_to, entry, data[entry])

                elif node_type == "set":
                    data = Set(db.smembers(node_name))
                    for entry in data:
                        db.sadd(write_to, entry)
                    

    # Great! Now get rid of absolutely everything that
    # is not the first or last nodes!


    # Remove unsued auth tokens from the collision proofing set.
    for node in linked_list[1:-1]:
        key = "s.%s.%s" % (node, Hash.meta)
        token = db.hget(key, Hash.auth)
        db.srem("s.global.%s" % Hash.all_auths, token)

    for node in linked_list[1:-1]:
        keyspace = db.keys("s.%s.*" % node)
        for key in keyspace:
            db.delete(key)

    # Lastly, we have to link the most recent session to the global session
    ssn = linked_list[-1]
    key = "s.%s.%s" % (ssn,Hash.meta)
    db.hset(key, Hash.prev, "global")
    # The very last thing we need to do is make sure the linked list 
    # remains as such. This is involves setting the 'prev' metadata in 
    # the endpoint to link back to the first session:
    db.hset("s.%s.%s" % (endpoint, Hash.meta), Hash.prev, linked_list[-1])

    # And we're done! 
if __name__ == "__main__":
    from redis import Redis
    db = Redis()
    merge_all(1,db)
