"""
Populates a list of all 
"""
from cityhash import Hash

def previous_sessions(db, starting_session, linked_list=None, stop_at="global"):
    if not linked_list:
        linked_list = []
    prev_session = db.hget("s.%s.%s" % (starting_session, Hash.meta), Hash.prev)
    if prev_session != stop_at:
        linked_list.append(prev_session)
        return previous_sessions(db, prev_session, linked_list, stop_at)
    return linked_list
    
