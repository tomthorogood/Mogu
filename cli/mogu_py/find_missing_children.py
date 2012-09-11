from redis import Redis

db = Redis()

children = db.keys("*.children")
for widget_with_kids in children:
    list_of_children = db.lrange(widget_with_kids, 0, db.llen(widget_with_kids))
    for kid in list_of_children:
        if not db.exists(kid):
            print("%s is referred to by %s but does not exist" % (kid, widget_with_kids))

