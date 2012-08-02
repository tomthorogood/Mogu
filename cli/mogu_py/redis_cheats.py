def full_list(db,widget):
    return db.lrange(widget,0,db.llen(widget))

def as_widget(widget):
    return "widgets.%s" % widget.replace("widgets.","")
