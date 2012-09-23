from redis import Redis
from redis_cheats import full_list
def find_missing_children(args,db):
    children = db.keys("*.children")
    for widget_with_kids in children:
        list_of_children = full_list(db, widget_with_kids)
        for kid in list_of_children:
            if not db.exists(kid):
                print("%s is referred to by %s but does not exist" % (kid, widget_with_kids))
