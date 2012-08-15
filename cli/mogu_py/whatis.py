import prefixes
import widget_properties as w
import coloring

def whatis(db, string, output_str=True):
    output = ""
    mtype = prefixes.prefix_lookup(db,string)

    if not output_str:
        return mtype
    
    if len(mtype) is 0:
        return coloring.fail("That item could not be found in the database.")

    for prefix in mtype:
        if prefix == "widgets":
            output = coloring.info(string)
            output += " is a " + coloring.info("widget")
            has_children = w.has_children(db, string)
            has_events = w.has_events(db, string)
            num_children = 0
            num_events = 0
            if has_children:
                num_children = w.num_children(db, string)
                output += " that has %d " % num_children
                output += coloring.info("children")
            if has_events:
                num_events = w.num_events(db, string)
                if has_children:
                    output += " and\n %d " % num_events 
                else:
                    output += " that has %d " % num_events
                output += coloring.info("events\n")
        elif prefix == "events":
            output = coloring.info(string)
            output += " is a " + coloring.info("global event")

        elif prefix == "perspectives":
            output = coloring.info(string)
            output += " is a " + coloring.info("perspective")
            output += " with %d events " % ( len(db.keys(w.build_node("perspectives", string, "*")))-1)

        elif prefix == "s":
            output = coloring.info(string)
            output += " is a " + coloring.info("session")
    return output

#TODO ADD TEMPLATES HERE
if __name__ == "__main__":
    from redis import Redis
    db = Redis()
    print    whatis(db, "wrapper")
