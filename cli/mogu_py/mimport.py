import bytemaps as b
import redis_cheats as r
from snippets import confirm
import coloring

def import_dict (db, node, data, flags):
    """
    @param db the redis database connection
    @param prefix everything up to the specific dictionary being
        imported (widgets.numero_uno.events)
    @name The specific name of this particular dictionary
        (1) -> widgets.numero_uno.events.1
    @data the dictionary
    @flags a bit-masked integral type that will determine certain 
        specifics (merge/replace, etc.)
    There is no return type for this function.
    """
    testing = b.is_set(flags, b.DictStorage.is_test)
    if testing:
        b.set_bit(flags, b.DictStorage.assume_yes)
    for entry in data:
        entry_exists = db.hexists(node, entry)
        if entry_exists and not b.is_set(flags, b.DictStorage.is_merge):
            __continue = confirm(
                    "This field already exists in your node and will be overwritten.",
                    (b.is_set(flags, b.DictStorage.assume_yes)))
            if not __continue:
                continue
        if not testing:
            db.hset(node, entry, data[entry])

def import_list(db, node, data, flags):
    testing = b.is_set(flags, b.ListStorage.is_test)
    if testing:
        b.set_bit(flags, b.ListStorage.assume_yes)
    if isinstance(data[0], (str,int)):
        new_list = []
        merging     = b.is_set(flags, b.ListStorage.is_merge)
        
        if merging:
            no_repeat   = b.is_set(flags, b.ListStorage.merge_no_repeat)
            current_list = r.full_list(db, node)
            new_list.extend(current_list)

            for entry in data:
                if entry in new_list and no_repeat:
                    while entry in new_list:
                        new_list.remove(entry)
                new_list.append(entry)
        else:
            new_list.extend(data)
       
        for entry in new_list:
            if not testing:
                db.rpush(node, entry)
   
    elif isinstance(data[0], dict):
        if not merging:
            dictset = db.keys(node+".*")
            for n in dictset:
                db.delete(n)
        dictset = db.keys(node+".*")
        num = len(dictset)
        for entry in data:
            nn = "%s.%d" % (node,num)
            fl =0
            if testing:
                b.set_bit(fl, b.DictStorage.is_test)
            if merging:
                set_bit(fl, b.DictStorage.is_merge)
            if b.is_set(flags, b.ListStorage.assume_yes):
                set_bit(fl, b.DictStorage.assume_yes)
            import_dict(db, nn, data[num], fl)
            num += 1
